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

#include "NamedTypesRegistry.h"

namespace drafter {

    template <typename T, typename V = typename T::ValueType>
    struct Append {
        typedef T ElementType;
        typedef V ValueType;
        ElementType*& element;

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
    };

    template <typename T>
    struct Append<T, std::vector<refract::IElement*> > {
        typedef T ElementType;
        typedef typename T::ValueType ValueType;
        ElementType*& element;

        Append(ElementType*& e) : element(e)
        {
        }

        void operator()(const NodeInfo<ValueType>& value)
        {
            std::for_each(value.node->begin(), value.node->end(), std::bind1st(std::mem_fun(&ElementType::push_back), element));
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
        type.visit(*element);

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
    static mson::BaseTypeName GetType(const T& type) {
        mson::BaseTypeName nameType = type.typeDefinition.typeSpecification.name.base;
        const std::string& parent = type.typeDefinition.typeSpecification.name.symbol.literal;

        if (nameType == mson::UndefinedTypeName && !parent.empty()) {
            refract::IElement* base = FindRootAncestor(parent, GetNamedTypesRegistry());
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
        if (ta & mson::NullableTypeAttribute) {
            attr->push_back(refract::IElement::Create(SerializeKey::Nullable));
        }

        if (attr->value.empty()) {
            delete attr;
            attr = NULL;
        }

        return attr;
    }

    struct RefractElementFactory
    {
        virtual ~RefractElementFactory() {}
        virtual refract::IElement* Create(const std::string& literal, bool) = 0;
    };

    template <typename E>
    struct RefractElementFactoryImpl : RefractElementFactory
    {
        virtual refract::IElement* Create(const std::string& literal, bool sample = false)
        {
            E* element = new E;

            if (literal.empty()) {
                return element;
            }

            if (sample) {
                refract::ArrayElement* samples = new refract::ArrayElement;
                samples->push_back(refract::IElement::Create(LiteralTo<typename E::ValueType>(literal)));
                element->attributes[SerializeKey::Samples] = samples;
            }
            else {
                element->set(LiteralTo<typename E::ValueType>(literal));
            }

            return element;
        }
    };

    template <>
    struct RefractElementFactoryImpl<refract::ObjectElement> : RefractElementFactory
    {
        virtual refract::IElement* Create(const std::string& literal, bool sample = false)
        {
            if (sample) {
                refract::StringElement* element = new refract::StringElement;
                element->element(SerializeKey::Generic);
                element->set(literal);
                return element;
            }

            refract::ObjectElement* element = new refract::ObjectElement;

            if (literal.empty()) {
                return element;
            }

            element->element(literal);

            return element;
        }
    };


    RefractElementFactory& FactoryFromType(const mson::BaseTypeName typeName)
    {
        static RefractElementFactoryImpl<refract::BooleanElement> bef;
        static RefractElementFactoryImpl<refract::NumberElement> nef;
        static RefractElementFactoryImpl<refract::StringElement> sef;
        static RefractElementFactoryImpl<refract::ObjectElement> oef;

        switch (typeName) {
            case mson::BooleanTypeName:
                return bef;
            case mson::NumberTypeName:
                return nef;
            case mson::StringTypeName:
                return sef;
            case mson::ObjectTypeName:
            case mson::UndefinedTypeName:
                return oef;
            default:
                ; // do nothing
        }

        throw snowcrash::Error("unknown type of mson value", snowcrash::MSONError);
    }

    static refract::IElement* MsonElementToRefract(const NodeInfo<mson::Element>& mse, mson::BaseTypeName defaultNestedType = mson::StringTypeName);

    RefractElements MsonElementsToRefract(const NodeInfo<mson::Elements>& elements, mson::BaseTypeName defaultNestedType = mson::StringTypeName)
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
            result.push_back(MsonElementToRefract(*it, defaultNestedType));
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
    struct ElementData {
        typedef T ElementType;

        typedef typename T::ValueType ValueType;
        typedef snowcrash::SourceMap<ValueType> ValueSourceMapType;

        // NOTE: use deque instead of vector, becouse avoid trouble with std::vector<bool> in NodeInfo<bool>
        typedef std::deque<ValueType> ValueCollectionType;
        typedef std::vector<ValueSourceMapType> ValueSourceMapCollectionType;

        ValueCollectionType values;
        ValueSourceMapCollectionType valuesSourceMap;

        RefractElements defaults;
        RefractElements samples;

        std::vector<std::string> descriptions;
        std::vector<snowcrash::SourceMap<std::string> > descriptionsSourceMap;
    };

    template <typename T>
    class ExtractTypeSection
    {
        typedef typename T::ValueType ValueType;

        ElementData<T>& data;
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
            U operator()(const NodeInfo<mson::TypeSection>& typeSection, const mson::BaseTypeName& defaultNestedType) {
                return LiteralTo<U>(typeSection.node->content.value);
            }
        };

        template<bool dummy>
        struct Fetch<RefractElements, dummy> {
            RefractElements operator()(const NodeInfo<mson::TypeSection>& typeSection, const mson::BaseTypeName& defaultNestedType) {
                return MsonElementsToRefract(MakeNodeInfo(typeSection.node->content.elements(),
                                                          typeSection.sourceMap->elements(),
                                                          typeSection.hasSourceMap()),
                                             defaultNestedType);
            }
        };

        template <typename U, bool dummy = true>
        struct FetchSourceMap {
            snowcrash::SourceMap<U> operator()(const NodeInfo<mson::TypeSection>& typeSection, const mson::BaseTypeName& defaultNestedType) {
                // conversion of source map from "string" into "typed" sourcemap
                if (!typeSection.hasSourceMap()) {
                    return *NodeInfo<U>::NullSourceMap();
                }

                snowcrash::SourceMap<U> sourceMap;
                sourceMap.sourceMap = typeSection.sourceMap->value.sourceMap;
                return sourceMap;
            }
        };

        template <typename U, bool dummy = true>
        struct FetchTypeDefinition;

        template<bool dummy>
        struct FetchTypeDefinition<snowcrash::DataStructure, dummy> {
            const mson::TypeDefinition& operator()(const snowcrash::DataStructure& dataStructure) {
                return dataStructure.typeDefinition;
            }
        };

        template<bool dummy>
        struct FetchTypeDefinition<mson::ValueMember, dummy> {
            const mson::TypeDefinition& operator()(const mson::ValueMember& valueMember) {
                return valueMember.valueDefinition.typeDefinition;
            }
        };

        template<typename V>
        struct Store {
            void operator()(RefractElements& elements, const V& value) {
                T* element = new T;
                element->set(value);
                elements.push_back(element);
            }
        };

    public:

        template<typename U>
        ExtractTypeSection(ElementData<T>& data, const NodeInfo<U>& sectionHolder)
          : data(data),
            elementTypeName(FetchTypeDefinition<U>()(*sectionHolder.node).typeSpecification.name.base),
            defaultNestedType(SelectNestedTypeSpecification(FetchTypeDefinition<U>()(*sectionHolder.node).typeSpecification.nestedTypes))
        {}

        void operator()(const NodeInfo<mson::TypeSection>& typeSection) {
            Fetch<ValueType> fetch;
            FetchSourceMap<ValueType> fetchSourceMap;
            Store<ValueType> store;

            switch (typeSection.node->klass) {

                case mson::TypeSection::MemberTypeClass:
                    // Primitives should not contain members
                    // this is to avoid push "empty" elements to primitives
                    // it is related to test/fixtures/mson/primitive-with-members.apib
                    //
                    // FIXME: handle this by specialization for **Primitives**
                    // rewrite it to similar way to ExtractValueMember
                    if (!typeSection.node->content.elements().empty()) {
                        data.values.push_back(fetch(typeSection, defaultNestedType));
                        data.valuesSourceMap.push_back(fetchSourceMap(typeSection, defaultNestedType));
                    }
                    break;

                case mson::TypeSection::SampleClass:
                    store(data.samples, fetch(typeSection, defaultNestedType));
                    break;

                case mson::TypeSection::DefaultClass:
                    store(data.defaults, fetch(typeSection, defaultNestedType));
                    break;

                case mson::TypeSection::BlockDescriptionClass:
                    data.descriptions.push_back(typeSection.node->content.description);
                    data.descriptionsSourceMap.push_back(typeSection.sourceMap->description);
                    break;

                default:
                    throw snowcrash::Error("unknown section type", snowcrash::MSONError);
            }
        }
    };


    template <typename T>
    struct ExtractTypeDefinition {

        typedef T ElementType;
        typedef typename ElementData<T>::ValueCollectionType ValueCollectionType;
        typedef typename ElementData<T>::ValueSourceMapCollectionType ValueSourceMapCollectionType;

        template<typename X, bool dummy = true>
        struct InjectNestedTypeInfo {
            void operator()(const mson::TypeNames&, ValueCollectionType&) {
                // do nothing
            }
        };

        template<bool dummy>
        struct InjectNestedTypeInfo<RefractElements, dummy> {
            void operator()(const mson::TypeNames& typeNames, ValueCollectionType& values) {
                if (typeNames.empty()) {
                    return;
                }

                RefractElements types;
                for (mson::TypeNames::const_iterator it = typeNames.begin(); it != typeNames.end(); ++it) {
                    RefractElementFactory& f = FactoryFromType(it->base);
                    types.push_back(f.Create(it->symbol.literal, it->symbol.variable));
                }

                values.push_back(types);
            }
        };

        template<typename X, bool dummy = true>
        struct InjectNestedTypeInfoSourceMaps {
            void operator()(const mson::TypeNames&, ValueSourceMapCollectionType&) {
            }
        };

        template<bool dummy>
        struct InjectNestedTypeInfoSourceMaps<RefractElements, dummy> {
            void operator()(const mson::TypeNames& typeNames, ValueSourceMapCollectionType& values) {
                if (typeNames.empty()) {
                    return;
                }

                values.push_back(*NodeInfo<typename T::ValueType>::NullSourceMap());
            }
        };

        ElementData<ElementType>& data;
        ExtractTypeDefinition(ElementData<ElementType>& data) : data(data) {}

        void operator()(const NodeInfo<mson::TypeDefinition>& typeDefinition) {
            InjectNestedTypeInfo<typename T::ValueType>()(typeDefinition.node->typeSpecification.nestedTypes, data.values);
            InjectNestedTypeInfoSourceMaps<typename T::ValueType>()(typeDefinition.node->typeSpecification.nestedTypes, data.valuesSourceMap);
        }
    };

    template <typename T, typename V = typename T::ValueType>
    struct ExtractValueMember
    {
        typedef T ElementType;
        typedef typename ElementData<T>::ValueCollectionType ValueCollectionType;
        typedef typename ElementData<T>::ValueSourceMapCollectionType ValueSourceMapCollectionType;

        ElementData<T>& data;

        template<typename Storage, bool dummy = true> struct Store;

        template<bool dummy>
        struct Store<ValueCollectionType, dummy> { // values, primitives
            void operator()(ValueCollectionType& storage, const typename T::ValueType& value) {
                storage.push_back(value);
            }
        };

        template<bool dummy>
        struct Store<RefractElements, dummy> {
            void operator()(RefractElements& storage, const typename T::ValueType& value) {
                ElementType* element = new ElementType;
                element->set(value);
                storage.push_back(element);
            }
        };

        template <typename U, bool dummy = true>
        struct Fetch {  // primitive values

            template <typename S>
            void operator()(S& storage, const NodeInfo<mson::ValueMember>& valueMember) {
                if (valueMember.node->valueDefinition.values.size() > 1) {
                    throw snowcrash::Error("only one value is supported for primitive types", snowcrash::MSONError);
                }

                const mson::Value& value = *valueMember.node->valueDefinition.values.begin();

                Store<S>()(storage, LiteralTo<U>(value.literal));
            }
        };

        template<bool dummy>
        struct Fetch<RefractElements, dummy> { // Array|Object

            template <typename S>
            void operator()(S& storage, const NodeInfo<mson::ValueMember>& valueMember) {
                const mson::BaseTypeName type = SelectNestedTypeSpecification(valueMember.node->valueDefinition.typeDefinition.typeSpecification.nestedTypes);

                RefractElementFactory& elementFactory = FactoryFromType(type);
                const mson::Values& values = valueMember.node->valueDefinition.values;

                RefractElements elements;

                for (mson::Values::const_iterator it = values.begin(); it != values.end(); ++it) {
                    refract::IElement* element = elementFactory.Create(it->literal, it->variable);
                    elements.push_back(element);
                }

                Store<S>()(storage, elements);
            }
        };

        template <typename U, bool dummy = true>
        struct FetchSourceMap {  // primitive values

            template <typename S>
            void operator()(S& storage, const NodeInfo<mson::ValueMember>& valueMember) {
                snowcrash::SourceMap<typename T::ValueType> sourceMap = *NodeInfo<typename T::ValueType>::NullSourceMap();

                if (valueMember.hasSourceMap()) {
                    sourceMap.sourceMap = valueMember.sourceMap->valueDefinition.sourceMap;
                }

                storage.push_back(sourceMap);
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

        ExtractValueMember(ElementData<T>& data, const mson::BaseTypeName) : data(data) {}

        void operator ()(const NodeInfo<mson::ValueMember>& valueMember)
        {
            Fetch<typename T::ValueType> fetch;
            FetchSourceMap<typename T::ValueType> fetchSourceMap;

            if (!valueMember.node->valueDefinition.values.empty()) {
                mson::TypeAttributes attrs = valueMember.node->valueDefinition.typeDefinition.attributes;
                const mson::Value& value = *valueMember.node->valueDefinition.values.begin();

                if (attrs & mson::DefaultTypeAttribute) {
                    fetch(data.defaults, valueMember);
                }
                else if ((attrs & mson::SampleTypeAttribute) || IsValueVariable<typename T::ValueType>()(value)) {
                    fetch(data.samples, valueMember);
                }
                else {
                    fetch(data.values, valueMember);
                    fetchSourceMap(data.valuesSourceMap, valueMember);
                }
            }

            if (!valueMember.node->description.empty()) {
                data.descriptions.push_back(valueMember.node->description);
                data.descriptionsSourceMap.push_back(valueMember.sourceMap->description);
            }

            if ((valueMember.node->valueDefinition.values.empty() ||
                (valueMember.node->valueDefinition.typeDefinition.typeSpecification.nestedTypes.size() > 1))  &&
                (GetType(valueMember.node->valueDefinition) != mson::EnumTypeName)) {

                ExtractTypeDefinition<T> extd(data);
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
            element->content(query);

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

        void SaveSamples(RefractElements& samples, refract::IElement* element) {

            std::for_each(samples.begin(), samples.end(), SetSerializeFlag);

            if (!samples.empty()) {
                refract::ArrayElement* a = new refract::ArrayElement;
                a->set(samples);
                element->attributes[SerializeKey::Samples] = a;
            }
        }

        void SaveDefault(RefractElements& defaults, refract::IElement* element) {

            std::for_each(defaults.begin(), defaults.end(), SetSerializeFlag);

            if (!defaults.empty()) {
                refract::IElement* e = *defaults.rbegin();
                defaults.pop_back();
                // if more default values
                // use last one, all other we will drop
                element->attributes[SerializeKey::Default] = e;

                std::for_each(defaults.begin(), defaults.end(), Deleter<refract::IElement>);
            }
        }

        template <typename T>
        struct MakeNodeInfoFunctor {
            const bool hasSourceMap;
            MakeNodeInfoFunctor(bool hasSourceMap) : hasSourceMap(hasSourceMap) {}

            NodeInfo<T> operator()(const T& v, const snowcrash::SourceMap<T>& sm) {
                return MakeNodeInfo<T>(v, sm, hasSourceMap);
            }
        };

        template<typename T>
        void TransformElementData(T* element, ElementData<T>& data, bool hasSourceMap) {

            if (data.values.size() != data.valuesSourceMap.size()) {
                throw snowcrash::Error("count of source maps is not equal to count of elements");
            }

            typedef std::vector<NodeInfo< typename T::ValueType> > ValueNodeInfoCollection;
            ValueNodeInfoCollection valuesNodeInfo = Zip<ValueNodeInfoCollection>(data.values, data.valuesSourceMap, MakeNodeInfoFunctor<typename T::ValueType>(hasSourceMap));

            std::for_each(valuesNodeInfo.begin(), valuesNodeInfo.end(), Append<T>(element));

            SaveSamples(data.samples, element);

            SaveDefault(data.defaults, element);
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
    refract::IElement* RefractElementFromValue(const NodeInfo<mson::ValueMember>& value, const mson::BaseTypeName defaultNestedType)
    {
        using namespace refract;
        typedef T ElementType;

        ElementData<ElementType> data;
        ElementType* element = new ElementType;

        ExtractValueMember<ElementType>(data, defaultNestedType)(value);

        size_t valuesCount = data.values.size();

        if (!data.descriptions.empty()) {
            element->meta[SerializeKey::Description] = DescriptionToRefract(data);
        }

        SetElementType(element, value.node->valueDefinition.typeDefinition);

        NodeInfoCollection<mson::TypeSections> typeSections(MAKE_NODE_INFO(value, sections));

        std::for_each(typeSections.begin(), typeSections.end(), ExtractTypeSection<T>(data, value));

        if (!value.node->valueDefinition.values.empty() && (valuesCount != data.values.size())) {
            // there are some values coming from TypeSections -> move first value into examples
            ElementType* element = new ElementType;
            element->set(data.values.front());
            data.samples.insert(data.samples.begin(), element);
            data.values.erase(data.values.begin());

            // FIXME append source map into "sample"
            data.valuesSourceMap.erase(data.valuesSourceMap.begin());
        }

        TransformElementData(element, data, value.hasSourceMap());

        return element;
    }

    template <typename T>
    refract::MemberElement* RefractElementFromProperty(const NodeInfo<mson::PropertyMember>& property, const mson::BaseTypeName defaultNestedType)
    {
        refract::MemberElement* element = new refract::MemberElement;
        refract::IElement* value = RefractElementFromValue<T>(NodeInfo<mson::ValueMember>(property.node, property.sourceMap), defaultNestedType);

        if (!property.node->name.literal.empty()) {
            snowcrash::SourceMap<mson::Literal> sourceMap;
            sourceMap.sourceMap.append(property.sourceMap->name.sourceMap);

            refract::IElement* key = PrimitiveToRefract(MakeNodeInfo(property.node->name.literal, sourceMap, property.hasSourceMap()));

            element->set(key, value);
        }
        else if (!property.node->name.variable.values.empty()) {

            if (property.node->name.variable.values.size() > 1) {
                // FIXME: is there example for multiple variables?
                throw snowcrash::Error("multiple variables in property definition are not implemented", snowcrash::MSONError);
            }

            snowcrash::SourceMap<mson::Literal> sourceMap;
            sourceMap.sourceMap.append(property.sourceMap->name.sourceMap);

            // check if base variable type is StringElement
            if (!property.node->name.variable.typeDefinition.empty()) {
                const std::string& type = property.node->name.variable.typeDefinition.typeSpecification.name.symbol.literal;
                if (!type.empty()) {
                    if (!refract::TypeQueryVisitor::as<refract::StringElement>(FindRootAncestor(type, GetNamedTypesRegistry()))) {
                        throw snowcrash::Error("'variable named property' must be string or its sub-type", snowcrash::MSONError);
                    }
                }
            }

            refract::IElement* key = PrimitiveToRefract(MakeNodeInfo(property.node->name.variable.values.begin()->literal, sourceMap, property.hasSourceMap()));

            key->attributes[SerializeKey::Variable] = refract::IElement::Create(true);

            element->set(key, value);

            SetElementType(element->value.first, property.node->name.variable.typeDefinition);
        }
        else {
            throw snowcrash::Error("no property name", snowcrash::MSONError);
        }

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
            element->meta[SerializeKey::Description] = PrimitiveToRefract(MakeNodeInfo(description, sourceMap, property.hasSourceMap()));
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

        template<typename T> static ElementType* Invoke(const InputType& prop, const mson::BaseTypeName defaultNestedType) {
                return RefractElementFromProperty<T>(prop, defaultNestedType);
        }
    };

    struct ValueTrait {
        typedef refract::IElement ElementType;
        typedef NodeInfo<mson::ValueMember> InputType;

        template<typename T> static ElementType* Invoke (const InputType& val, const mson::BaseTypeName defaultNestedType) {
                return RefractElementFromValue<T>(val, defaultNestedType);
        }
    };

    template <typename Trait>
    static refract::IElement* MsonMemberToRefract(const typename Trait::InputType& input, const mson::BaseTypeName defaultNestedType) {
        mson::BaseTypeName nameType = GetType(input.node->valueDefinition);

        switch (nameType) {
            case mson::BooleanTypeName:
                return Trait::template Invoke<refract::BooleanElement>(input, defaultNestedType);

            case mson::NumberTypeName:
                return Trait::template Invoke<refract::NumberElement>(input, defaultNestedType);

            case mson::StringTypeName:
                return Trait::template Invoke<refract::StringElement>(input, defaultNestedType);

            case mson::EnumTypeName:
                return Trait::template Invoke<refract::EnumElement>(input, defaultNestedType);

            case mson::ArrayTypeName:
                return Trait::template Invoke<refract::ArrayElement>(input, defaultNestedType);

            case mson::ObjectTypeName:
                return Trait::template Invoke<refract::ObjectElement>(input, defaultNestedType);

            case mson::UndefinedTypeName:
            {
                if (ValueHasChildren(input.node)) {
                    // FIXME: what about EnumElement
                    return Trait::template Invoke<refract::ArrayElement>(input, defaultNestedType);
                }
                else if (ValueHasName(input.node) || ValueHasMembers(input.node)) {
                    return Trait::template Invoke<refract::ObjectElement>(input, defaultNestedType);
                }

                switch (defaultNestedType) {
                   case mson::BooleanTypeName:
                       return Trait::template Invoke<refract::BooleanElement>(input, defaultNestedType);

                   case mson::NumberTypeName:
                       return Trait::template Invoke<refract::NumberElement>(input, defaultNestedType);

                   case mson::StringTypeName:
                       return Trait::template Invoke<refract::StringElement>(input, defaultNestedType);

                   default:
                       throw snowcrash::Error("nested complex types are not implemented", snowcrash::MSONError);
                }
            }

            default:
                throw snowcrash::Error("unknown type of mson member", snowcrash::MSONError);
        }
    }

    static refract::IElement* MsonOneofToRefract(const NodeInfo<mson::OneOf>& oneOf)
    {
        refract::ArrayElement* select = new refract::ArrayElement;
        select->element(SerializeKey::Select);

        NodeInfoCollection<mson::OneOf> oneOfNodeInfo(oneOf);

        for (NodeInfoCollection<mson::OneOf>::const_iterator it = oneOfNodeInfo.begin(); it != oneOfNodeInfo.end(); ++it) {
            refract::ArrayElement* option = new refract::ArrayElement;
            option->element(SerializeKey::Option);

            // we can not use MsonElementToRefract() for groups,
            // "option" element handles directly all elements in group
            if (it->node->klass == mson::Element::GroupClass) {
                option->set(MsonElementsToRefract(MakeNodeInfo(it->node->content.elements(), it->sourceMap->elements(), it->hasSourceMap())));
            }
            else {
                option->push_back(MsonElementToRefract(*it, mson::StringTypeName));
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

    static refract::IElement* MsonElementToRefract(const NodeInfo<mson::Element>& mse, const mson::BaseTypeName defaultNestedType/* = mson::StringTypeName */)
    {
        switch (mse.node->klass) {
            case mson::Element::PropertyClass:
                return MsonMemberToRefract<PropertyTrait>(MakeNodeInfo(mse.node->content.property, mse.sourceMap->property, mse.hasSourceMap()), defaultNestedType);

            case mson::Element::ValueClass:
                return MsonMemberToRefract<ValueTrait>(MakeNodeInfo(mse.node->content.value, mse.sourceMap->value, mse.hasSourceMap()), defaultNestedType);

            case mson::Element::MixinClass:
                return MsonMixinToRefract(MakeNodeInfo(mse.node->content.mixin, mse.sourceMap->mixin, mse.hasSourceMap()));

            case mson::Element::OneOfClass:
                return MsonOneofToRefract(MakeNodeInfo(mse.node->content.oneOf(), mse.sourceMap->oneOf(), mse.hasSourceMap()));

            case mson::Element::GroupClass:
                throw snowcrash::Error("unable to handle element group", snowcrash::MSONError);

            default:
                throw snowcrash::Error("unknown type of mson element", snowcrash::MSONError);
        }
    }

    template<typename T>
    refract::IElement* RefractElementFromMSON(const NodeInfo<snowcrash::DataStructure>& ds)
    {
        using namespace refract;
        typedef T ElementType;

        ElementType* element = new ElementType;
        SetElementType(element, ds.node->typeDefinition);

        if (!ds.node->name.symbol.literal.empty()) {
            snowcrash::SourceMap<mson::Literal> sourceMap = *NodeInfo<mson::Literal>::NullSourceMap();
            sourceMap.sourceMap.append(ds.sourceMap->name.sourceMap);
            element->meta[SerializeKey::Id] = PrimitiveToRefract(MakeNodeInfo(ds.node->name.symbol.literal, sourceMap, ds.hasSourceMap()));
        }

        ElementData<T> data;

        ExtractTypeDefinition<ElementType> extd(data);
        extd(MAKE_NODE_INFO(ds, typeDefinition));

        NodeInfoCollection<mson::TypeSections> typeSections(MAKE_NODE_INFO(ds, sections));

        std::for_each(typeSections.begin(), typeSections.end(), ExtractTypeSection<T>(data, ds));

        TransformElementData<T>(element, data, ds.hasSourceMap());

        if (refract::IElement* description = DescriptionToRefract(data)) {
            element->meta[SerializeKey::Description] = description;
        }

        return element;
    }

    refract::IElement* MSONToRefract(const NodeInfo<snowcrash::DataStructure>& dataStructure)
    {
        if (dataStructure.node->empty()) {
            return NULL;
        }

        using namespace refract;
        IElement* element = NULL;

        mson::BaseTypeName nameType = GetType(*dataStructure.node);

        switch (nameType) {
            case mson::BooleanTypeName:
                element = RefractElementFromMSON<refract::BooleanElement>(dataStructure);
                break;

            case mson::NumberTypeName:
                element = RefractElementFromMSON<refract::NumberElement>(dataStructure);
                break;

            case mson::StringTypeName:
                element = RefractElementFromMSON<refract::StringElement>(dataStructure);
                break;

            case mson::EnumTypeName:
                element = RefractElementFromMSON<refract::EnumElement>(dataStructure);
                break;

            case mson::ArrayTypeName:
                element = RefractElementFromMSON<refract::ArrayElement>(dataStructure);
                break;

            case mson::ObjectTypeName:
            case mson::UndefinedTypeName:
                element = RefractElementFromMSON<refract::ObjectElement>(dataStructure);
                break;

            default:
                throw snowcrash::Error("unknown type of data structure", snowcrash::MSONError);
        }

        return element;
    }

    refract::IElement* ExpandRefract(refract::IElement* element, const refract::Registry& registry)
    {
        if (!element) {
            return element;
        }

        refract::ExpandVisitor expander(registry);
        expander.visit(*element);

        if (refract::IElement* expanded = expander.get()) {
            delete element;
            element = expanded;
        }

        return element;
    }

    sos::Object SerializeRefract(refract::IElement* element)
    {
        if (!element) {
            return sos::Object();
        }

        refract::SerializeVisitor serializer;
        serializer.visit(*element);

        return serializer.get();
    }

} // namespace drafter
