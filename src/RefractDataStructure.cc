//
//  RefractDataStructure.cc
//  drafter
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "SourceAnnotation.h"
#include "RefractDataStructure.h"

#include "RefractSourceMap.h"
#include "refract/VisitorUtils.h"
#include "refract/ExpandVisitor.h"
#include "refract/SerializeVisitor.h"

#include "NamedTypesRegistry.h"
#include "RefractElementFactory.h"
#include "ConversionContext.h"

#include "ElementData.h"

#include <assert.h>

namespace drafter
{

    const auto PrimitiveType = std::true_type::value;
    const auto ComplexType = std::false_type::value;

    template <typename U>
    struct FetchSourceMap {

        snowcrash::SourceMap<U> operator()(const NodeInfo<mson::ValueMember>& valueMember)
        {
            snowcrash::SourceMap<U> sourceMap = *NodeInfo<U>::NullSourceMap();
            sourceMap.sourceMap = valueMember.sourceMap->valueDefinition.sourceMap;
            return sourceMap;
        }

        snowcrash::SourceMap<U> operator()(const NodeInfo<mson::TypeSection>& typeSection)
        {
            snowcrash::SourceMap<U> sourceMap = *NodeInfo<U>::NullSourceMap();
            sourceMap.sourceMap = typeSection.sourceMap->value.sourceMap;
            return sourceMap;
        }
    };

    template <typename V, bool dummy = true>
    struct CheckValueValidity {

        typedef typename ElementData<V>::ElementInfo ElementInfo;

        void operator()(const ElementInfo&, ConversionContext&)
        {
            // do nothing
        }
    };

    template <bool dummy>
    struct CheckValueValidity<refract::NumberElement, dummy> {

        using ElementInfo = typename ElementData<refract::NumberElement>::ElementInfo;
        using ValueType = typename refract::NumberElement::ValueType;

        void operator()(const ElementInfo& value, ConversionContext& context)
        {

            std::pair<bool, ValueType> result = LiteralTo<ValueType>(std::get<0>(value));

            if (!std::get<0>(result)) {
                context.warn(snowcrash::Warning(
                    "invalid value format for 'number' type. please check mson specification for valid format",
                    snowcrash::MSONError,
                    std::get<1>(value).sourceMap));
            }
        }
    };

    template <bool dummy>
    struct CheckValueValidity<refract::BooleanElement, dummy> {

        typedef typename ElementData<refract::BooleanElement>::ElementInfo ElementInfo;
        typedef typename refract::BooleanElement::ValueType ValueType;

        void operator()(const ElementInfo& value, ConversionContext& context)
        {
            std::pair<bool, ValueType> result = LiteralTo<ValueType>(std::get<0>(value));

            if (!std::get<0>(result)) {
                context.warn(
                    snowcrash::Warning("invalid value for 'boolean' type. allowed values are 'true' or 'false'",
                        snowcrash::MSONError,
                        std::get<1>(value).sourceMap));
            }
        }
    };

    using refract::RefractElements;

    static void SetElementType(refract::IElement* element, const mson::TypeDefinition& td)
    {
        if (!td.typeSpecification.name.symbol.literal.empty()) {
            element->element(td.typeSpecification.name.symbol.literal);
        }
    }

    static mson::BaseTypeName NamedTypeFromElement(const refract::IElement* element)
    {
        refract::TypeQueryVisitor type;
        refract::Visit(type, *element);

        switch (type.get()) {
            case refract::TypeQueryVisitor::Boolean:
                return mson::BooleanTypeName;

            case refract::TypeQueryVisitor::Number:
                return mson::NumberTypeName;

            case refract::TypeQueryVisitor::String:
                return mson::StringTypeName;

            case refract::TypeQueryVisitor::Array:
                return mson::ArrayTypeName;

            case refract::TypeQueryVisitor::Enum:
                return mson::EnumTypeName;

            case refract::TypeQueryVisitor::Object:
                return mson::ObjectTypeName;

            default:
                return mson::UndefinedTypeName;
        }

        return mson::UndefinedTypeName;
    }

    template <typename T>
    static mson::BaseTypeName GetType(const T& type, ConversionContext& context)
    {
        mson::BaseTypeName nameType = type.typeDefinition.typeSpecification.name.base;
        const std::string& parent = type.typeDefinition.typeSpecification.name.symbol.literal;

        if (nameType == mson::UndefinedTypeName && !parent.empty()) {
            refract::IElement* base = FindRootAncestor(parent, context.GetNamedTypesRegistry());
            if (base) {
                nameType = NamedTypeFromElement(base);
            }
        }

        return nameType;
    }

    static refract::ArrayElement* MsonTypeAttributesToRefract(const mson::TypeAttributes& ta)
    {
        if (ta == 0) {
            return NULL;
        }

        refract::ArrayElement* attr = new refract::ArrayElement;

        if (ta & mson::RequiredTypeAttribute) {
            attr->push_back(refract::IElement::Create(SerializeKey::Required));
        }
        if (ta & mson::OptionalTypeAttribute) {
            attr->push_back(refract::IElement::Create(SerializeKey::Optional));
        }
        if (ta & mson::FixedTypeAttribute) {
            attr->push_back(refract::IElement::Create(SerializeKey::Fixed));
        }
        if (ta & mson::FixedTypeTypeAttribute) {
            attr->push_back(refract::IElement::Create(SerializeKey::FixedType));
        }
        if (ta & mson::NullableTypeAttribute) {
            attr->push_back(refract::IElement::Create(SerializeKey::Nullable));
        }

        if (attr->value.empty()) {
            delete attr;
            attr = NULL;
        }

        return attr;
    }

    static refract::IElement* MsonElementToRefract(const NodeInfo<mson::Element>& mse,
        ConversionContext& context,
        mson::BaseTypeName defaultNestedType = mson::StringTypeName);

    RefractElements MsonElementsToRefract(const NodeInfo<mson::Elements>& elements,
        ConversionContext& context,
        mson::BaseTypeName defaultNestedType = mson::StringTypeName)
    {
        RefractElements result;
        NodeInfoCollection<mson::Elements> elementsNodeInfo(elements);

        std::transform(elementsNodeInfo.begin(),
            elementsNodeInfo.end(),
            std::back_inserter(result),
            std::bind(MsonElementToRefract, std::placeholders::_1, std::ref(context), defaultNestedType));

        return result;
    }

    static mson::BaseTypeName SelectNestedTypeSpecification(
        const mson::TypeNames& nestedTypes, const mson::BaseTypeName defaultNestedType = mson::StringTypeName)
    {
        mson::BaseTypeName type = defaultNestedType;
        // Found if type of element is specified.
        // if more types is used - fallback to "StringType"
        if (nestedTypes.size() == 1) {
            type = nestedTypes.begin()->base;
        }
        return type;
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
        using ElementInfo = typename ElementData<T>::ElementInfo;

        ElementData<T>& data;
        ConversionContext& context;

        mson::BaseTypeName elementTypeName;
        mson::BaseTypeName defaultNestedType;

        template <typename U, bool IsPrimitive = IsPrimitive<U>::type::value>
        struct Fetch;

        template <typename U>
        struct Fetch<U, PrimitiveType> {
            ElementInfo operator()(const NodeInfo<mson::TypeSection>& typeSection,
                ConversionContext& context,
                const mson::BaseTypeName& defaultNestedType)
            {

                snowcrash::SourceMap<ValueType> sourceMap = FetchSourceMap<ValueType>()(typeSection);
                return std::make_tuple(typeSection.node->content.value, sourceMap);
            }
        };

        template <typename U>
        struct Fetch<U, ComplexType> {
            ElementInfo operator()(const NodeInfo<mson::TypeSection>& typeSection,
                ConversionContext& context,
                const mson::BaseTypeName& defaultNestedType)
            {
                return std::make_tuple(MsonElementsToRefract(MakeNodeInfo(typeSection.node->content.elements(),
                                                                 typeSection.sourceMap->elements()),
                                           context,
                                           defaultNestedType),
                    FetchSourceMap<ValueType>()(typeSection));
            }
        };

        template <typename U, bool IsPrimitive = IsPrimitive<U>::type::value, bool dummy = true>
        struct Store;

        template <typename U, bool dummy>
        struct Store<U, PrimitiveType, dummy> {
            void operator()(ElementData<U>& data,
                const NodeInfo<mson::TypeSection>& typeSection,
                ConversionContext& context,
                const mson::BaseTypeName& defaultNestedType)
            {
                // do nothing

                // Primitives should not contain members
                // this is to avoid push "empty" elements to primitives
                // it is related to test/fixtures/mson/primitive-with-members.apib
            }
        };

        template <typename U, bool dummy>
        struct Store<U, ComplexType, dummy> {
            void operator()(ElementData<U>& data,
                const NodeInfo<mson::TypeSection>& typeSection,
                ConversionContext& context,
                const mson::BaseTypeName& defaultNestedType)
            {

                data.values.push_back(Fetch<U>()(typeSection, context, defaultNestedType));
            }
        };

        template <bool dummy>
        struct Store<refract::EnumElement, IsPrimitive<refract::EnumElement>::type::value, dummy> {
            void operator()(ElementData<refract::EnumElement>& data,
                const NodeInfo<mson::TypeSection>& typeSection,
                ConversionContext& context,
                const mson::BaseTypeName& defaultNestedType)
            {

                data.enumerations.push_back(Fetch<refract::EnumElement>()(typeSection, context, defaultNestedType));
            }
        };

        template <typename U, bool dummy = true>
        struct TypeDefinition;

        template <bool dummy>
        struct TypeDefinition<snowcrash::DataStructure, dummy> {
            const mson::TypeDefinition& operator()(const snowcrash::DataStructure& dataStructure)
            {
                return dataStructure.typeDefinition;
            }
        };

        template <bool dummy>
        struct TypeDefinition<mson::ValueMember, dummy> {
            const mson::TypeDefinition& operator()(const mson::ValueMember& valueMember)
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

        void operator()(const NodeInfo<mson::TypeSection>& typeSection)
        {
            Fetch<ElementType> fetch;

            switch (typeSection.node->klass) {
                case mson::TypeSection::MemberTypeClass:

                    Store<ElementType>()(data, typeSection, context, defaultNestedType);
                    break;

                case mson::TypeSection::SampleClass:
                    data.samples.push_back(fetch(typeSection, context, defaultNestedType));
                    break;

                case mson::TypeSection::DefaultClass:
                    data.defaults.push_back(fetch(typeSection, context, defaultNestedType));
                    break;

                case mson::TypeSection::BlockDescriptionClass:
                    data.descriptions.push_back(
                        std::make_tuple(typeSection.node->content.description, typeSection.sourceMap->description));
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

    static mson::BaseTypeName RefractElementTypeToMsonType(refract::TypeQueryVisitor::ElementType type)
    {
        switch (type) {
            case refract::TypeQueryVisitor::String:
                return mson::StringTypeName;

            case refract::TypeQueryVisitor::Number:
                return mson::NumberTypeName;

            case refract::TypeQueryVisitor::Boolean:
                return mson::BooleanTypeName;

            case refract::TypeQueryVisitor::Array:
                return mson::ArrayTypeName;

            case refract::TypeQueryVisitor::Object:
                return mson::ObjectTypeName;

            case refract::TypeQueryVisitor::Enum:
                return mson::EnumTypeName;

            case refract::TypeQueryVisitor::Null:
            case refract::TypeQueryVisitor::Holder:
            case refract::TypeQueryVisitor::Member:
            case refract::TypeQueryVisitor::Ref:
            case refract::TypeQueryVisitor::Extend:
            case refract::TypeQueryVisitor::Option:
            case refract::TypeQueryVisitor::Select:;
        };
        return mson::UndefinedTypeName;
    }

    static mson::BaseTypeName GetMsonTypeFromName(const std::string& name, const ConversionContext& context)
    {
        refract::IElement* e = FindRootAncestor(name, context.GetNamedTypesRegistry());
        if (!e) {
            return mson::UndefinedTypeName;
        }

        refract::TypeQueryVisitor query;
        refract::VisitBy(*e, query);
        return RefractElementTypeToMsonType(query.get());
    }

    /**
     * Extract mson definitions like
     * - value enum[td1, td2]
     */

    template <typename T>
    struct ExtractTypeDefinition {

        using ElementType = T;
        using ElementInfo = typename ElementData<T>::ElementInfo;

        ElementData<ElementType>& data;
        const ConversionContext& context;

        template <typename E, bool IsPrimitive = IsPrimitive<E>::type::value>
        struct Fetch;

        template <typename E>
        struct Fetch<E, ComplexType> {
            ElementInfo operator()(const mson::TypeNames& typeNames, const ConversionContext& context)
            {
                RefractElements types;

                for (mson::TypeNames::const_iterator it = typeNames.begin(); it != typeNames.end(); ++it) {
                    mson::BaseTypeName typeName = it->base;
                    FactoryCreateMethod method = eValue;

                    if (typeName == mson::UndefinedTypeName && !it->symbol.literal.empty()) {
                        typeName = GetMsonTypeFromName(it->symbol.literal, context);
                        method = it->symbol.variable ? eSample : eElement;
                    }

                    const RefractElementFactory& f = FactoryFromType(typeName);
                    types.push_back(f.Create(it->symbol.literal, method));
                }

                return std::make_tuple(types, *NodeInfo<typename T::ValueType>::NullSourceMap());
            }
        };

        template <typename E, bool IsPrimitive = IsPrimitive<E>::type::value, bool dummy = true>
        struct Store;

        template <typename E, bool dummy>
        struct Store<E, PrimitiveType, dummy> {
            void operator()(ElementData<E>& data, const mson::TypeNames& typeNames, const ConversionContext& context)
            {
                // do nothing - Primitive types does not contain TypeDefinitions
            }
        };

        template <typename E, bool dummy>
        struct Store<E, ComplexType, dummy> {
            void operator()(ElementData<E>& data, const mson::TypeNames& typeNames, const ConversionContext& context)
            {
                data.values.push_back(Fetch<E>()(typeNames, context));
            }
        };

        template <bool dummy>
        struct Store<refract::EnumElement, IsPrimitive<refract::EnumElement>::type::value, dummy> {
            using E = refract::EnumElement;
            void operator()(ElementData<E>& data, const mson::TypeNames& typeNames, const ConversionContext& context)
            {
                data.enumerations.push_back(Fetch<E>()(typeNames, context));
            }
        };

        ExtractTypeDefinition(ElementData<ElementType>& data, const ConversionContext& context)
            : data(data), context(context)
        {
        }

        void operator()(const NodeInfo<mson::TypeDefinition>& typeDefinition)
        {
            Store<ElementType>()(data, typeDefinition.node->typeSpecification.nestedTypes, context);
        }
    };

    /**
     * Extract mson definitions like
     * - value: vm1, vm2
     */
    template <typename T, typename V = typename T::ValueType>
    struct ExtractValueMember {
        using ElementType = T;
        using ElementInfo = typename ElementData<T>::ElementInfo;

        ElementData<T>& data;
        ConversionContext& context;

        template <typename U, bool IsPrimitive = IsPrimitive<U>::type::value, bool dummy = true>
        struct Fetch;

        template <typename U, bool dummy>
        struct Fetch<U, PrimitiveType, dummy> { // primitive values

            ElementInfo operator()(const NodeInfo<mson::ValueMember>& valueMember, ConversionContext& context)
            {
                if (valueMember.node->valueDefinition.values.size() > 1) {
                    throw snowcrash::Error("only one value is supported for primitive types",
                        snowcrash::MSONError,
                        valueMember.sourceMap->sourceMap);
                }

                const mson::Value& value = *valueMember.node->valueDefinition.values.begin();
                snowcrash::SourceMap<V> sourceMap = FetchSourceMap<V>()(valueMember);

                return std::make_tuple(value.literal, sourceMap);
            }
        };

        template <typename U, bool dummy>
        struct Fetch<U, ComplexType, dummy> { // Array|Object

            ElementInfo operator()(const NodeInfo<mson::ValueMember>& valueMember, ConversionContext& context)
            {

                const mson::BaseTypeName type = SelectNestedTypeSpecification(
                    valueMember.node->valueDefinition.typeDefinition.typeSpecification.nestedTypes);

                const RefractElementFactory& factory = FactoryFromType(type);
                const mson::Values& values = valueMember.node->valueDefinition.values;

                RefractElements elements;
                std::transform(
                    values.begin(), values.end(), std::back_inserter(elements), [&factory](const mson::Value& value) {
                        return factory.Create(value.literal, value.variable ? eSample : eValue);
                    });

                return std::make_tuple(elements, FetchSourceMap<V>()(valueMember));
            }
        };

        template <bool dummy>
        struct Fetch<refract::EnumElement, IsPrimitive<refract::EnumElement>::type::value, dummy> { // Enum

            ElementInfo operator()(const NodeInfo<mson::ValueMember>& valueMember, ConversionContext& context)
            {

                const mson::BaseTypeName type = SelectNestedTypeSpecification(
                    valueMember.node->valueDefinition.typeDefinition.typeSpecification.nestedTypes);

                const RefractElementFactory& f = FactoryFromType(type);
                const mson::Values& values = valueMember.node->valueDefinition.values;

                RefractElements elements;

                for (mson::Values::const_iterator it = values.begin(); it != values.end(); ++it) {
                    elements.push_back(f.Create(it->literal, eValue));
                }

                return std::make_tuple(elements, FetchSourceMap<V>()(valueMember));
            }
        };

        template <typename Y, bool dummy = true>
        struct IsValueVariable {

            bool operator()(const mson::Value& value)
            {
                return value.variable;
            }
        };

        template <bool dummy>
        struct IsValueVariable<RefractElements, dummy> {

            bool operator()(const mson::Value&)
            {
                return false;
            }
        };

        template <typename E, bool dummy = true>
        struct Store {
            void operator()(ElementData<E>& data, const ElementInfo& info)
            {
                data.values.push_back(info);
            }
        };

        template <bool dummy>
        struct Store<refract::EnumElement, dummy> {
            void operator()(ElementData<refract::EnumElement>& data, const ElementInfo& info)
            {
                if (std::get<0>(info).size() == 1) {
                    data.values.push_back(info);
                } else {
                    data.enumerations.push_back(info);
                }
            }
        };

        ExtractValueMember(ElementData<T>& data, ConversionContext& context, const mson::BaseTypeName)
            : data(data), context(context)
        {
        }

        void operator()(const NodeInfo<mson::ValueMember>& valueMember)
        {
            // silently ignore "value" for ObjectElement e.g.
            // # A (array)
            // - key (object)
            // warning is attached while creating ValueMember in snowcrash
            if (valueMember.node->valueDefinition.typeDefinition.baseType == mson::ImplicitObjectBaseType
                || valueMember.node->valueDefinition.typeDefinition.baseType == mson::ObjectBaseType) {
                return;
            }

            Fetch<ElementType> fetch;
            mson::TypeAttributes attrs = valueMember.node->valueDefinition.typeDefinition.attributes;

            if (!valueMember.node->valueDefinition.values.empty()) {
                const mson::Value& value = *valueMember.node->valueDefinition.values.begin();

                ElementInfo parsed = fetch(valueMember, context);

                if (attrs & mson::DefaultTypeAttribute) {
                    data.defaults.push_back(parsed);
                } else if ((attrs & mson::SampleTypeAttribute) || IsValueVariable<typename T::ValueType>()(value)) {
                    data.samples.push_back(parsed);
                } else {
                    Store<ElementType>()(data, parsed);
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
                    std::make_tuple(valueMember.node->description, valueMember.sourceMap->description));
            }

            if ((valueMember.node->valueDefinition.values.empty()
                    || (valueMember.node->valueDefinition.typeDefinition.typeSpecification.nestedTypes.size() > 1))) {

                ExtractTypeDefinition<T> extd(data, context);
                extd(MakeNodeInfoWithoutSourceMap(valueMember.node->valueDefinition.typeDefinition));
            }
        }
    };

    namespace
    {
        struct Join {
            std::string& base;
            Join(std::string& str) : base(str) {}

            void operator()(const std::string& append, const std::string separator = "\n")
            {
                if (append.empty()) {
                    return;
                }

                if (!base.empty()) {
                    base.append(separator);
                }

                base.append(append);
            }
        };

        template <typename T, bool IsPrimitive = IsPrimitive<T>::type::value>
        struct Merge;

        template <typename T>
        struct Merge<T, PrimitiveType> {
            using ElementInfo = typename ElementData<T>::ElementInfo;
            using Container = typename ElementData<T>::ElementInfoContainer;
            using StoredType = typename ElementData<T>::StoredType;
            using SourceMap = typename ElementData<T>::ValueSourceMapType;

            ElementInfo operator()(const Container& container) const
            {

                return container.empty() ? ElementInfo() : container.front();
            }
        };

        template <typename T>
        struct Merge<T, ComplexType> {
            using ElementInfo = typename ElementData<T>::ElementInfo;
            using Container = typename ElementData<T>::ElementInfoContainer;
            using StoredType = typename ElementData<T>::StoredType;
            using SourceMap = typename ElementData<T>::ValueSourceMapType;

            ElementInfo operator()(const Container& container) const
            {

                StoredType value;
                SourceMap sourceMap;

                for (const auto& info : container) {
                    auto iValue = std::get<0>(info);

                    std::copy(iValue.begin(), iValue.end(), std::back_inserter(value));

                    // FIXME: merge source map?
                    // it is not used later,
                    // every element in vector has it own map
                }

                return std::make_tuple(value, sourceMap);
            }
        };

        template <typename E, bool IsPrimitive = IsPrimitive<E>::type::value>
        struct ElementInfoToElement;

        template <typename E>
        struct ElementInfoToElement<E, PrimitiveType> {
            E* operator()(const typename ElementData<E>::ElementInfo& value) const
            {
                std::pair<bool, typename E::ValueType> result = LiteralTo<typename E::ValueType>(std::get<0>(value));
                return result.first ? new E(std::get<1>(result)) : new E;
            }
        };

        template <typename E>
        struct ElementInfoToElement<E, ComplexType> {
            E* operator()(const typename ElementData<E>::ElementInfo& value) const
            {
                return new E(std::get<0>(value));
            }
        };

        template <>
        struct ElementInfoToElement<refract::EnumElement, IsPrimitive<refract::EnumElement>::type::value> {
            refract::EnumElement* operator()(const typename ElementData<refract::EnumElement>::ElementInfo& value) const
            {

                auto v = std::get<0>(value);

                refract::EnumElement* e = new refract::EnumElement;

                if (v.size() == 1) {
                    e->set(v.front());
                } else if (v.size() > 1) {
                    e->attributes[SerializeKey::Enumerations] = new refract::ArrayElement(v);
                }

                return e;
            }
        };

        template <typename T, bool IsPrimitive = IsPrimitive<T>::type::value>
        struct SaveValue;

        template <typename T>
        struct SaveValue<T, PrimitiveType> {
            using ElementInfo = typename ElementData<T>::ElementInfo;
            using ValueType = typename T::ValueType;

            void operator()(const ElementData<T>& data, T* element) const
            {
                if (data.values.empty()) {
                    return;
                }

                ElementInfo value = Merge<T>()(data.values);
                std::pair<bool, ValueType> result = LiteralTo<ValueType>(std::get<0>(value));

                element->set(std::get<1>(result));

                // FIXME: refactoring adept - AttachSourceMap require NodeInfo, let it pass for now
                AttachSourceMap(element, MakeNodeInfo(std::get<1>(result), std::get<1>(value)));
            }
        };

        template <typename T>
        struct SaveValue<T, ComplexType> {
            using ElementInfo = typename ElementData<T>::ElementInfo;

            void operator()(const ElementData<T>& data, T* element) const
            {
                ElementInfo value = Merge<T>()(data.values);

                if (!std::get<0>(value).empty()) {
                    element->set(std::get<0>(value));
                }
            }
        };

        template <>
        struct SaveValue<refract::EnumElement, IsPrimitive<refract::EnumElement>::type::value> {
            using ElementInfo = typename ElementData<refract::EnumElement>::ElementInfo;

            void operator()(const ElementData<refract::EnumElement>& data, refract::EnumElement* element) const
            {
                ElementInfo values = Merge<refract::EnumElement>()(data.values);
                ElementInfo enumerations = Merge<refract::EnumElement>()(data.enumerations);

                if (!std::get<0>(values).empty()) {
                    element->set(std::get<0>(values).front());
                    // FIXME: relese rest of elements
                    // it can leak if more values wil be in `values`
                }

                if (!std::get<0>(enumerations).empty()) {
                    element->attributes[SerializeKey::Enumerations]
                        = new refract::ArrayElement(std::get<0>(enumerations));
                }
            }
        };

        template <typename T, bool IsPrimitive = IsPrimitive<T>::type::value>
        struct ReleaseStoredData;

        template <typename T>
        struct ReleaseStoredData<T, PrimitiveType> {
            template <typename I1, typename I2>
            void operator()(const I1& begin, const I2& end) const
            {
                // do nothing
            }
        };

        template <typename T>
        struct ReleaseStoredData<T, ComplexType> {
            template <typename I1, typename I2>
            void operator()(const I1& begin, const I2& end) const
            {
                std::for_each(begin, end, [](const typename ElementData<T>::ElementInfo& info) {
                    const auto& elements = std::get<0>(info);
                    for_each(elements.begin(), elements.end(), [](const refract::IElement* e) { delete e; });
                });
            }
        };

        template <typename T>
        struct AllElementsToAtribute {

            template <typename U>
            void operator()(const U& values, const std::string& key, refract::IElement* element) const
            {
                if (values.empty()) {
                    return;
                }

                refract::ArrayElement* a = new refract::ArrayElement;

                ElementInfoToElement<T> fetch;

                for (const auto& value : values) {
                    a->push_back(fetch(value));
                }

                element->attributes[key] = a;
            }
        };

        template <>
        struct AllElementsToAtribute<refract::EnumElement> {
            using T = refract::EnumElement;

            template <typename U>
            void operator()(const U& values, const std::string& key, refract::IElement* element) const
            {

                auto merged = Merge<T>()(values);
                const auto& items = std::get<0>(merged);

                if (items.empty()) {
                    return;
                }

                refract::ArrayElement* a = new refract::ArrayElement;
                for (const auto& item : items) {
                    refract::EnumElement* e = new refract::EnumElement;
                    e->set(item);
                    a->push_back(e);
                }

                element->attributes[key] = a;
            }
        };

        template <typename T>
        struct LastElementToAttribute {

            template <typename U>
            void operator()(const U& values,
                const std::string& key,
                refract::IElement* element,
                ConversionContext& /* context */) const
            {

                if (values.empty()) {
                    return;
                }

                auto rbegin = values.rbegin();

                // pick last one
                ElementInfoToElement<T> fetch;
                refract::IElement* value = fetch(*rbegin);

                // and release rest of them
                rbegin++;
                ReleaseStoredData<T>()(rbegin, values.rend());

                element->attributes[key] = value;
            }
        };

        template <typename T>
        void CheckForMultipleDefaultDefinitions(const T& values, ConversionContext& context)
        {
            if (values.empty()) {
                return;
            }

            const auto& first = std::get<0>(*values.begin());
            // we are sure `first` is valid because precondition check `values.empty()`
            if ((values.size() > 1) || (first.size() > 1)) {

                mdp::CharactersRangeSet location;
                for (const auto& item : values) {
                    const auto& sourceMap = std::get<1>(item);
                    location.append(sourceMap.sourceMap);
                }

                context.warn(
                    snowcrash::Warning("multiple definitions of 'default' value", snowcrash::MSONError, location));
            }
        }

        template <>
        struct LastElementToAttribute<refract::EnumElement> {
            using T = refract::EnumElement;

            template <typename U>
            void operator()(
                const U& values, const std::string& key, refract::IElement* element, ConversionContext& context) const
            {

                CheckForMultipleDefaultDefinitions<U>(values, context);

                auto merged = Merge<T>()(values);
                const auto& items = std::get<0>(merged);

                if (items.empty()) {

                    // empty value for default enum
                    // see test/fixtures/mson/enum-empty-default.apib
                    if (!values.empty()) {
                        element->attributes[key] = new refract::EnumElement;
                    }
                    return;
                }

                // pick last one value and use as default value
                auto rbegin = items.rbegin();

                assert(*rbegin);
                refract::EnumElement* e = new refract::EnumElement;
                e->set(*rbegin);
                element->attributes[key] = e;

                // move to previous
                // we can do it, because we know there is at least one element
                rbegin++;

                // release rest of them
                for_each(rbegin, items.rend(), [](const refract::IElement* e) { delete e; });
            }
        };

        template <typename T>
        void ElementDataToElement(T* element, const ElementData<T>& data, ConversionContext& context)
        {

            for (auto collection : { data.values, data.samples, data.defaults, data.enumerations }) {
                std::for_each(collection.begin(),
                    collection.end(),
                    std::bind(CheckValueValidity<T>(), std::placeholders::_1, std::ref(context)));
            }

            SaveValue<T>()(data, element);
            AllElementsToAtribute<T>()(data.samples, SerializeKey::Samples, element);
            LastElementToAttribute<T>()(data.defaults, SerializeKey::Default, element, context);
        }
    }

    template <typename T>
    refract::IElement* DescriptionToRefract(const T& descriptions)
    {
        if (descriptions.empty()) {
            return NULL;
        }

        std::string description;
        Join join(description);
        snowcrash::SourceMap<std::string> sourceMap;

        for (const auto& item : descriptions) {
            join(std::get<0>(item));
            sourceMap.sourceMap.append(std::get<1>(item).sourceMap);
        }

        if (description.empty()) {
            return NULL;
        }

        return PrimitiveToRefract(NodeInfo<std::string>(&description, &sourceMap));
    }

    // FIXME: refactoring - description is not used while calling from
    // `RefractElementFromProperty()`
    template <typename T>
    refract::IElement* RefractElementFromValue(const NodeInfo<mson::ValueMember>& value,
        ConversionContext& context,
        const mson::BaseTypeName defaultNestedType,
        typename ElementData<T>::DescriptionInfoContainer& descriptions)
    {
        using namespace refract;
        using ElementType = T;

        ElementData<ElementType> data;
        ElementType* element = new ElementType;

        ExtractValueMember<ElementType>(data, context, defaultNestedType)(value);

        SetElementType(element, value.node->valueDefinition.typeDefinition);
        AttachSourceMap(element, value);

        NodeInfoCollection<mson::TypeSections> typeSections(MAKE_NODE_INFO(value, sections));

        std::for_each(typeSections.begin(), typeSections.end(), ExtractTypeSection<T>(data, context, value));

        ElementDataToElement(element, data, context);

        descriptions = data.descriptions;

        return element;
    }

    static bool VariablePropertyIsString(const mson::ValueDefinition& variable, ConversionContext& context)
    {
        if (variable.typeDefinition.typeSpecification.name.base == mson::StringTypeName) {
            return true;
        }

        if (refract::TypeQueryVisitor::as<refract::StringElement>(FindRootAncestor(
                variable.typeDefinition.typeSpecification.name.symbol.literal, context.GetNamedTypesRegistry()))) {
            return true;
        }

        return false;
    }

    refract::IElement* GetPropertyKey(const NodeInfo<mson::PropertyMember>& property, ConversionContext& context)
    {

        refract::StringElement* key = new refract::StringElement;
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
                    delete key;
                    throw snowcrash::Error("'variable named property' must be string or its sub-type",
                        snowcrash::MSONError,
                        sourceMap.sourceMap);
                }

                SetElementType(key, property.node->name.variable.typeDefinition);
            }

            key->attributes[SerializeKey::Variable] = refract::IElement::Create(true);

            if (!property.node->name.variable.values.empty()) {
                key->set(property.node->name.variable.values.begin()->literal);
            }
        }

        if (!property.node->name.literal.empty()) {
            key->set(property.node->name.literal);
        }

        AttachSourceMap(key, MakeNodeInfo(property.node->name.literal, sourceMap));

        return key;
    }

    template <typename T>
    refract::MemberElement* RefractElementFromProperty(const NodeInfo<mson::PropertyMember>& property,
        ConversionContext& context,
        const mson::BaseTypeName defaultNestedType)
    {

        typename ElementData<T>::DescriptionInfoContainer dummy; // we need no this
        typename ElementData<T>::DescriptionInfoContainer descriptions;

        refract::IElement* key = GetPropertyKey(property, context);
        refract::IElement* value = RefractElementFromValue<T>(
            NodeInfo<mson::ValueMember>(property.node, property.sourceMap), context, defaultNestedType, dummy);
        refract::MemberElement* element = new refract::MemberElement(key, value);

        mson::TypeAttributes attrs = property.node->valueDefinition.typeDefinition.attributes;

        // there is no source map for attributes
        if (refract::IElement* attributes = MsonTypeAttributesToRefract(attrs)) {
            element->attributes[SerializeKey::TypeAttributes] = attributes;
        }

        if (!property.node->description.empty()) {
            descriptions.push_back(std::make_tuple(property.node->description, property.sourceMap->description));
        }

        NodeInfoCollection<mson::TypeSections> typeSections(MAKE_NODE_INFO(property, sections));

        for (const auto& typeSection : typeSections) {
            if (typeSection.node->klass == mson::TypeSection::BlockDescriptionClass) {
                descriptions.push_back(
                    std::make_tuple(typeSection.node->content.description, typeSection.sourceMap->description));
            }
        }

        if (!property.node->description.empty() && (descriptions.size() > 1)) {
            std::get<0>(descriptions[0]).append("\n");
        }

        if (refract::IElement* description = DescriptionToRefract(descriptions)) {
            element->meta[SerializeKey::Description] = description;
        }

        return element;
    }

    static bool ValueHasMembers(const mson::ValueMember* value)
    {
        for (mson::TypeSections::const_iterator it = value->sections.begin(); it != value->sections.end(); ++it) {
            if (it->klass == mson::TypeSection::MemberTypeClass) {
                return true;
            }
        }
        return false;
    }

    static bool ValueHasChildren(const mson::ValueMember* value)
    {
        return value->valueDefinition.values.size() > 1;
    }

    static bool ValueHasName(const mson::ValueMember* value)
    {
        return !value->valueDefinition.typeDefinition.typeSpecification.name.symbol.literal.empty();
    }

    struct PropertyTrait {
        typedef refract::MemberElement ElementType;
        typedef NodeInfo<mson::PropertyMember> InputType;

        template <typename T>
        static ElementType* Invoke(
            const InputType& prop, ConversionContext& context, const mson::BaseTypeName defaultNestedType)
        {
            return RefractElementFromProperty<T>(prop, context, defaultNestedType);
        }
    };

    struct ValueTrait {
        typedef refract::IElement ElementType;
        typedef NodeInfo<mson::ValueMember> InputType;

        template <typename T>
        static ElementType* Invoke(
            const InputType& val, ConversionContext& context, const mson::BaseTypeName defaultNestedType)
        {
            typename ElementData<T>::DescriptionInfoContainer descriptions;

            refract::IElement* element = RefractElementFromValue<T>(val, context, defaultNestedType, descriptions);

            mson::TypeAttributes attrs = val.node->valueDefinition.typeDefinition.attributes;

            // there is no source map for attributes
            if (refract::IElement* attributes = MsonTypeAttributesToRefract(attrs)) {
                element->attributes[SerializeKey::TypeAttributes] = attributes;
            }

            if (refract::IElement* description = DescriptionToRefract(descriptions)) {
                element->meta[SerializeKey::Description] = description;
            }

            return element;
        }
    };

    static void CheckTypeAttributesClash(const mson::TypeAttributes& attributes,
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
    static refract::IElement* MsonMemberToRefract(const typename Trait::InputType& input,
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
                return Trait::template Invoke<refract::BooleanElement>(input, context, defaultNestedType);

            case mson::NumberTypeName:
                return Trait::template Invoke<refract::NumberElement>(input, context, defaultNestedType);

            case mson::StringTypeName:
                return Trait::template Invoke<refract::StringElement>(input, context, defaultNestedType);

            case mson::EnumTypeName:
                return Trait::template Invoke<refract::EnumElement>(input, context, defaultNestedType);

            case mson::ArrayTypeName:
                return Trait::template Invoke<refract::ArrayElement>(input, context, defaultNestedType);

            case mson::ObjectTypeName:
                return Trait::template Invoke<refract::ObjectElement>(input, context, defaultNestedType);

            case mson::UndefinedTypeName: {
                if (ValueHasChildren(input.node)) {
                    // FIXME: what about EnumElement
                    return Trait::template Invoke<refract::ArrayElement>(input, context, defaultNestedType);
                } else if (ValueHasName(input.node) || ValueHasMembers(input.node)) {
                    return Trait::template Invoke<refract::ObjectElement>(input, context, defaultNestedType);
                } else if (nameType != defaultNestedType) {
                    return MsonMemberToRefract<Trait>(input, context, defaultNestedType, defaultNestedType, false);
                }

                return MsonMemberToRefract<Trait>(input, context, mson::StringTypeName, defaultNestedType, false);
            }
        }

        throw snowcrash::Error("unknown type of mson member", snowcrash::MSONError, input.sourceMap->sourceMap);
    }

    static refract::IElement* MsonOneofToRefract(const NodeInfo<mson::OneOf>& oneOf, ConversionContext& context)
    {
        refract::SelectElement* select = new refract::SelectElement;

        NodeInfoCollection<mson::OneOf> oneOfNodeInfo(oneOf);

        for (NodeInfoCollection<mson::OneOf>::const_iterator it = oneOfNodeInfo.begin(); it != oneOfNodeInfo.end();
             ++it) {

            refract::OptionElement* option = new refract::OptionElement;

            // we can not use MsonElementToRefract() for groups,
            // "option" element handles directly all elements in group
            if (it->node->klass == mson::Element::GroupClass) {
                option->set(MsonElementsToRefract(
                    MakeNodeInfo(it->node->content.elements(), it->sourceMap->elements()), context));
            } else {
                option->push_back(MsonElementToRefract(*it, context, mson::StringTypeName));
            }

            select->push_back(option);
        }

        return select;
    }

    static refract::IElement* MsonMixinToRefract(const NodeInfo<mson::Mixin>& mixin)
    {
        refract::RefElement* ref = new refract::RefElement;

        ref->set(mixin.node->typeSpecification.name.symbol.literal);
        ref->attributes[SerializeKey::Path] = refract::IElement::Create(SerializeKey::Content);

        return ref;
    }

    static refract::IElement* MsonElementToRefract(const NodeInfo<mson::Element>& mse,
        ConversionContext& context,
        const mson::BaseTypeName defaultNestedType /* = mson::StringTypeName */)
    {
        switch (mse.node->klass) {
            case mson::Element::PropertyClass:
                return MsonMemberToRefract<PropertyTrait>(
                    MakeNodeInfo(mse.node->content.property, mse.sourceMap->property),
                    context,
                    GetType(mse.node->content.property.valueDefinition, context),
                    defaultNestedType);

            case mson::Element::ValueClass:
                return MsonMemberToRefract<ValueTrait>(MakeNodeInfo(mse.node->content.value, mse.sourceMap->value),
                    context,
                    GetType(mse.node->content.value.valueDefinition, context),
                    defaultNestedType);

            case mson::Element::MixinClass:
                return MsonMixinToRefract(MakeNodeInfo(mse.node->content.mixin, mse.sourceMap->mixin));

            case mson::Element::OneOfClass:
                return MsonOneofToRefract(MakeNodeInfo(mse.node->content.oneOf(), mse.sourceMap->oneOf()), context);

            case mson::Element::GroupClass:
                throw snowcrash::Error("unable to handle element group", snowcrash::ApplicationError);

            default:
                throw snowcrash::Error("unknown type of mson element", snowcrash::ApplicationError);
        }
    }

    template <typename T>
    refract::IElement* RefractElementFromMSON(const NodeInfo<snowcrash::DataStructure>& ds, ConversionContext& context)
    {
        using namespace refract;
        typedef T ElementType;

        ElementType* element = new ElementType;
        SetElementType(element, ds.node->typeDefinition);

        if (!ds.node->name.symbol.literal.empty()) {
            snowcrash::SourceMap<mson::Literal> sourceMap = *NodeInfo<mson::Literal>::NullSourceMap();
            sourceMap.sourceMap.append(ds.sourceMap->name.sourceMap);
            element->meta[SerializeKey::Id] = PrimitiveToRefract(MakeNodeInfo(ds.node->name.symbol.literal, sourceMap));
        }

        AttachSourceMap(element, MakeNodeInfo(ds.node, ds.sourceMap));

        // there is no source map for attributes
        if (refract::IElement* attributes = MsonTypeAttributesToRefract(ds.node->typeDefinition.attributes)) {
            element->attributes[SerializeKey::TypeAttributes] = attributes;
        }

        ElementData<T> data;

        ExtractTypeDefinition<ElementType> extd(data, context);
        extd(MAKE_NODE_INFO(ds, typeDefinition));

        NodeInfoCollection<mson::TypeSections> typeSections(MAKE_NODE_INFO(ds, sections));

        std::for_each(typeSections.begin(), typeSections.end(), ExtractTypeSection<T>(data, context, ds));

        ElementDataToElement(element, data, context);

        if (refract::IElement* description = DescriptionToRefract(data.descriptions)) {
            element->meta[SerializeKey::Description] = description;
        }

        return element;
    }

    refract::IElement* MSONToRefract(
        const NodeInfo<snowcrash::DataStructure>& dataStructure, ConversionContext& context)
    {
        if (dataStructure.node->empty()) {
            return NULL;
        }

        using namespace refract;
        IElement* element = NULL;

        mson::BaseTypeName nameType = GetType(*dataStructure.node, context);

        switch (nameType) {
            case mson::BooleanTypeName:
                element = RefractElementFromMSON<refract::BooleanElement>(dataStructure, context);
                break;

            case mson::NumberTypeName:
                element = RefractElementFromMSON<refract::NumberElement>(dataStructure, context);
                break;

            case mson::StringTypeName:
                element = RefractElementFromMSON<refract::StringElement>(dataStructure, context);
                break;

            case mson::EnumTypeName:
                element = RefractElementFromMSON<refract::EnumElement>(dataStructure, context);
                break;

            case mson::ArrayTypeName:
                element = RefractElementFromMSON<refract::ArrayElement>(dataStructure, context);
                break;

            case mson::ObjectTypeName:
            case mson::UndefinedTypeName:
                element = RefractElementFromMSON<refract::ObjectElement>(dataStructure, context);
                break;

            default:
                throw snowcrash::Error("unknown type of data structure", snowcrash::ApplicationError);
        }

        return element;
    }

    refract::IElement* ExpandRefract(refract::IElement* element, ConversionContext& context)
    {
        if (!element) {
            return element;
        }

        refract::ExpandVisitor expander(context.GetNamedTypesRegistry());
        refract::Visit(expander, *element);

        if (refract::IElement* expanded = expander.get()) {
            delete element;
            element = expanded;
        }

        return element;
    }

    sos::Object SerializeRefract(refract::IElement* element, ConversionContext& context)
    {
        if (!element) {
            return sos::Object();
        }

        refract::SosSerializeVisitor serializer(context.options.generateSourceMap);
        refract::Visit(serializer, *element);

        return serializer.get();
    }

} // namespace drafter
