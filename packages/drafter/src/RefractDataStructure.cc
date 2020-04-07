//
//  RefractDataStructure.cc
//  drafter
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "RefractDataStructure.h"

#include "SourceAnnotation.h"

#include "RefractSourceMap.h"
#include "refract/VisitorUtils.h"
#include "refract/ExpandVisitor.h"
#include "refract/PrintVisitor.h"
#include "refract/InfoElementsUtils.h"

#include "NamedTypesRegistry.h"
#include "RefractElementFactory.h"
#include "ConversionContext.h"

#include "ElementData.h"
#include "refract/ElementUtils.h"
#include "ElementInfoUtils.h"
#include "ElementComparator.h"
#include "MsonTypeSectionToApie.h"
#include "MsonMemberToApie.h"

#include "refract/VisitorUtils.h"

#include "utils/log/Trivial.h"

#include <fstream>
#include <functional>
#include <numeric> // accumulate

using namespace refract;
using namespace drafter;
using namespace drafter::utils::log;

namespace
{
    bool hasContent(const NodeInfo<mson::TypeSection>& typeSection) noexcept
    {
        return !typeSection.node->content.value.empty() || !typeSection.node->content.elements().empty();
    }

    // const auto PrimitiveType = std::true_type::value;
    const auto ComplexType = std::false_type::value;

    template <typename U>
    struct FetchSourceMap {

        snowcrash::SourceMap<U> operator()(const NodeInfo<mson::ValueMember>& valueMember) const
        {
            snowcrash::SourceMap<U> sourceMap = *NodeInfo<U>::NullSourceMap();
            sourceMap.sourceMap = valueMember.sourceMap->valueDefinition.sourceMap;
            return sourceMap;
        }

        snowcrash::SourceMap<U> operator()(const NodeInfo<mson::TypeSection>& typeSection) const
        {
            snowcrash::SourceMap<U> sourceMap = *NodeInfo<U>::NullSourceMap();
            sourceMap.sourceMap = typeSection.sourceMap->value.sourceMap;
            return sourceMap;
        }
    };

    template <typename V, bool dummy = true>
    struct CheckValueValidity {

        using Info = ElementInfo<V>;

        void operator()(const Info&, ConversionContext&) const
        {
            // do nothing
        }
    };

    template <bool dummy>
    struct CheckValueValidity<NumberElement, dummy> {

        using Info = ElementInfo<NumberElement>;

        void operator()(const Info& info, ConversionContext& context) const
        {

            auto result = LiteralTo<dsd::Number>(info.value);

            if (!std::get<0>(result)) {
                context.warn(snowcrash::Warning(
                    "invalid value format for 'number' type. please check mson specification for valid format",
                    snowcrash::MSONError,
                    info.sourceMap.sourceMap));
            }
        }
    };

    template <bool dummy>
    struct CheckValueValidity<BooleanElement, dummy> {

        using Info = ElementInfo<BooleanElement>;

        void operator()(const Info& info, ConversionContext& context) const
        {
            auto result = LiteralTo<dsd::Boolean>(info.value);

            if (!std::get<0>(result)) {
                context.warn(
                    snowcrash::Warning("invalid value for 'boolean' type. allowed values are 'true' or 'false'",
                        snowcrash::MSONError,
                        info.sourceMap.sourceMap));
            }
        }
    };

    void SetElementType(IElement& element, const mson::TypeDefinition& td)
    {
        if (!td.typeSpecification.name.symbol.literal.empty()) {
            element.element(td.typeSpecification.name.symbol.literal);
        }
    }

    std::unique_ptr<ArrayElement> MsonTypeAttributesToRefract(const mson::TypeAttributes& ta)
    {
        if (ta == 0) {
            return nullptr;
        }

        auto attr = make_element<ArrayElement>();
        auto& content = attr->get();

        if (ta & mson::RequiredTypeAttribute) {
            content.push_back(from_primitive(SerializeKey::Required));
        }
        if (ta & mson::OptionalTypeAttribute) {
            content.push_back(from_primitive(SerializeKey::Optional));
        }
        if (ta & mson::FixedTypeAttribute) {
            content.push_back(from_primitive(SerializeKey::Fixed));
        }
        if (ta & mson::FixedTypeTypeAttribute) {
            content.push_back(from_primitive(SerializeKey::FixedType));
        }
        if (ta & mson::NullableTypeAttribute) {
            content.push_back(from_primitive(SerializeKey::Nullable));
        }

        if (content.empty()) {
            return nullptr;
        }

        return attr;
    }

    mson::BaseTypeName SelectNestedTypeSpecification(const mson::TypeNames& nestedTypes)
    {
        mson::BaseTypeName defaultNestedType = mson::StringTypeName;

        // Found if type of element is specified.
        // if more types is used - fallback to "StringType"
        if (nestedTypes.size() == 1)
            defaultNestedType = nestedTypes.begin()->base;

        switch (defaultNestedType) {
            default:
                return mson::StringTypeName;
            case mson::BooleanTypeName:
            case mson::StringTypeName:
            case mson::NumberTypeName:
                return defaultNestedType;
        }
    }

    /**
     * Extract mson definitions like
     * - value
     *       - ts1
     *       - ts2
     */

    template <typename T>
    class ExtractTypeSection
    {
        using ElementType = T;
        using ValueType = typename T::ValueType;

        ElementData<T>& data;
        ConversionContext& context;

        mson::BaseTypeName elementTypeName;
        mson::BaseTypeName defaultNestedType;

        template <typename U, bool IsPrimitive = is_primitive<U>(), bool dummy = false>
        struct Fetch;

        template <typename U, bool dummy>
        struct Fetch<U, true, dummy> {
            ElementInfo<T> operator()(const NodeInfo<mson::TypeSection>& typeSection,
                ConversionContext& context,
                const mson::BaseTypeName& defaultNestedType) const
            {

                snowcrash::SourceMap<ValueType> sourceMap = FetchSourceMap<ValueType>()(typeSection);
                return ElementInfo<T>{ typeSection.node->content.value, sourceMap };
            }
        };

        template <typename U, bool dummy>
        struct Fetch<U, false, dummy> {
            ElementInfo<T> operator()(const NodeInfo<mson::TypeSection>& typeSection,
                ConversionContext& context,
                const mson::BaseTypeName& defaultNestedType) const
            {
                std::deque<std::unique_ptr<IElement> > values;
                MsonTypeSectionsToApie(
                    MakeNodeInfo(typeSection.node->content.elements(), typeSection.sourceMap->elements()),
                    values,
                    context,
                    defaultNestedType);
                return ElementInfo<T>{ std::move(values), FetchSourceMap<ValueType>()(typeSection) };
            }
        };

        template <bool dummy>
        struct Fetch<EnumElement, false, dummy> {
            ElementInfo<T> operator()(const NodeInfo<mson::TypeSection>& typeSection,
                ConversionContext& context,
                const mson::BaseTypeName& defaultNestedType) const
            {
                std::deque<std::unique_ptr<IElement> > values;
                MsonTypeSectionsToApie(
                    MakeNodeInfo(typeSection.node->content.elements(), typeSection.sourceMap->elements()),
                    values,
                    context,
                    defaultNestedType);

                for (auto& e : values)
                    if (IsLiteral(*e))
                        setFixedTypeAttribute(*e);

                return ElementInfo<T>{ std::move(values), FetchSourceMap<ValueType>()(typeSection) };
            }
        };

        template <typename U, bool IsPrimitive = is_primitive<U>(), bool dummy = false>
        struct Store;

        template <typename U, bool dummy>
        struct Store<U, true, dummy> {
            void operator()(ElementData<U>& data,
                const NodeInfo<mson::TypeSection>& typeSection,
                ConversionContext& context,
                const mson::BaseTypeName& defaultNestedType) const
            {
                // do nothing

                // Primitives should not contain members
                // this is to avoid push "empty" elements to primitives
                // it is related to test/fixtures/mson/primitive-with-members.apib
            }
        };

        template <typename U, bool dummy>
        struct Store<U, false, dummy> {
            void operator()(ElementData<U>& data,
                const NodeInfo<mson::TypeSection>& typeSection,
                ConversionContext& context,
                const mson::BaseTypeName& defaultNestedType) const
            {

                data.values.push_back(Fetch<U>()(typeSection, context, defaultNestedType));
            }
        };

        template <typename U, bool dummy = true>
        struct TypeDefinition;

        template <bool dummy>
        struct TypeDefinition<snowcrash::DataStructure, dummy> {
            const mson::TypeDefinition& operator()(const snowcrash::DataStructure& dataStructure) const
            {
                return dataStructure.typeDefinition;
            }
        };

        template <bool dummy>
        struct TypeDefinition<mson::ValueMember, dummy> {
            const mson::TypeDefinition& operator()(const mson::ValueMember& valueMember) const
            {
                return valueMember.valueDefinition.typeDefinition;
            }
        };

    public:
        template <typename U>
        ExtractTypeSection(ElementData<T>& data, ConversionContext& context, const NodeInfo<U>& sectionHolder)
            : data(data)
            , context(context)
            , elementTypeName(TypeDefinition<U>()(*sectionHolder.node).typeSpecification.name.base)
            , defaultNestedType(
                  SelectNestedTypeSpecification(TypeDefinition<U>()(*sectionHolder.node).typeSpecification.nestedTypes))
        {
        }

        void operator()(const NodeInfo<mson::TypeSection>& typeSection) const
        {
            Fetch<ElementType> fetch;

            switch (typeSection.node->klass) {
                case mson::TypeSection::MemberTypeClass:

                    Store<ElementType>()(data, typeSection, context, defaultNestedType);
                    break;

                case mson::TypeSection::SampleClass:
                    if (hasContent(typeSection))
                        data.samples.push_back(fetch(typeSection, context, defaultNestedType));
                    break;

                case mson::TypeSection::DefaultClass:
                    if (hasContent(typeSection))
                        data.defaults.push_back(fetch(typeSection, context, defaultNestedType));
                    break;

                case mson::TypeSection::BlockDescriptionClass:
                    data.descriptions.push_back(
                        DescriptionInfo{ typeSection.node->content.description, typeSection.sourceMap->description });
                    break;

                default:
                    // we are not able to get sourcemap info there
                    // It is strongly dependent on type of section.
                    // For unknown type of Element we are not able to locate SourceMap
                    // with adequate effort we do not provide it to upper level
                    throw snowcrash::Error("unknown section type", snowcrash::ApplicationError);
            }
        }
    };

    mson::BaseTypeName RefractElementTypeToMsonType(TypeQueryVisitor::ElementType type)
    {
        switch (type) {
            case TypeQueryVisitor::String:
                return mson::StringTypeName;

            case TypeQueryVisitor::Number:
                return mson::NumberTypeName;

            case TypeQueryVisitor::Boolean:
                return mson::BooleanTypeName;

            case TypeQueryVisitor::Array:
                return mson::ArrayTypeName;

            case TypeQueryVisitor::Object:
                return mson::ObjectTypeName;

            case TypeQueryVisitor::Enum:
                return mson::EnumTypeName;

            case TypeQueryVisitor::Null:
            case TypeQueryVisitor::Holder:
            case TypeQueryVisitor::Member:
            case TypeQueryVisitor::Ref:
            case TypeQueryVisitor::Extend:
            case TypeQueryVisitor::Option:
            case TypeQueryVisitor::Select:;
        };
        return mson::UndefinedTypeName;
    }

    mson::BaseTypeName GetMsonTypeFromName(const std::string& name, const ConversionContext& context)
    {
        const IElement* e = FindRootAncestor(name, context.typeRegistry());
        if (!e) {
            return mson::UndefinedTypeName;
        }

        TypeQueryVisitor query;
        VisitBy(*e, query);
        return RefractElementTypeToMsonType(query.get());
    }

    /**
     * Extract mson definitions like
     * - value enum[td1, td2]
     */

    template <typename T>
    struct ExtractTypeDefinition {

        ElementData<T>& data;
        ConversionContext& context;

        using TypeDefinition = NodeInfo<mson::TypeDefinition>;

        template <typename E, bool IsPrimitive = is_primitive<E>()>
        struct Fetch;

        template <typename E>
        struct Fetch<E, ComplexType> {
            ElementInfo<T> operator()(const TypeDefinition& typeDefinition, ConversionContext& context) const
            {
                std::deque<std::unique_ptr<IElement> > types;

                auto typeNames = typeDefinition.node->typeSpecification.nestedTypes;

                for (const auto& type : typeNames) {
                    mson::BaseTypeName typeName = type.base;
                    FactoryCreateMethod method = eValue;

                    if (typeName == mson::UndefinedTypeName && !type.symbol.literal.empty()) {
                        typeName = GetMsonTypeFromName(type.symbol.literal, context);
                        method = type.symbol.variable ? eSample : eElement;

                        if (typeName == mson::UndefinedTypeName
                            && method == eElement) { // undefined named element in hinting
                            std::stringstream msg;
                            msg << "Undefined named type '" << type.symbol.literal << "' referenced in type definition";
                            context.warn(snowcrash::Warning(
                                msg.str().c_str(), snowcrash::MSONError, typeDefinition.sourceMap->sourceMap));
                            continue;
                        }
                    }

                    const RefractElementFactory& f = FactoryFromType(typeName);
                    types.push_back(f.Create(type.symbol.literal, method));
                }

                return ElementInfo<T>{ std::move(types), *NodeInfo<typename T::ValueType>::NullSourceMap() };
            }
        };

        template <typename E, bool IsPrimitive = is_primitive<E>(), bool dummy = true>
        struct Store;

        template <typename E, bool dummy>
        struct Store<E, true, dummy> {
            void operator()(
                ElementData<E>& data, const TypeDefinition& typeDefinition, const ConversionContext& context) const
            {
                // do nothing - Primitive types does not contain TypeDefinitions
            }
        };

        template <typename E, bool dummy>
        struct Store<E, false, dummy> {
            void operator()(
                ElementData<E>& data, const TypeDefinition& typeDefinition, ConversionContext& context) const
            {
                data.hints.push_back(Fetch<E>()(typeDefinition, context));
            }
        };

        ExtractTypeDefinition(ElementData<T>& data, ConversionContext& context) : data(data), context(context) {}

        void operator()(const TypeDefinition& typeDefinition) const
        {
            Store<T>()(data, typeDefinition, context);
        }
    };

    /**
     * Extract mson definitions like
     * - value: vm1, vm2
     */
    template <typename T, typename V = typename T::ValueType>
    struct ExtractValueMember {

        ElementData<T>& data;
        ConversionContext& context;

        template <typename U, bool IsPrimitive = is_primitive<U>(), bool dummy = true>
        struct Fetch;

        template <typename U, bool dummy>
        struct Fetch<U, true, dummy> { // primitive values

            ElementInfo<T> operator()(const NodeInfo<mson::ValueMember>& valueMember, ConversionContext& context) const
            {
                if (valueMember.node->valueDefinition.values.size() > 1) {
                    throw snowcrash::Error("only one value is supported for primitive types",
                        snowcrash::MSONError,
                        valueMember.sourceMap->sourceMap);
                }

                const mson::Value& value = *valueMember.node->valueDefinition.values.begin();
                snowcrash::SourceMap<V> sourceMap = FetchSourceMap<V>()(valueMember);

                return ElementInfo<T>{ value.literal, sourceMap };
            }
        };

        template <typename U, bool dummy>
        struct Fetch<U, false, dummy> { // Array|Object

            ElementInfo<T> operator()(const NodeInfo<mson::ValueMember>& valueMember, ConversionContext& context) const
            {

                const mson::BaseTypeName type = SelectNestedTypeSpecification(
                    valueMember.node->valueDefinition.typeDefinition.typeSpecification.nestedTypes);

                const RefractElementFactory& factory = FactoryFromType(type);
                const mson::Values& values = valueMember.node->valueDefinition.values;

                std::deque<std::unique_ptr<IElement> > elements;

                std::transform(
                    values.begin(), values.end(), std::back_inserter(elements), [&factory](const mson::Value& value) {
                        return factory.Create(value.literal, value.variable ? eSample : eValue);
                    });

                return ElementInfo<T>{ std::move(elements), FetchSourceMap<V>()(valueMember) };
            }
        };

        template <bool dummy>
        struct Fetch<EnumElement, false, dummy> { // Enum

            ElementInfo<T> operator()(const NodeInfo<mson::ValueMember>& valueMember, ConversionContext& context) const
            {

                const mson::BaseTypeName type = SelectNestedTypeSpecification(
                    valueMember.node->valueDefinition.typeDefinition.typeSpecification.nestedTypes);

                const RefractElementFactory& f = FactoryFromType(type);
                const mson::Values& values = valueMember.node->valueDefinition.values;

                std::deque<std::unique_ptr<IElement> > elements;

                for (const auto& value : values) {
                    auto entry = f.Create(value.literal, eValue);
                    if (IsLiteral(*entry))
                        setFixedTypeAttribute(*entry);
                    elements.push_back(std::move(entry));
                }

                return ElementInfo<T>{ std::move(elements), FetchSourceMap<V>()(valueMember) };
            }
        };

        template <typename Y, bool IsIterable = dsd::is_iterable<Y>::value>
        struct IsValueVariable {
            bool operator()(const mson::Value& value) const
            {
                return value.variable;
            }
        };

        template <typename Y>
        struct IsValueVariable<Y, true> {
            bool operator()(const mson::Value&) const
            {
                return false;
            }
        };

        ExtractValueMember(ElementData<T>& data, ConversionContext& context, const mson::BaseTypeName)
            : data(data), context(context)
        {
        }

        static bool hasValueForObject(const NodeInfo<mson::ValueMember>& valueMember)
        {
            return valueMember.node->valueDefinition.typeDefinition.baseType == mson::ImplicitObjectBaseType
                || valueMember.node->valueDefinition.typeDefinition.baseType == mson::ObjectBaseType;
        }

        void operator()(const NodeInfo<mson::ValueMember>& valueMember) const
        {
            // silently ignore "value" for ObjectElement e.g.
            // ```
            // # A (array)
            // - key (object)
            // ```
            // `key` - value "defintion for object" warning is already created in snowcrash
            //  so we can sillently ignore it
            if (hasValueForObject(valueMember)) {
                return;
            }

            Fetch<T> fetch;
            mson::TypeAttributes attrs = valueMember.node->valueDefinition.typeDefinition.attributes;

            if (!valueMember.node->valueDefinition.values.empty()) {
                const mson::Value& value = *valueMember.node->valueDefinition.values.begin();

                if (attrs & mson::DefaultTypeAttribute) {
                    data.defaults.push_back(fetch(valueMember, context));
                } else if ((attrs & mson::SampleTypeAttribute) || IsValueVariable<typename T::ValueType>()(value)) {
                    data.samples.push_back(fetch(valueMember, context));
                } else {
                    data.inlines.push_back(fetch(valueMember, context));
                }
            } else {
                if (attrs & mson::DefaultTypeAttribute) {
                    context.warn(snowcrash::Warning("no value present when 'default' is specified",
                        snowcrash::MSONError,
                        valueMember.sourceMap->sourceMap));
                }

                if (attrs & mson::SampleTypeAttribute) {
                    context.warn(snowcrash::Warning("no value present when 'sample' is specified",
                        snowcrash::MSONError,
                        valueMember.sourceMap->sourceMap));
                }
            }

            if (!valueMember.node->description.empty()) {
                data.descriptions.push_back(
                    DescriptionInfo{ valueMember.node->description, valueMember.sourceMap->description });
            }

            if ((valueMember.node->valueDefinition.values.empty()
                    || (valueMember.node->valueDefinition.typeDefinition.typeSpecification.nestedTypes.size() > 1))) {

                ExtractTypeDefinition<T> extd(data, context);
                extd(MakeNodeInfoWithoutSourceMap(valueMember.node->valueDefinition.typeDefinition));
            }
        }
    };

    template <typename E, bool IsPrimitive = is_primitive<E>(), bool dummy = true>
    struct ElementInfoToElement;

    template <typename E, bool dummy>
    struct ElementInfoToElement<E, true, dummy> {
        std::unique_ptr<E> operator()(ElementInfo<E>&& info) const
        {
            auto result = LiteralTo<typename E::ValueType>(info.value);
            return result.first ? make_element<E>(result.second) : make_empty<E>();
        }
    };

    template <typename E, bool dummy>
    struct ElementInfoToElement<E, false, dummy> {
        std::unique_ptr<E> operator()(ElementInfo<E>&& info) const
        {
            auto result = make_element<E>();
            std::move(info.value.begin(), info.value.end(), std::back_inserter(result->get()));
            return result;
        }
    };

    template <bool dummy>
    struct ElementInfoToElement<EnumElement, false, dummy> {
        std::unique_ptr<EnumElement> operator()(ElementInfo<EnumElement>&& info) const
        {

            auto& v = info.value;

            if (v.size() == 1) {
                auto result = make_element<EnumElement>(std::move(v.front()));
                v.pop_front(); // front has been moved from - remove garbage
                return result;
            } else if (v.size() > 1) {
                auto result = make_empty<EnumElement>();
                auto enumerations = make_element<ArrayElement>();
                std::move(v.begin(), v.end(), std::back_inserter(enumerations->get()));
                result->attributes().set(SerializeKey::Enumerations, std::move(enumerations));
                return result;
            }

            return make_empty<EnumElement>();
        }
    };

    template <typename T, bool IsPrimitive = is_primitive<T>()>
    struct SaveValue;

    template <typename T>
    struct SaveValue<T, true> {
        void operator()(ElementData<T>& data, T& element, ConversionContext& /* context */) const
        {
            if (data.inlines.empty() && data.values.empty()) {
                return;
            }

            auto inlines = Merge<T>()(std::move(data.inlines));
            auto values = Merge<T>()(std::move(data.values));

            auto result = data.values.empty() ? LiteralTo<typename T::ValueType>(inlines.value) :
                                                LiteralTo<typename T::ValueType>(values.value);

            // if(result.first) // FIXME: @tjanc@ uncomment to be specification compliant
            element.set(result.second);

            // FIXME: refactoring adept - AttachSourceMap require NodeInfo, let it pass for now
            AttachSourceMap(
                element, MakeNodeInfo(result.second, data.values.empty() ? inlines.sourceMap : values.sourceMap));
        }
    };

    template <typename T>
    struct SaveValue<T, false> {
        void operator()(ElementData<T>& data, T& element, ConversionContext& /* context */) const
        {
            auto inlines = Merge<T>()(std::move(data.inlines));
            auto info = Merge<T>()(std::move(data.values));
            auto hint = Merge<T>()(std::move(data.hints));

            if (!(hint.value.empty() && inlines.value.empty() && info.value.empty()) && element.empty()) {
                element.set();
            }

            std::move(hint.value.begin(), hint.value.end(), std::back_inserter(element.get()));
            std::move(inlines.value.begin(), inlines.value.end(), std::back_inserter(element.get()));
            std::move(info.value.begin(), info.value.end(), std::back_inserter(element.get()));
        }
    };

    template <>
    struct SaveValue<EnumElement> {
        using T = EnumElement;

        void operator()(ElementData<T>& data, T& element, ConversionContext& context) const
        {
            if ((data.inlines.size() >= 1) && (data.inlines.front().value.size() == 1)
                && !data.inlines.front().value.front()->empty()) {
                auto content = data.inlines.front().value.front()->clone();
                element.set(dsd::Enum{ std::move(content) });
            } else if ((data.values.size() >= 1) && (data.values.front().value.size() == 1)
                && !data.values.front().value.front()->empty()) {
                auto content = data.values.front().value.front()->clone();
                element.set(dsd::Enum{ std::move(content) });
            }

            auto inlines = Merge<EnumElement>()(std::move(data.inlines));
            auto values = Merge<EnumElement>()(std::move(data.values));
            auto hints = Merge<EnumElement>()(std::move(data.hints));
            auto samples = Merge<EnumElement>()(CloneElementInfoContainer<T>(data.samples));
            auto defaults = Merge<EnumElement>()(CloneElementInfoContainer<T>(data.defaults));

            dsd::Array enums;

            auto addToEnumerations = [](std::unique_ptr<IElement>& info,
                                         dsd::Array& enums,
                                         ConversionContext& context,
                                         const snowcrash::SourceMap<typename EnumElement::ValueType>& sourceMap,
                                         const bool reportDuplicity) {
                if (std::find_if(enums.begin(),
                        enums.end(),
                        [&info](std::unique_ptr<IElement>& enm) {
                            return Equal<detail::IgnoreKeys>(*info,
                                *enm,
                                detail::IgnoreKeys({ "sourceMap" }),
                                detail::IgnoreKeys({ "description" }));
                        })
                    == enums.end()) {

                    enums.push_back(std::move(info));
                } else if (reportDuplicity) {
                    context.warn(
                        snowcrash::Warning("duplicit value in enumeration", snowcrash::MSONError, sourceMap.sourceMap));
                }
            };

            // clean `inlines` which are later defined in `values`
            // to avoid reporting duplicity
            // in definitions like
            // ```
            // - enumerable: a (enum)
            // + Members
            //   + a
            //   + b
            //   + c
            // ```

            inlines.value.erase(std::remove_if(inlines.value.begin(),
                                    inlines.value.end(),
                                    [&values](const std::unique_ptr<IElement>& in) {
                                        return std::find_if(values.value.begin(),
                                                   values.value.end(),
                                                   [&in](const std::unique_ptr<IElement>& val) {
                                                       return Equal<detail::IgnoreKeys>(*val,
                                                           *in,
                                                           detail::IgnoreKeys({ "sourceMap" }),
                                                           detail::IgnoreKeys({ "description" }));
                                                   })
                                            != values.value.end();
                                    }),
                inlines.value.end());

            std::for_each(inlines.value.begin(),
                inlines.value.end(),
                [&inlines, &addToEnumerations, &enums, &context](std::unique_ptr<IElement>& info) {
                    addToEnumerations(info, enums, context, inlines.sourceMap, true);
                });
            std::for_each(values.value.begin(),
                values.value.end(),
                [&values, &addToEnumerations, &enums, &context](std::unique_ptr<IElement>& info) {
                    addToEnumerations(info, enums, context, values.sourceMap, true);
                });
            std::for_each(samples.value.begin(),
                samples.value.end(),
                [&samples, &addToEnumerations, &enums, &context](std::unique_ptr<IElement>& info) {
                    addToEnumerations(info, enums, context, samples.sourceMap, false);
                });
            std::for_each(defaults.value.begin(),
                defaults.value.end(),
                [&defaults, &addToEnumerations, &enums, &context](std::unique_ptr<IElement>& info) {
                    addToEnumerations(info, enums, context, defaults.sourceMap, false);
                });
            std::for_each(hints.value.begin(),
                hints.value.end(),
                [&hints, &addToEnumerations, &enums, &context](std::unique_ptr<IElement>& info) {
                    addToEnumerations(info, enums, context, hints.sourceMap, true);
                });

            if (!enums.empty()) {
                std::for_each(enums.begin(), enums.end(), [](std::unique_ptr<IElement>& info) {
                    if (IsLiteral(*info)) {
                        AppendInfoElement<ArrayElement>(info->attributes(), "typeAttributes", dsd::String{ "fixed" });
                    }
                });
                auto enumsElement = make_element<ArrayElement>(enums);
                element.attributes().set(SerializeKey::Enumerations, std::move(enumsElement));
            }
        }
    };

    template <typename T, typename U>
    void AllElementsToAtribute(U values, const std::string& key, IElement& element)
    {
        if (values.empty()) {
            return;
        }

        auto a = make_element<ArrayElement>();

        ElementInfoToElement<T> fetch;
        std::transform(std::make_move_iterator(values.begin()),
            std::make_move_iterator(values.end()),
            std::back_inserter(a->get()),
            fetch);

        element.attributes().set(key, std::move(a));
    }

    template <>
    void AllElementsToAtribute<refract::EnumElement, ElementInfoContainer<refract::EnumElement> >(
        ElementInfoContainer<refract::EnumElement> values, const std::string& key, IElement& element)
    {
        using T = refract::EnumElement;

        auto info = Merge<T>()(std::move(values));
        auto a = make_element<ArrayElement>();

        if (info.value.empty()) {
            return;
        }

        std::transform(std::make_move_iterator(info.value.begin()),
            std::make_move_iterator(info.value.end()),
            std::back_inserter(a->get()),
            [](std::unique_ptr<IElement> node) { return make_element<EnumElement>(dsd::Enum{ std::move(node) }); });

        element.attributes().set(key, std::move(a));
    }

    template <typename T, typename U>
    void LastElementToAttribute(U values, const std::string& key, IElement& element, ConversionContext& /* dummy */)
    {

        if (values.empty()) {
            return;
        }

        ElementInfoToElement<T> fetch;
        element.attributes().set(key, fetch(std::move(values.back())));
    }

    void CheckForMultipleDefaultDefinitions(
        const ElementInfoContainer<refract::EnumElement>& values, ConversionContext& context)
    {
        if (values.empty()) {
            return;
        }

        if ((values.size() > 1) || (values.front().value.size() > 1)) {

            mdp::CharactersRangeSet location;
            for (const auto& item : values) {
                location.append(item.sourceMap.sourceMap);
            }

            context.warn(snowcrash::Warning("multiple definitions of 'default' value", snowcrash::MSONError, location));
        }
    }

    template <>
    void LastElementToAttribute<refract::EnumElement, ElementInfoContainer<refract::EnumElement> >(
        ElementInfoContainer<refract::EnumElement> values,
        const std::string& key,
        IElement& element,
        ConversionContext& context)
    {
        using T = refract::EnumElement;

        CheckForMultipleDefaultDefinitions(values, context);

        if (values.empty()) {
            return;
        }

        auto info = Merge<T>()(std::move(values));

        if (info.value.empty()) {
            element.attributes().set(key, make_empty<EnumElement>());
            return;
        }

        element.attributes().set(key, make_element<EnumElement>(dsd::Enum{ std::move(info.value.back()) }));
    }

    template <typename T>
    void ElementDataToElement(T& element, ElementData<T> data, ConversionContext& context)
    {
        auto validate = [&context](const ElementInfo<T>& val) {
            CheckValueValidity<T> validate;
            validate(val, context);
        };

        std::for_each(data.inlines.begin(), data.inlines.end(), validate);
        std::for_each(data.values.begin(), data.values.end(), validate);
        std::for_each(data.samples.begin(), data.samples.end(), validate);
        std::for_each(data.defaults.begin(), data.defaults.end(), validate);
        std::for_each(data.hints.begin(), data.hints.end(), validate);

        SaveValue<T>()(data, element, context);
        AllElementsToAtribute<T>(std::move(data.samples), SerializeKey::Samples, element);
        LastElementToAttribute<T>(std::move(data.defaults), SerializeKey::Default, element, context);
    }

    std::unique_ptr<IElement> DescriptionToRefract(const DescriptionInfoContainer& descriptions)
    {
        if (descriptions.empty()) {
            return nullptr;
        }

        auto info = std::accumulate(descriptions.cbegin(),
            descriptions.cend(),
            DescriptionInfo(),
            [](DescriptionInfo& info, typename DescriptionInfoContainer::const_reference item) {
                if (item.description.empty())
                    return info;

                if (!info.description.empty())
                    info.description.append("\n");

                info.description.append(item.description);
                info.sourceMap.sourceMap.append(item.sourceMap.sourceMap);

                return info;
            });

        if (info.description.empty()) {
            return nullptr;
        }

        return PrimitiveToRefract(NodeInfo<std::string>(&info.description, &info.sourceMap));
    }

    // FIXME: refactoring - description is not used while calling from
    // `RefractElementFromProperty()`
    template <typename T>
    std::unique_ptr<IElement> RefractElementFromValue(const NodeInfo<mson::ValueMember>& value,
        ConversionContext& context,
        mson::BaseTypeName defaultNestedType,
        DescriptionInfoContainer& descriptions)
    {
        using ElementType = T;

        ElementData<ElementType> data;
        auto element = make_empty<ElementType>();

        ExtractValueMember<ElementType>(data, context, defaultNestedType)(value);

        SetElementType(*element, value.node->valueDefinition.typeDefinition);
        AttachSourceMap(*element, value);

        NodeInfoCollection<mson::TypeSections> typeSections(MAKE_NODE_INFO(value, sections));

        std::for_each(typeSections.begin(), typeSections.end(), ExtractTypeSection<T>(data, context, value));

        descriptions = std::move(data.descriptions);
        ElementDataToElement(*element, std::move(data), context);

        return std::move(element);
    }

    bool VariablePropertyIsString(const mson::ValueDefinition& variable, ConversionContext& context)
    {
        if (variable.typeDefinition.typeSpecification.name.base == mson::StringTypeName) {
            return true;
        }

        if (TypeQueryVisitor::as<StringElement>(FindRootAncestor(
                variable.typeDefinition.typeSpecification.name.symbol.literal, context.typeRegistry()))) {
            return true;
        }

        return false;
    }

    std::unique_ptr<StringElement> GetPropertyKey(
        const NodeInfo<mson::PropertyMember>& property, ConversionContext& context)
    {

        auto key = make_empty<StringElement>();

        snowcrash::SourceMap<mson::Literal> sourceMap;
        sourceMap.sourceMap.append(property.sourceMap->name.sourceMap);

        if (!property.node->name.variable.empty()) {

            if (property.node->name.variable.values.size() > 1) {
                // FIXME: is there example for multiple variables?
                context.warn(snowcrash::Warning("multiple variables in property definition is not implemented",
                    snowcrash::MSONError,
                    sourceMap.sourceMap));
            }

            // variable containt type definition
            if (!property.node->name.variable.typeDefinition.empty()) {
                if (!VariablePropertyIsString(property.node->name.variable, context)) {
                    throw snowcrash::Error("'variable named property' must be string or its sub-type",
                        snowcrash::MSONError,
                        sourceMap.sourceMap);
                }

                SetElementType(*key, property.node->name.variable.typeDefinition);
            }

            if (!property.node->name.variable.values.empty()) {
                key->set(property.node->name.variable.values.begin()->literal);
            }
        }

        if (!property.node->name.literal.empty()) {
            key->set(property.node->name.literal);
        }

        AttachSourceMap(*key, MakeNodeInfo(property.node->name.literal, sourceMap));

        return key;
    }

    bool validKey(const StringElement& element)
    {
        // we don't have references resolved;
        // named elements can't be validated
        if (!isReserved(element.element()))
            return true;

        if (const StringElement* value = get<const StringElement>(findValue(element)))
            // empty strings SHOULD NOT occur
            // TODO @tjanc@ decide whether too strict
            if (!value->get().get().empty())
                return true;

        return false;
    }

    template <typename T>
    std::unique_ptr<MemberElement> RefractElementFromProperty(const NodeInfo<mson::PropertyMember>& property,
        ConversionContext& context,
        const mson::BaseTypeName defaultNestedType)
    {

        DescriptionInfoContainer dummy; // we need no this
        DescriptionInfoContainer descriptions;

        auto key = GetPropertyKey(property, context);
        if (!key || !validKey(*key))
            return nullptr;

        auto element = make_element<MemberElement>(std::move(key),
            RefractElementFromValue<T>(
                NodeInfo<mson::ValueMember>(property.node, property.sourceMap), context, defaultNestedType, dummy));

        if (!property.node->name.variable.empty()) {
            element->attributes().set("variable", from_primitive(true));
        }

        mson::TypeAttributes attrs = property.node->valueDefinition.typeDefinition.attributes;

        // there is no source map for attributes
        if (auto attributes = MsonTypeAttributesToRefract(attrs)) {
            element->attributes().set(SerializeKey::TypeAttributes, std::move(attributes));
        }

        if (!property.node->description.empty()) {
            descriptions.push_back(DescriptionInfo{ property.node->description, property.sourceMap->description });
        }

        NodeInfoCollection<mson::TypeSections> typeSections(MAKE_NODE_INFO(property, sections));

        for (const auto& typeSection : typeSections) {
            if (typeSection.node->klass == mson::TypeSection::BlockDescriptionClass) {
                descriptions.push_back(
                    DescriptionInfo{ typeSection.node->content.description, typeSection.sourceMap->description });
            }
        }

        if (!property.node->description.empty() && (descriptions.size() > 1)) {
            descriptions[0].description.append("\n");
        }

        if (auto description = DescriptionToRefract(descriptions)) {
            element->meta().set(SerializeKey::Description, std::move(description));
        }

        return element;
    }

    bool ValueHasMembers(const mson::ValueMember* value)
    {
        return std::find_if(value->sections.cbegin(),
                   value->sections.cend(),
                   [](typename mson::TypeSections::const_reference item) {
                       return item.klass == mson::TypeSection::MemberTypeClass;
                   })
            != value->sections.cend();
    }

    bool ValueHasChildren(const mson::ValueMember* value)
    {
        return value->valueDefinition.values.size() > 1;
    }

    bool ValueHasName(const mson::ValueMember* value)
    {
        return !value->valueDefinition.typeDefinition.typeSpecification.name.symbol.literal.empty();
    }

    struct PropertyTrait {
        typedef MemberElement ElementType;
        typedef NodeInfo<mson::PropertyMember> InputType;

        template <typename T>
        static std::unique_ptr<ElementType> Invoke(
            const InputType& prop, ConversionContext& context, const mson::BaseTypeName defaultNestedType)
        {
            return RefractElementFromProperty<T>(prop, context, defaultNestedType);
        }
    };

    struct ValueTrait {
        typedef IElement ElementType;
        typedef NodeInfo<mson::ValueMember> InputType;

        template <typename T>
        static std::unique_ptr<ElementType> Invoke(
            const InputType& val, ConversionContext& context, const mson::BaseTypeName defaultNestedType)
        {
            DescriptionInfoContainer descriptions;

            auto element = RefractElementFromValue<T>(val, context, defaultNestedType, descriptions);

            mson::TypeAttributes attrs = val.node->valueDefinition.typeDefinition.attributes;

            // there is no source map for attributes
            if (auto attributes = MsonTypeAttributesToRefract(attrs)) {
                element->attributes().set(SerializeKey::TypeAttributes, std::move(attributes));
            }

            if (auto description = DescriptionToRefract(descriptions)) {
                element->meta().set(SerializeKey::Description, std::move(description));
            }

            return element;
        }
    };

    void CheckTypeAttributesClash(const mson::TypeAttributes& attributes,
        const snowcrash::SourceMap<mson::ValueDefinition>& sourceMap,
        ConversionContext& context)
    {

        if ((attributes & mson::FixedTypeAttribute) != 0 && (attributes & mson::OptionalTypeAttribute) != 0) {

            context.warn(snowcrash::Warning(
                "cannot use 'fixed' and 'optional' together", snowcrash::MSONError, sourceMap.sourceMap));
        }

        if ((attributes & mson::RequiredTypeAttribute) != 0 && (attributes & mson::OptionalTypeAttribute) != 0) {

            context.warn(snowcrash::Warning(
                "cannot use 'required' and 'optional' together", snowcrash::MSONError, sourceMap.sourceMap));
        }

        if ((attributes & mson::DefaultTypeAttribute) != 0 && (attributes & mson::SampleTypeAttribute) != 0) {

            context.warn(snowcrash::Warning(
                "cannot use 'default' and 'sample' together", snowcrash::MSONError, sourceMap.sourceMap));
        }

        if ((attributes & mson::FixedTypeAttribute) != 0 && (attributes & mson::FixedTypeTypeAttribute) != 0) {

            context.warn(snowcrash::Warning(
                "cannot use 'fixed' and 'fixed-type' together", snowcrash::MSONError, sourceMap.sourceMap));
        }
    }

    template <typename Trait>
    std::unique_ptr<IElement> MsonMemberToRefract(const typename Trait::InputType& input,
        ConversionContext& context,
        const mson::BaseTypeName nameType,
        const mson::BaseTypeName defaultNestedType,
        bool checkTypeAttributes = true)
    {

        if (checkTypeAttributes) {
            CheckTypeAttributesClash(
                input.node->valueDefinition.typeDefinition.attributes, input.sourceMap->valueDefinition, context);
        }

        switch (nameType) {
            case mson::BooleanTypeName:
                return Trait::template Invoke<BooleanElement>(input, context, defaultNestedType);

            case mson::NumberTypeName:
                return Trait::template Invoke<NumberElement>(input, context, defaultNestedType);

            case mson::StringTypeName:
                return Trait::template Invoke<StringElement>(input, context, defaultNestedType);

            case mson::EnumTypeName:
                return Trait::template Invoke<EnumElement>(input, context, defaultNestedType);

            case mson::ArrayTypeName:
                return Trait::template Invoke<ArrayElement>(input, context, defaultNestedType);

            case mson::ObjectTypeName:
                return Trait::template Invoke<ObjectElement>(input, context, defaultNestedType);

            case mson::UndefinedTypeName: {
                if (ValueHasChildren(input.node)) {
                    // FIXME: what about EnumElement
                    return Trait::template Invoke<ArrayElement>(input, context, defaultNestedType);
                } else if (ValueHasName(input.node) || ValueHasMembers(input.node)) {
                    return Trait::template Invoke<ObjectElement>(input, context, defaultNestedType);
                } else if (nameType != defaultNestedType) {
                    return MsonMemberToRefract<Trait>(input, context, defaultNestedType, defaultNestedType, false);
                }

                return MsonMemberToRefract<Trait>(input, context, mson::StringTypeName, defaultNestedType, false);
            }
        }

        throw snowcrash::Error("unknown type of mson member", snowcrash::MSONError, input.sourceMap->sourceMap);
    }
}

namespace
{
    template <typename T>
    std::unique_ptr<IElement> RefractElementFromMSON(
        const NodeInfo<snowcrash::DataStructure>& ds, ConversionContext& context)
    {
        using ElementType = T;

        auto element = make_empty<ElementType>();
        SetElementType(*element, ds.node->typeDefinition);

        if (!ds.node->name.symbol.literal.empty()) {
            snowcrash::SourceMap<mson::Literal> sourceMap = *NodeInfo<mson::Literal>::NullSourceMap();
            sourceMap.sourceMap.append(ds.sourceMap->name.sourceMap);
            element->meta().set(
                SerializeKey::Id, PrimitiveToRefract(MakeNodeInfo(ds.node->name.symbol.literal, sourceMap)));
        }

        AttachSourceMap(*element, MakeNodeInfo(ds.node, ds.sourceMap));

        // there is no source map for attributes
        if (auto attributes = MsonTypeAttributesToRefract(ds.node->typeDefinition.attributes)) {
            element->attributes().set(SerializeKey::TypeAttributes, std::move(attributes));
        }

        ElementData<ElementType> data;

        ExtractTypeDefinition<ElementType> extd(data, context);
        extd(MAKE_NODE_INFO(ds, typeDefinition));

        NodeInfoCollection<mson::TypeSections> typeSections(MAKE_NODE_INFO(ds, sections));

        std::for_each(typeSections.begin(), typeSections.end(), ExtractTypeSection<T>(data, context, ds));

        if (auto description = DescriptionToRefract(std::move(data.descriptions))) {
            element->meta().set(SerializeKey::Description, std::move(description));
        }

        ElementDataToElement(*element, std::move(data), context);

        return std::move(element);
    }
} // namespace

std::unique_ptr<IElement> drafter::MSONToRefract(
    const NodeInfo<snowcrash::DataStructure>& dataStructure, ConversionContext& context)
{
    if (dataStructure.node->empty()) {
        return nullptr;
    }

    mson::BaseTypeName nameType = ResolveType(dataStructure.node->typeDefinition.typeSpecification, context);

    switch (nameType) {
        case mson::BooleanTypeName:
            return RefractElementFromMSON<BooleanElement>(dataStructure, context);
        case mson::NumberTypeName:
            return RefractElementFromMSON<NumberElement>(dataStructure, context);
        case mson::StringTypeName:
            return RefractElementFromMSON<StringElement>(dataStructure, context);
        case mson::EnumTypeName:
            return RefractElementFromMSON<EnumElement>(dataStructure, context);
        case mson::ArrayTypeName:
            return RefractElementFromMSON<ArrayElement>(dataStructure, context);
        case mson::ObjectTypeName:
        case mson::UndefinedTypeName:
            return RefractElementFromMSON<ObjectElement>(dataStructure, context);
        default:
            throw snowcrash::Error("unknown type of data structure", snowcrash::ApplicationError);
    }
}

std::unique_ptr<IElement> drafter::ExpandRefract(std::unique_ptr<IElement> element, ConversionContext& context)
{
    if (!element) {
        return nullptr;
    }

    ExpandVisitor expander(context.typeRegistry());
    Visit(expander, *element);

    if (auto expanded = expander.get()) {
        return expanded;
    }

    return element;
}

// OPTIM @tjanc@ move implementation to MsonMemberToApie.cc
std::unique_ptr<refract::IElement> drafter::MsonMemberToApie( //
    const NodeInfo<mson::PropertyMember>& nodeInfo,
    ConversionContext& context,
    mson::BaseTypeName nameType,
    mson::BaseTypeName defaultNestedType)
{
    return MsonMemberToRefract<PropertyTrait>(nodeInfo, context, nameType, defaultNestedType);
}

// OPTIM @tjanc@ move implementation to MsonMemberToApie.cc
std::unique_ptr<refract::IElement> drafter::MsonMemberToApie( //
    const NodeInfo<mson::ValueMember>& nodeInfo,
    ConversionContext& context,
    mson::BaseTypeName nameType,
    mson::BaseTypeName defaultNestedType)
{
    return MsonMemberToRefract<ValueTrait>(nodeInfo, context, nameType, defaultNestedType);
}
