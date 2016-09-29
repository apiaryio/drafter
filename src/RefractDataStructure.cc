//
//  RefractDataStructure.cc
//  drafter
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "SourceAnnotation.h"
#include "RefractDataStructure.h"
#include "refract/AppendDecorator.h"

#include "RefractSourceMap.h"
#include "refract/VisitorUtils.h"
#include "refract/ExpandVisitor.h"
#include "refract/SerializeVisitor.h"

#include "NamedTypesRegistry.h"
#include "RefractElementFactory.h"
#include "ConversionContext.h"

namespace drafter {

    template <typename T>
    struct ElementData {
        typedef T ElementType;

        typedef typename T::ValueType ValueType;
        typedef snowcrash::SourceMap<ValueType> ValueSourceMapType;

        typedef std::tuple<ValueType, ValueSourceMapType, bool> ValueInfo; // [value, sourceMap, validity(by LiteralTo<>)]

        std::vector<ValueInfo> values;
        std::vector<ValueInfo> defaults;
        std::vector<ValueInfo> samples;

        std::vector<std::string> descriptions;
        std::vector<snowcrash::SourceMap<std::string> > descriptionsSourceMap;
    };

    template <typename U>
    struct FetchSourceMap {

        snowcrash::SourceMap<U> operator()(const NodeInfo<mson::ValueMember>& valueMember) {
            snowcrash::SourceMap<U> sourceMap = *NodeInfo<U>::NullSourceMap();
            sourceMap.sourceMap = valueMember.sourceMap->valueDefinition.sourceMap;
            return sourceMap;
        }

        snowcrash::SourceMap<U> operator()(const NodeInfo<mson::TypeSection>& typeSection) {
            snowcrash::SourceMap<U> sourceMap = *NodeInfo<U>::NullSourceMap();
            sourceMap.sourceMap = typeSection.sourceMap->value.sourceMap;
            return sourceMap;
        }
    };


    template <typename T, typename V = typename T::ValueType>
    struct Append {
        typedef T ElementType;
        typedef V ValueType;
        ElementType*& element;
        typedef typename ElementData<T>::ValueInfo ValueInfo;

        Append(ElementType*& e) : element(e)
        {
        }

        void operator()(const NodeInfo<ValueType>& value)
        {
            // FIXME: snowcrash warn about "Primitive type can not have member"
            // but in real it create "empty" member
            //
            // solution for now: set if element has no already value, otherwise silently ignore
            //
            //throw snowcrash::Error("can not append to primitive type", snowcrash::MSONError);

            if (element->empty()) {
                element->set(*value.node);
                AttachSourceMap(element, value);
            }
        }

        void operator()(const ValueInfo& value)
        {
            const NodeInfo<ValueType> nodeInfo = MakeNodeInfo(std::get<0>(value), std::get<1>(value));
            (*this)(nodeInfo);
        }
    };

    template <typename T>
    struct Append<T, std::vector<refract::IElement*> > {
        typedef T ElementType;
        typedef typename T::ValueType ValueType;
        ElementType*& element;
        typedef typename ElementData<T>::ValueInfo ValueInfo;

        Append(ElementType*& e) : element(e)
        {
        }

        void operator()(const NodeInfo<ValueType>& value)
        {
            std::for_each(value.node->begin(), value.node->end(), std::bind1st(std::mem_fun(&ElementType::push_back), element));
        }

        void operator()(const ValueInfo& value)
        {
            const NodeInfo<ValueType> nodeInfo = MakeNodeInfo(std::get<0>(value), std::get<1>(value));
            (*this)(nodeInfo);
        }
    };

    template <typename V, bool dummy = true>
    struct CheckValueValidity {

        typedef typename ElementData<V>::ValueInfo ValueInfo;

        void operator()(const ValueInfo&, ConversionContext&) {
            // do nothing
        }
    };

    template <bool dummy>
    struct CheckValueValidity<refract::NumberElement, dummy> {

        typedef typename ElementData<refract::NumberElement>::ValueInfo ValueInfo;

        void operator()(const ValueInfo& value, ConversionContext& context) {

            if (!std::get<2>(value)) {
                context.warn(snowcrash::Warning("invalid value format for 'number' type. please check mson specification for valid format", snowcrash::MSONError, std::get<1>(value).sourceMap));
            }
        }
    };

    template <bool dummy>
    struct CheckValueValidity<refract::BooleanElement, dummy> {

        typedef typename ElementData<refract::BooleanElement>::ValueInfo ValueInfo;

        void operator()(const ValueInfo& value, ConversionContext& context) {
            if (!std::get<2>(value)) {
                context.warn(snowcrash::Warning("invalid value for 'boolean' type. allowed values are 'true' or 'false'", snowcrash::MSONError, std::get<1>(value).sourceMap));
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

    static mson::BaseTypeName NamedTypeFromElement(const refract::IElement* element) {
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

    template<typename T>
    static mson::BaseTypeName GetType(const T& type, ConversionContext& context) {
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

    static refract::IElement* MsonElementToRefract(const NodeInfo<mson::Element>& mse, ConversionContext& context, mson::BaseTypeName defaultNestedType = mson::StringTypeName);

    RefractElements MsonElementsToRefract(const NodeInfo<mson::Elements>& elements, ConversionContext& context, mson::BaseTypeName defaultNestedType = mson::StringTypeName)
    {
        RefractElements result;

        // FIXME: should be used instead of "for loop" below, but there is some problem with
        // std::bind2nd && enum, will be fixed
        //
        //std::transform(elements.begin(), elements.end(),
        //               std::back_inserter(result),
        //               std::bind2nd(std::ptr_fun(MsonElementToRefract), nestedTypeName));

        NodeInfoCollection<mson::Elements> elementsNodeInfo(elements);

        for (NodeInfoCollection<mson::Elements>::const_iterator it = elementsNodeInfo.begin(); it != elementsNodeInfo.end(); ++it) {
            result.push_back(MsonElementToRefract(*it, context, defaultNestedType));
        }

        return result;
    }

    static mson::BaseTypeName SelectNestedTypeSpecification(const mson::TypeNames& nestedTypes, const mson::BaseTypeName defaultNestedType = mson::StringTypeName) {
        mson::BaseTypeName type = defaultNestedType;
        // Found if type of element is specified.
        // if more types is used - fallback to "StringType"
        if (nestedTypes.size() == 1) {
            type = nestedTypes.begin()->base;
        }
        return type;
    }


    template <typename T>
    class ExtractTypeSection
    {
        typedef typename T::ValueType ValueType;
        typedef typename ElementData<T>::ValueInfo ValueInfo;

        ElementData<T>& data;
        ConversionContext& context;

        mson::BaseTypeName elementTypeName;
        mson::BaseTypeName defaultNestedType;

        /**
         * Fetch<> is intended to extract value from TypeSection.
         * Generalized type is for primitive types
         * Specialized is for (Array|Object)Element because of underlying type.
         * `dummy` param is used because of specialization inside another struct
         */
        template <typename U, bool dummy = true>
        struct Fetch {
            ValueInfo operator()(const NodeInfo<mson::TypeSection>& typeSection, ConversionContext& context, const mson::BaseTypeName& defaultNestedType) {

                std::pair<bool, U> val = LiteralTo<U>(typeSection.node->content.value);
                snowcrash::SourceMap<U> sourceMap = FetchSourceMap<U>()(typeSection);

                ValueInfo result = std::make_tuple(val.second, sourceMap, val.first);

                CheckValueValidity<T>()(result, context);

                return result;
            }
        };

        template<bool dummy>
        struct Fetch<RefractElements, dummy> {
            ValueInfo operator()(const NodeInfo<mson::TypeSection>& typeSection, ConversionContext& context, const mson::BaseTypeName& defaultNestedType) {
                return std::make_tuple(MsonElementsToRefract(MakeNodeInfo(typeSection.node->content.elements(),
                                                            typeSection.sourceMap->elements()),
                                                            context,
                                                            defaultNestedType),
                                       FetchSourceMap<RefractElements>()(typeSection),
                                       true
                        );
            }
        };

        template <typename U, bool dummy = true>
        struct TypeDefinition;

        template<bool dummy>
        struct TypeDefinition<snowcrash::DataStructure, dummy> {
            const mson::TypeDefinition& operator()(const snowcrash::DataStructure& dataStructure) {
                return dataStructure.typeDefinition;
            }
        };

        template<bool dummy>
        struct TypeDefinition<mson::ValueMember, dummy> {
            const mson::TypeDefinition& operator()(const mson::ValueMember& valueMember) {
                return valueMember.valueDefinition.typeDefinition;
            }
        };

    public:

        template<typename U>
        ExtractTypeSection(ElementData<T>& data, ConversionContext& context, const NodeInfo<U>& sectionHolder)
          : data(data),
            context(context),
            elementTypeName(TypeDefinition<U>()(*sectionHolder.node).typeSpecification.name.base),
            defaultNestedType(SelectNestedTypeSpecification(TypeDefinition<U>()(*sectionHolder.node).typeSpecification.nestedTypes))
        {}

        void operator()(const NodeInfo<mson::TypeSection>& typeSection) {
            Fetch<ValueType> fetch;

            switch (typeSection.node->klass) {

                case mson::TypeSection::MemberTypeClass:
                    // Primitives should not contain members
                    // this is to avoid push "empty" elements to primitives
                    // it is related to test/fixtures/mson/primitive-with-members.apib

                    if (!typeSection.node->content.elements().empty()) {
                        data.values.push_back(fetch(typeSection, context, defaultNestedType));
                    }
                    break;

                case mson::TypeSection::SampleClass:
                    data.samples.push_back(fetch(typeSection, context, defaultNestedType));
                    break;

                case mson::TypeSection::DefaultClass:
                    data.defaults.push_back(fetch(typeSection, context, defaultNestedType));
                    break;

                case mson::TypeSection::BlockDescriptionClass:
                    data.descriptions.push_back(typeSection.node->content.description);
                    data.descriptionsSourceMap.push_back(typeSection.sourceMap->description);
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

    static mson::BaseTypeName RefractElementTypeToMsonType(refract::TypeQueryVisitor::ElementType type) {
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
            case refract::TypeQueryVisitor::Member:
            case refract::TypeQueryVisitor::Extend:
            case refract::TypeQueryVisitor::Option:
            case refract::TypeQueryVisitor::Select:
                ;
        };
        return mson::UndefinedTypeName;
    }

    static mson::BaseTypeName GetMsonTypeFromName(const std::string& name, ConversionContext& context) {
        refract::IElement* e = FindRootAncestor(name, context.GetNamedTypesRegistry());
        if (!e) {
            return mson::UndefinedTypeName;
        }

        refract::TypeQueryVisitor query;
        refract::VisitBy(*e, query);
        return RefractElementTypeToMsonType(query.get());
    }

    template <typename T>
    struct ExtractTypeDefinition {

        typedef T ElementType;
        typedef typename ElementData<T>::ValueInfo ValueInfo;

        ElementData<ElementType>& data;
        ConversionContext& context;

        template<typename X, bool dummy = true>
        struct Fetch {
            ValueInfo operator()(const mson::TypeNames&, ConversionContext&) {
                typename T::ValueType val;
                return std::make_tuple(val, *NodeInfo<typename T::ValueType>::NullSourceMap(), false);
            }
        };

        template<bool dummy>
        struct Fetch<RefractElements, dummy> {
            ValueInfo operator()(const mson::TypeNames& typeNames, ConversionContext& context) {
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

                return std::make_tuple(types, *NodeInfo<typename T::ValueType>::NullSourceMap(), true);
            }
        };

        ExtractTypeDefinition(ElementData<ElementType>& data, ConversionContext& context) : data(data), context(context) {}

        void operator()(const NodeInfo<mson::TypeDefinition>& typeDefinition) {
            ValueInfo value = Fetch<typename T::ValueType>()(typeDefinition.node->typeSpecification.nestedTypes, context);

            if (std::get<2>(value)) {
                data.values.push_back(value);
            }
        }
    };

    template <typename T, typename V = typename T::ValueType>
    struct ExtractValueMember
    {
        typedef T ElementType;
        typedef typename ElementData<T>::ValueInfo ValueInfo;

        ElementData<T>& data;
        ConversionContext& context;

        template <typename U, bool dummy = true>
        struct Fetch {  // primitive values

            ValueInfo operator()(const NodeInfo<mson::ValueMember>& valueMember, ConversionContext& context) {
                if (valueMember.node->valueDefinition.values.size() > 1) {
                    throw snowcrash::Error("only one value is supported for primitive types", snowcrash::MSONError, valueMember.sourceMap->sourceMap);
                }

                const mson::Value& value = *valueMember.node->valueDefinition.values.begin();

                std::pair<bool, U> val = LiteralTo<U>(value.literal);
                snowcrash::SourceMap<U> sourceMap = FetchSourceMap<U>()(valueMember);

                ValueInfo result = std::make_tuple(val.second, sourceMap, val.first);

                CheckValueValidity<T>()(result, context);

                return result;
            }
        };

        template<bool dummy>
        struct Fetch<RefractElements, dummy> { // Array|Object

            ValueInfo operator()(const NodeInfo<mson::ValueMember>& valueMember, ConversionContext& context) {

                const mson::BaseTypeName type = SelectNestedTypeSpecification(valueMember.node->valueDefinition.typeDefinition.typeSpecification.nestedTypes);

                const RefractElementFactory& f = FactoryFromType(type);
                const mson::Values& values = valueMember.node->valueDefinition.values;

                RefractElements elements;

                for (mson::Values::const_iterator it = values.begin(); it != values.end(); ++it) {
                    elements.push_back(f.Create(it->literal, it->variable ? eSample : eValue));
                }

                return std::make_tuple(elements, FetchSourceMap<RefractElements>()(valueMember), true);
            }
        };

        template<typename Y, bool dummy = true>
        struct IsValueVariable {

            bool operator()(const mson::Value& value) {
                return value.variable;
            }
        };

        template<bool dummy>
        struct IsValueVariable<RefractElements, dummy>{

            bool operator()(const mson::Value&) {
                return false;
            }
        };

        ExtractValueMember(ElementData<T>& data, ConversionContext& context, const mson::BaseTypeName) : data(data), context(context) {}

        void operator ()(const NodeInfo<mson::ValueMember>& valueMember)
        {
            // silently ignore "value" for ObjectElement e.g.
            // # A (array)
            // - key (object)
            // warning is attached while creating ValueMember in snowcrash
            if (valueMember.node->valueDefinition.typeDefinition.baseType == mson::ImplicitObjectBaseType ||
                valueMember.node->valueDefinition.typeDefinition.baseType == mson::ObjectBaseType) {
                return;
            }

            Fetch<typename T::ValueType> fetch;

            if (!valueMember.node->valueDefinition.values.empty()) {
                mson::TypeAttributes attrs = valueMember.node->valueDefinition.typeDefinition.attributes;
                const mson::Value& value = *valueMember.node->valueDefinition.values.begin();

                ValueInfo parsed = fetch(valueMember, context);

                if (attrs & mson::DefaultTypeAttribute) {
                    data.defaults.push_back(parsed);
                }
                else if ((attrs & mson::SampleTypeAttribute) || IsValueVariable<typename T::ValueType>()(value)) {
                    data.samples.push_back(parsed);
                }
                else {
                    data.values.push_back(parsed);
                }
            }

            if (!valueMember.node->description.empty()) {
                data.descriptions.push_back(valueMember.node->description);
                data.descriptionsSourceMap.push_back(valueMember.sourceMap->description);
            }

            if ((valueMember.node->valueDefinition.values.empty() ||
                (valueMember.node->valueDefinition.typeDefinition.typeSpecification.nestedTypes.size() > 1))  &&
                (GetType(valueMember.node->valueDefinition, context) != mson::EnumTypeName)) {

                ExtractTypeDefinition<T> extd(data, context);
                extd(MakeNodeInfoWithoutSourceMap(valueMember.node->valueDefinition.typeDefinition));
            }
        }
    };

    namespace
    {
        template<typename T> void Deleter(T* ptr) { delete ptr; }

        struct Join {
            std::string& base;
            Join(std::string& str) : base(str)
            {
            }

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

        refract::IElement* SetSerializeFlag(refract::IElement* element) {
            refract::TypeQueryVisitor query;
            refract::VisitBy(*element, query);

            RefractElements* children = NULL;

            if (query.get() == refract::TypeQueryVisitor::Array) {
                children = &static_cast<refract::ArrayElement*>(element)->value;
            }
            else if (query.get() == refract::TypeQueryVisitor::Enum) {
                children = &static_cast<refract::EnumElement*>(element)->value;

            }
            else if (query.get() == refract::TypeQueryVisitor::Object) {
                children = &static_cast<refract::ObjectElement*>(element)->value;

            }

            if (children) {
                refract::SetRenderFlag(*children, refract::IElement::rFull);
            }

            return element;
        }

        template <typename T>
        struct SaveSamples {

            template <typename U>
            void operator()(const U& samples, refract::IElement* element) {
                if (samples.empty()) {
                    return;
                }

                refract::ArrayElement* a = new refract::ArrayElement;

                for (auto sample : samples) {
                    T* sampleElement = new T;
                    sampleElement->set(std::get<0>(sample));
                    SetSerializeFlag(sampleElement);
                    a->push_back(sampleElement);
                }

                element->attributes[SerializeKey::Samples] = a;
            }

        };

        template <typename T>
        struct SaveDefault {

            template <typename U>
            void operator()(const U& defaults, refract::IElement* element) {
                if (defaults.empty()) {
                    return;
                }

                T* defaultElement = new T;
                defaultElement->set(std::get<0>(*defaults.rbegin()));
                SetSerializeFlag(defaultElement);
                element->attributes[SerializeKey::Default] = defaultElement;
            }

        };


        template <typename T>
        struct MakeNodeInfoFunctor {
            NodeInfo<T> operator()(std::pair<bool, const T&> v, const snowcrash::SourceMap<T>& sm) {
                return MakeNodeInfo<T>(v.second, sm);
            }
        };

        template<typename T>
        void TransformElementData(T* element, ElementData<T>& data) {
            std::for_each(data.values.begin(), data.values.end(), Append<T>(element));
            SaveSamples<T>()(data.samples, element);
            SaveDefault<T>()(data.defaults, element);
        }
    }

    template<typename T>
    refract::IElement* DescriptionToRefract(const ElementData<T>& data)
    {
        if (data.descriptions.empty()) {
            return NULL;
        }

        std::string description;
        Join join(description);

        for_each(data.descriptions.begin(), data.descriptions.end(), join);

        snowcrash::SourceMap<std::string> sourceMap;
        typedef typename std::vector<snowcrash::SourceMap<std::string> >::const_iterator Iterator;

        for (Iterator it = data.descriptionsSourceMap.begin(); it != data.descriptionsSourceMap.end(); ++it) {
            sourceMap.sourceMap.append(it->sourceMap);
        }

        return PrimitiveToRefract(NodeInfo<std::string>(&description, &sourceMap));
    }

    template <typename T>
    struct MoveFirstValueToSample {
       void operator() (const NodeInfo<mson::ValueMember>& value, ElementData<T>& data)
       {
       }
    };

    template <>
    struct MoveFirstValueToSample<refract::EnumElement> {
        typedef refract::EnumElement T;
        void operator()(const NodeInfo<mson::ValueMember>& value, ElementData<T>& data) {
            if (value.node->valueDefinition.values.empty() || data.values.empty()) {
                return;
            }

            data.samples.insert(data.samples.begin(), data.values.front());
            data.values.erase(data.values.begin());
        }
    };

    template <typename T>
    refract::IElement* RefractElementFromValue(const NodeInfo<mson::ValueMember>& value, ConversionContext& context, const mson::BaseTypeName defaultNestedType, bool generateAttributes = true)
    {
        using namespace refract;
        typedef T ElementType;

        ElementData<ElementType> data;
        ElementType* element = new ElementType;

        if (generateAttributes) {
            mson::TypeAttributes attrs = value.node->valueDefinition.typeDefinition.attributes;

            // there is no source map for attributes
            if (refract::IElement* attributes = MsonTypeAttributesToRefract(attrs)) {
                element->attributes[SerializeKey::TypeAttributes] = attributes;
            }
        }

        ExtractValueMember<ElementType>(data, context, defaultNestedType)(value);

        size_t valuesCount = data.values.size();

        if (!data.descriptions.empty()) {
            element->meta[SerializeKey::Description] = DescriptionToRefract(data);
        }

        SetElementType(element, value.node->valueDefinition.typeDefinition);
        AttachSourceMap(element, MakeNodeInfo(value.node, value.sourceMap));

        NodeInfoCollection<mson::TypeSections> typeSections(MAKE_NODE_INFO(value, sections));

        std::for_each(typeSections.begin(), typeSections.end(), ExtractTypeSection<T>(data, context, value));

        if (valuesCount != data.values.size()) {
            // there are some values coming from TypeSections -> move first value into examples
            MoveFirstValueToSample<T>()(value, data);
        }

        TransformElementData(element, data);

        return element;
    }

    static bool VariablePropertyIsString(const mson::ValueDefinition& variable, ConversionContext& context)
    {
        if (variable.typeDefinition.typeSpecification.name.base == mson::StringTypeName) {
            return true;
        }

        if (refract::TypeQueryVisitor::as<refract::StringElement>(FindRootAncestor(variable.typeDefinition.typeSpecification.name.symbol.literal, context.GetNamedTypesRegistry()))) {
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
                context.warn(
                    snowcrash::Warning(
                        "multiple variables in property definition is not implemented",
                        snowcrash::MSONError,
                        sourceMap.sourceMap));
            }

            // variable containt type definition
            if (!property.node->name.variable.typeDefinition.empty()) {
                if (!VariablePropertyIsString(property.node->name.variable, context)) {
                    delete key;
                    throw snowcrash::Error("'variable named property' must be string or its sub-type", snowcrash::MSONError, sourceMap.sourceMap);
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
    refract::MemberElement* RefractElementFromProperty(const NodeInfo<mson::PropertyMember>& property, ConversionContext& context, const mson::BaseTypeName defaultNestedType)
    {
        refract::IElement* key = GetPropertyKey(property, context);
        refract::IElement* value = RefractElementFromValue<T>(NodeInfo<mson::ValueMember>(property.node, property.sourceMap), context, defaultNestedType, false);
        refract::MemberElement* element = new refract::MemberElement(key, value);

        mson::TypeAttributes attrs = property.node->valueDefinition.typeDefinition.attributes;

        // there is no source map for attributes
        if (refract::IElement* attributes = MsonTypeAttributesToRefract(attrs)) {
            element->attributes[SerializeKey::TypeAttributes] = attributes;
        }

        std::string description;
        std::string& descriptionRef = description;
        Join join(descriptionRef);
        snowcrash::SourceMap<std::string> sourceMap;

        refract::IElement::MemberElementCollection::iterator iterator = value->meta.find(SerializeKey::Description);
        if (iterator != value->meta.end()) {
            // There is already setted description to "value" as result of `RefractElementFromValue()`
            // so we need to move this atribute from "value" up to MemberElement
            //
            // NOTE: potentionaly unsafe, but we set it already to StringElement
            // most safe is check it via refract::TypeQueryVisitor
            descriptionRef = (static_cast<refract::StringElement*>((*iterator)->value.second)->value);
            element->meta.push_back(*iterator);
            value->meta.std::vector<refract::MemberElement*>::erase(iterator);
            // FIXME: extract source map
        }
        else {
            join(property.node->description);
            sourceMap.sourceMap.append(property.sourceMap->description.sourceMap);
        }

        bool addNewLine = false;
        if (!descriptionRef.empty()) {
            addNewLine = true;
        }

        NodeInfoCollection<mson::TypeSections> typeSections(MAKE_NODE_INFO(property, sections));

        for (NodeInfoCollection<mson::TypeSections>::const_iterator it = typeSections.begin(); it != typeSections.end(); ++it) {
           if (it->node->klass == mson::TypeSection::BlockDescriptionClass) {
               if (addNewLine) {
                   descriptionRef.append("\n");
                   addNewLine = false;
               }

               join(it->node->content.description);
               sourceMap.sourceMap.append(it->sourceMap->description.sourceMap);
           }
        }

        if (!description.empty()) {
            element->meta[SerializeKey::Description] = PrimitiveToRefract(MakeNodeInfo(description, sourceMap));
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

        template<typename T> static ElementType* Invoke(const InputType& prop, ConversionContext& context, const mson::BaseTypeName defaultNestedType) {
                return RefractElementFromProperty<T>(prop, context, defaultNestedType);
        }
    };

    struct ValueTrait {
        typedef refract::IElement ElementType;
        typedef NodeInfo<mson::ValueMember> InputType;

        template<typename T> static ElementType* Invoke (const InputType& val, ConversionContext& context, const mson::BaseTypeName defaultNestedType) {
                return RefractElementFromValue<T>(val, context, defaultNestedType);
        }
    };

    static void CheckTypeAttributesClash(const mson::TypeAttributes& attributes,
                                         const snowcrash::SourceMap<mson::ValueDefinition>& sourceMap,
                                         ConversionContext& context) {

        if ((attributes & mson::FixedTypeAttribute) != 0 &&
            (attributes & mson::OptionalTypeAttribute) != 0) {

            context.warn(snowcrash::Warning("cannot use 'fixed' and 'optional' together",
                                            snowcrash::MSONError, sourceMap.sourceMap));
        }

        if ((attributes & mson::RequiredTypeAttribute) != 0 &&
            (attributes & mson::OptionalTypeAttribute) != 0) {

            context.warn(snowcrash::Warning("cannot use 'required' and 'optional' together",
                                            snowcrash::MSONError, sourceMap.sourceMap));
        }

        if ((attributes & mson::DefaultTypeAttribute) != 0 &&
            (attributes & mson::SampleTypeAttribute) != 0) {

            context.warn(snowcrash::Warning("cannot use 'default' and 'sample' together",
                                            snowcrash::MSONError, sourceMap.sourceMap));
        }

        if ((attributes & mson::FixedTypeAttribute) != 0 &&
            (attributes & mson::FixedTypeTypeAttribute) != 0) {

            context.warn(snowcrash::Warning("cannot use 'fixed' and 'fixed-type' together",
                                            snowcrash::MSONError, sourceMap.sourceMap));
        }
    }

    template <typename Trait>
    static refract::IElement* MsonMemberToRefract(const typename Trait::InputType& input,
                                                  ConversionContext& context,
                                                  const mson::BaseTypeName nameType,
                                                  const mson::BaseTypeName defaultNestedType,
                                                  bool checkTypeAttributes = true) {

        if (checkTypeAttributes) {
            CheckTypeAttributesClash(input.node->valueDefinition.typeDefinition.attributes, input.sourceMap->valueDefinition, context);
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

            case mson::UndefinedTypeName:
            {
                if (ValueHasChildren(input.node)) {
                    // FIXME: what about EnumElement
                    return Trait::template Invoke<refract::ArrayElement>(input, context, defaultNestedType);
                }
                else if (ValueHasName(input.node) || ValueHasMembers(input.node)) {
                    return Trait::template Invoke<refract::ObjectElement>(input, context, defaultNestedType);
                }
                else if (nameType != defaultNestedType) {
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

        for (NodeInfoCollection<mson::OneOf>::const_iterator it = oneOfNodeInfo.begin(); it != oneOfNodeInfo.end(); ++it) {

            refract::OptionElement* option = new refract::OptionElement;

            // we can not use MsonElementToRefract() for groups,
            // "option" element handles directly all elements in group
            if (it->node->klass == mson::Element::GroupClass) {
                option->set(MsonElementsToRefract(MakeNodeInfo(it->node->content.elements(), it->sourceMap->elements()), context));
            }
            else {
                option->push_back(MsonElementToRefract(*it, context, mson::StringTypeName));
            }

            select->push_back(option);
        }

        return select;
    }

    static refract::IElement* MsonMixinToRefract(const NodeInfo<mson::Mixin>& mixin)
    {
        refract::ObjectElement* ref = new refract::ObjectElement;
        ref->element(SerializeKey::Ref);
        ref->renderType(refract::IElement::rCompact);

        refract::MemberElement* href = new refract::MemberElement;
        href->set(SerializeKey::Href, refract::IElement::Create(mixin.node->typeSpecification.name.symbol.literal));
        ref->push_back(href);

        refract::MemberElement* path = new refract::MemberElement;
        path->set(SerializeKey::Path,refract::IElement::Create(SerializeKey::Content));
        ref->push_back(path);

        return ref;
    }

    static refract::IElement* MsonElementToRefract(const NodeInfo<mson::Element>& mse, ConversionContext& context, const mson::BaseTypeName defaultNestedType/* = mson::StringTypeName */)
    {
        switch (mse.node->klass) {
            case mson::Element::PropertyClass:
                return MsonMemberToRefract<PropertyTrait>(MakeNodeInfo(mse.node->content.property, mse.sourceMap->property),
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

    template<typename T>
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

        TransformElementData<T>(element, data);

        if (refract::IElement* description = DescriptionToRefract(data)) {
            element->meta[SerializeKey::Description] = description;
        }

        return element;
    }

    refract::IElement* MSONToRefract(const NodeInfo<snowcrash::DataStructure>& dataStructure, ConversionContext& context)
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
