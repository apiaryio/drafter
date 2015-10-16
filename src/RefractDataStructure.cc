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

namespace drafter {

    template <typename T, typename V = typename T::ValueType>
    struct Append {
        typedef T ElementType;
        typedef V ValueType;
        ElementType*& element;
        Append(ElementType*& e) : element(e)
        {
        }

        void operator()(const SectionInfo<ValueType>& value)
        {
            //throw std::logic_error("Can not append to primitive type");
            // FIXME: snowcrash warn about "Primitive type can not have member"
            // but in real it create "empty" member
            //
            // solution for now: set if element has no already value, otherwise silently ignore
            if (element->empty()) {
                element->set(value.section);
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

        void operator()(const SectionInfo<ValueType>& value)
        {
            for_each(value.section.begin(), value.section.end(), std::bind1st(std::mem_fun(&ElementType::push_back), element));
        }
    };


    typedef std::vector<refract::IElement*> RefractElements;

    static void SetElementType(refract::IElement* element, const mson::TypeDefinition& td)
    {
        if (!td.typeSpecification.name.symbol.literal.empty()) {
            element->element(td.typeSpecification.name.symbol.literal);
        }
        else if (td.typeSpecification.name.base == mson::EnumTypeName) {
            element->element(SerializeKey::Enum);
        }
    }

    template<typename T>
    static mson::BaseTypeName GetType(const T& type) {
        return type.typeDefinition.typeSpecification.name.base;
    }

    static refract::ArrayElement* MsonTypeAttributesToRefract(const mson::TypeAttributes& ta)
    {
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
            case mson::UndefinedTypeName:
                return oef;
            default:
                ; // do nothing
        }

        throw snowcrash::Error("unknown type of mson value", snowcrash::MSONError);
    }

    static refract::IElement* MsonElementToRefract(const SectionInfo<mson::Element>& mse, mson::BaseTypeName defaultNestedType = mson::StringTypeName);

    RefractElements MsonElementsToRefract(const SectionInfo<mson::Elements>& elements, mson::BaseTypeName defaultNestedType = mson::StringTypeName)
    {
        RefractElements result;

        // FIXME: should be used instead of "for loop" below, but there is some problem with
        // std::bind2nd && enum, will be fixed
        //
        //std::transform(elements.begin(), elements.end(),
        //               std::back_inserter(result),
        //               std::bind2nd(std::ptr_fun(MsonElementToRefract), nestedTypeName));
        //

        SectionInfoCollection<mson::Elements> elementsSectionInfo(elements.section, elements.sourceMap);
        
        for (SectionInfoCollection<mson::Elements>::ConstIterarator it = elementsSectionInfo.sections.begin() ; it != elementsSectionInfo.sections.end() ; ++it) {
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

        // NOTE: use deque instead of vector, becouse avoid trouble with std::vector<bool> in SectionInfo<bool>
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
            U operator()(const SectionInfo<mson::TypeSection>& t, const mson::BaseTypeName& defaultNestedType) {
                return LiteralTo<U>(t.section.content.value);
            }
        };

        template<bool dummy>
        struct Fetch<RefractElements, dummy> {
            RefractElements operator()(const SectionInfo<mson::TypeSection>& t, const mson::BaseTypeName& defaultNestedType) {
                return MsonElementsToRefract(MakeSectionInfo(t.section.content.elements(), t.sourceMap.elements(), t.hasSourceMap()), defaultNestedType);
            }
        };

        template <typename U, bool dummy = true>
        struct FetchSourceMap {
            snowcrash::SourceMap<U> operator()(const SectionInfo<mson::TypeSection>& t, const mson::BaseTypeName& defaultNestedType) {
                // conversion of source map from "string" into "typed" sourcemap
                if (!t.hasSourceMap()) {
                    return SectionInfo<U>::NullSourceMap();
                }

                snowcrash::SourceMap<U> sourceMap;
                sourceMap.sourceMap = t.sourceMap.value.sourceMap;
                return sourceMap;
            }
        };

        template <typename U, bool dummy = true> struct FetchTypeDefinition;

        template<bool dummy>
        struct FetchTypeDefinition<snowcrash::DataStructure, dummy> {
            const mson::TypeDefinition& operator()(const snowcrash::DataStructure& ds) {
                return ds.typeDefinition;
            }
        };

        template<bool dummy>
        struct FetchTypeDefinition<mson::ValueMember, dummy> {
            const mson::TypeDefinition& operator()(const mson::ValueMember& vm) {
                return vm.valueDefinition.typeDefinition;
            }
        };

        template<typename V>
        struct Store {
            void operator()(RefractElements& elements, const V& v) {
                T* element = new T;
                element->set(v);
                elements.push_back(element);
            }
        };

    public:

        template<typename U>
        ExtractTypeSection(ElementData<T>& data, const SectionInfo<U>& sectionHolder)
          : data(data),
            elementTypeName(FetchTypeDefinition<U>()(sectionHolder.section).typeSpecification.name.base),
            defaultNestedType(SelectNestedTypeSpecification(FetchTypeDefinition<U>()(sectionHolder.section).typeSpecification.nestedTypes))
        {}

        void operator()(const SectionInfo<mson::TypeSection>& ts) {
            Fetch<ValueType> fetch;
            FetchSourceMap<ValueType> fetchSourceMap;
            Store<ValueType> store;

            switch (ts.section.klass) {

                case mson::TypeSection::MemberTypeClass:
                    // Primitives should not contain members
                    // this is to avoid push "empty" elements to primitives
                    // it is related to test/fixtures/mson/primitive-with-members.apib
                    //
                    // FIXME: handle this by specialization for **Primitives**
                    // rewrite it to similar way to ExtractValueMember
                    if (!ts.section.content.elements().empty()) { 
                      data.values.push_back(fetch(ts, defaultNestedType));
                      data.valuesSourceMap.push_back(fetchSourceMap(ts, defaultNestedType));
                    }
                    break;

                case mson::TypeSection::SampleClass:
                    store(data.samples, fetch(ts, defaultNestedType));
                    break;

                case mson::TypeSection::DefaultClass:
                    store(data.defaults, fetch(ts, defaultNestedType));
                    break;

                case mson::TypeSection::BlockDescriptionClass:
                    data.descriptions.push_back(ts.section.content.description);
                    data.descriptionsSourceMap.push_back(ts.sourceMap.description);
                    break;

                default:
                    throw snowcrash::Error("unknown section type", snowcrash::MSONError);
            }
        }
    };

    template <typename T, typename V = typename T::ValueType>
    struct ExtractValueMember
    { 
        typedef T ElementType;
        typedef typename ElementData<T>::ValueCollectionType ValueCollectionType;

        ElementData<T>& data;

        template<typename Storage, bool dummy = true> struct Store;

        template<bool dummy>
        struct Store<ValueCollectionType, dummy> { // values, primitives
            void operator()(ValueCollectionType& storage, const typename T::ValueType& v) {
                storage.push_back(v);
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
            void operator()(S& storage, const SectionInfo<mson::ValueMember>& vm) {
                if (vm.section.valueDefinition.values.size() > 1) {
                    throw std::logic_error("For primitive types is just one value supported");
                } 

                const mson::Value& value = *vm.section.valueDefinition.values.begin();

                Store<S>()(storage, LiteralTo<U>(value.literal));
            }
        };

        template<bool dummy> 
        struct Fetch<RefractElements, dummy> { // Array|Object

            template <typename S>
            void operator()(S& storage, const SectionInfo<mson::ValueMember>& vm) {
                const mson::BaseTypeName type = SelectNestedTypeSpecification(vm.section.valueDefinition.typeDefinition.typeSpecification.nestedTypes);

                RefractElementFactory& elementFactory = FactoryFromType(type);
                const mson::Values& values = vm.section.valueDefinition.values;

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
            void operator()(S& storage, const SectionInfo<mson::ValueMember>& vm) {

                snowcrash::SourceMap<typename T::ValueType> sourceMap = SectionInfo<typename T::ValueType>::NullSourceMap();
                if (vm.hasSourceMap()) {
                    sourceMap.sourceMap = vm.sourceMap.valueDefinition.sourceMap;
                }

                storage.push_back(sourceMap);
            }
        };

        template<typename X, bool dummy = true>
        struct InjectNestedTypeInfo {
            void operator()(const SectionInfo<mson::ValueMember>& vm, ValueCollectionType&) {
                // do nothing
            }
        };

        template<bool dummy>
        struct InjectNestedTypeInfo<RefractElements, dummy> {
            void operator()(const SectionInfo<mson::ValueMember>& vm, ValueCollectionType& values) {
                // inject type info into arrays [ "type", {}, {}, null ]
                const mson::TypeNames& nestedTypes = vm.section.valueDefinition.typeDefinition.typeSpecification.nestedTypes;
                if (!nestedTypes.empty() && GetType(vm.section.valueDefinition) != mson::EnumTypeName) {

                    RefractElements types;
                    for (mson::TypeNames::const_iterator it = nestedTypes.begin() ; it != nestedTypes.end(); ++it) {
                        RefractElementFactory& f = FactoryFromType(it->base);
                        types.push_back(f.Create(it->symbol.literal, it->symbol.variable));
                    }

                    values.push_back(types);
                }
            }
        };

        template<typename X, bool dummy = true>
        struct InjectNestedTypeInfoSourceMaps {
            void operator()(const SectionInfo<mson::ValueMember>& vm, std::vector<snowcrash::SourceMap<typename T::ValueType> >&) {
            }
        };

        template<bool dummy>
        struct InjectNestedTypeInfoSourceMaps<RefractElements, dummy> {
            void operator()(const SectionInfo<mson::ValueMember>& vm, std::vector<snowcrash::SourceMap<typename T::ValueType> >& values) {
                // inject type info into arrays [ "type", {}, {}, null ]
                const mson::TypeNames& nestedTypes = vm.section.valueDefinition.typeDefinition.typeSpecification.nestedTypes;

                if (!nestedTypes.empty() && GetType(vm.section.valueDefinition) != mson::EnumTypeName) {
                    snowcrash::SourceMap<typename T::ValueType> sourceMap = SectionInfo<typename T::ValueType>::NullSourceMap();
                    if (vm.hasSourceMap()) {
                        sourceMap.sourceMap = vm.sourceMap.valueDefinition.sourceMap;
                    }

                    values.push_back(sourceMap);
                }
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

        void operator ()(const SectionInfo<mson::ValueMember>& vm)
        {
            Fetch<typename T::ValueType> fetch;
            FetchSourceMap<typename T::ValueType> fetchSourceMap;

            if (!vm.section.valueDefinition.values.empty()) {
                mson::TypeAttributes attrs = vm.section.valueDefinition.typeDefinition.attributes;
                const mson::Value& value = *vm.section.valueDefinition.values.begin();

                if (attrs & mson::DefaultTypeAttribute) {
                    fetch(data.defaults, vm);
                }
                else if ((attrs & mson::SampleTypeAttribute) || IsValueVariable<typename T::ValueType>()(value)) {
                    fetch(data.samples, vm);
                }
                else {
                    fetch(data.values, vm);
                    fetchSourceMap(data.valuesSourceMap, vm);
                }
            }

            if (!vm.section.description.empty()) {
                data.descriptions.push_back(vm.section.description);
                data.descriptionsSourceMap.push_back(vm.sourceMap.description);
            }

            if (vm.section.valueDefinition.values.empty() || (vm.section.valueDefinition.typeDefinition.typeSpecification.nestedTypes.size() > 1)) {
                InjectNestedTypeInfo<typename T::ValueType>()(vm, data.values);
                InjectNestedTypeInfoSourceMaps<typename T::ValueType>()(vm, data.valuesSourceMap);
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
            else if (query.get() == refract::TypeQueryVisitor::Object) {
                children = &static_cast<refract::ObjectElement*>(element)->value;

            }

            if (children) {
                for_each((*children).begin(), (*children).end(),
                         std::bind2nd(std::mem_fun((void (refract::IElement::*)(const refract::IElement::renderFlags))&refract::IElement::renderType), refract::IElement::rFull));
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
        struct MakeSectionInfo_ {
            const bool hasSourceMap;
            MakeSectionInfo_(bool hasSourceMap) : hasSourceMap(hasSourceMap) {}

            SectionInfo<T> operator()(const T& v, const snowcrash::SourceMap<T>& sm) {
                return MakeSectionInfo<T>(v, sm, hasSourceMap);
            }
        };

        template<typename T>
        void TransformElementData(T* element, ElementData<T>& data, bool hasSourceMap) {

            if (data.values.size() != data.valuesSourceMap.size()) {
                throw std::logic_error("Internal: count of source maps is not equal to count of elements");
            }

            typedef std::vector<SectionInfo< typename T::ValueType> > ValueSectionInfoCollection;
            ValueSectionInfoCollection valuesSectionInfo = Zip<ValueSectionInfoCollection>(data.values, data.valuesSourceMap, MakeSectionInfo_<typename T::ValueType>(hasSourceMap));

            std::for_each(valuesSectionInfo.begin(), valuesSectionInfo.end(), Append<T>(element));

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

        refract::StringElement* element = new refract::StringElement;
        std::string description;
        Join join(description);

        for_each(data.descriptions.begin(), data.descriptions.end(), join);

        element->set(description);

        snowcrash::SourceMap<std::string> sourceMap;
        typedef typename std::vector<snowcrash::SourceMap<std::string> >::const_iterator iterator;

        for (iterator it = data.descriptionsSourceMap.begin() ; it != data.descriptionsSourceMap.end() ; ++it) {
            // FIXME: sourcemaps comming from snowcrash are empty there 
            sourceMap.sourceMap.append(it->sourceMap);
        }

        PrimitiveToRefract(SectionInfo<std::string>(description, sourceMap));

        return element; 
    }

    template <typename T>
    refract::IElement* RefractElementFromValue(const SectionInfo<mson::ValueMember>& value, const mson::BaseTypeName defaultNestedType)
    {
        using namespace refract;
        typedef T ElementType;

        ElementData<T> data;
        ElementType* element = new ElementType;

        ExtractValueMember<ElementType>(data, defaultNestedType)(value);

        size_t valuesCount = data.values.size();

        if (!data.descriptions.empty()) {
            element->meta[SerializeKey::Description] = DescriptionToRefract(data);
        }

        SetElementType(element, value.section.valueDefinition.typeDefinition);

        SectionInfoCollection<mson::TypeSections> typeSections(value.section.sections, value.sourceMap.sections);

        std::for_each(typeSections.sections.begin(), typeSections.sections.end(), ExtractTypeSection<T>(data, value));
        
        if (!value.section.valueDefinition.values.empty() && (valuesCount != data.values.size())) { 
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
    refract::MemberElement* RefractElementFromProperty(const SectionInfo<mson::PropertyMember>& property, const mson::BaseTypeName defaultNestedType)
    {
        refract::MemberElement* element = new refract::MemberElement;
        refract::IElement* value = RefractElementFromValue<T>(SectionInfo<mson::ValueMember>(property.section, property.sourceMap), defaultNestedType);

        if (!property.section.name.literal.empty()) {
            snowcrash::SourceMap<mson::Literal> sourceMap;
            sourceMap.sourceMap.append(property.sourceMap.name.sourceMap);

            refract::IElement* key = PrimitiveToRefract(MakeSectionInfo(property.section.name.literal, sourceMap, property.hasSourceMap()));

            element->set(key, value);
        }
        else if (!property.section.name.variable.values.empty()) {

            if (property.section.name.variable.values.size() > 1) {
                // FIXME: is there example for multiple variables?
                throw snowcrash::Error("multiple variables in property definition are not allowed", snowcrash::MSONError);
            }

            snowcrash::SourceMap<mson::Literal> sourceMap;
            sourceMap.sourceMap.append(property.sourceMap.name.sourceMap);

            refract::IElement* key = PrimitiveToRefract(MakeSectionInfo(property.section.name.variable.values.begin()->literal, sourceMap, property.hasSourceMap()));

            key->attributes[SerializeKey::Variable] = refract::IElement::Create(true);

            element->set(key, value);

            SetElementType(element->value.first, property.section.name.variable.typeDefinition);
        }
        else {
            throw snowcrash::Error("no property name", snowcrash::MSONError);
        }

        mson::TypeAttributes attrs = property.section.valueDefinition.typeDefinition.attributes;
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
            join(property.section.description);
            sourceMap.sourceMap.append(property.sourceMap.description.sourceMap);
        }

        bool addNewLine = false;
        if (!descriptionRef.empty()) {
            addNewLine = true;
        }

        SectionInfoCollection<mson::TypeSections> typeSections(property.section.sections, property.sourceMap.sections);

        for (SectionInfoCollection<mson::TypeSections>::ConstIterarator it = typeSections.sections.begin() ; it != typeSections.sections.end(); ++it) {
           if (it->section.klass == mson::TypeSection::BlockDescriptionClass) {
               if (addNewLine) {
                   descriptionRef.append("\n");
                   addNewLine = false;
               }
               join(it->section.content.description);
               sourceMap.sourceMap.append(it->sourceMap.description.sourceMap);
           }
        }

        if (!description.empty()) {
            element->meta[SerializeKey::Description] = PrimitiveToRefract(MakeSectionInfo(description, sourceMap, property.hasSourceMap()));
        }

        return element;
    }

    static bool ValueHasMembers(const mson::ValueMember& value)
    {
        for (mson::TypeSections::const_iterator it = value.sections.begin(); it != value.sections.end(); ++it) {
            if (it->klass == mson::TypeSection::MemberTypeClass) {
                return true;
            }
        }
        return false;
    }

    static bool ValueHasChildren(const mson::ValueMember& value)
    {
        return value.valueDefinition.values.size() > 1;
    }

    static bool ValueHasName(const mson::ValueMember& value)
    {
        return !value.valueDefinition.typeDefinition.typeSpecification.name.symbol.literal.empty();
    }

    struct PropertyTrait {
        typedef refract::MemberElement ElementType;
        typedef SectionInfo<mson::PropertyMember> InputType;

        template<typename T> static ElementType* Invoke(const InputType& prop, const mson::BaseTypeName defaultNestedType) {
                return RefractElementFromProperty<T>(prop, defaultNestedType);
        }
    };

    struct ValueTrait {
        typedef refract::IElement ElementType;
        typedef SectionInfo<mson::ValueMember> InputType;

        template<typename T> static ElementType* Invoke (const InputType& val, const mson::BaseTypeName defaultNestedType) {
                return RefractElementFromValue<T>(val, defaultNestedType);
        }
    };

    template <typename Trait>
    static refract::IElement* MsonMemberToRefract(const typename Trait::InputType& input, const mson::BaseTypeName defaultNestedType) {
        mson::BaseTypeName nameType = GetType(input.section.valueDefinition);
        switch (nameType) {
            case mson::BooleanTypeName:
                return Trait::template Invoke<refract::BooleanElement>(input, defaultNestedType);

            case mson::NumberTypeName:
                return Trait::template Invoke<refract::NumberElement>(input, defaultNestedType);

            case mson::StringTypeName:
                return Trait::template Invoke<refract::StringElement>(input, defaultNestedType);

            case mson::EnumTypeName:
            case mson::ArrayTypeName:
                return Trait::template Invoke<refract::ArrayElement>(input, defaultNestedType);

            case mson::ObjectTypeName:
                return Trait::template Invoke<refract::ObjectElement>(input, defaultNestedType);

            case mson::UndefinedTypeName:
            {
                if (ValueHasChildren(input.section)) {
                    return Trait::template Invoke<refract::ArrayElement>(input, defaultNestedType);
                }
                else if (ValueHasName(input.section) || ValueHasMembers(input.section)) {
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

    static refract::IElement* MsonOneofToRefract(const SectionInfo<mson::OneOf>& oneOf)
    {
        refract::ArrayElement* select = new refract::ArrayElement;
        select->element(SerializeKey::Select);

        SectionInfoCollection<mson::OneOf> oneOfSectionInfo(oneOf.section, oneOf.sourceMap);

        for (SectionInfoCollection<mson::OneOf>::ConstIterarator it = oneOfSectionInfo.sections.begin(); it != oneOfSectionInfo.sections.end(); ++it) {
            refract::ArrayElement* option = new refract::ArrayElement;
            option->element(SerializeKey::Option);

            // we can not use MsonElementToRefract() for groups,
            // "option" element handles directly all elements in group
            if (it->section.klass == mson::Element::GroupClass) {
                option->set(MsonElementsToRefract(MakeSectionInfo(it->section.content.elements(), it->sourceMap.elements(), it->hasSourceMap())));
            }
            else {
                option->push_back(MsonElementToRefract(*it, mson::StringTypeName));
            }

            select->push_back(option);
        }

        return select;
    }

    static refract::IElement* MsonMixinToRefract(const SectionInfo<mson::Mixin>& mixin)
    {
        refract::ObjectElement* ref = new refract::ObjectElement;
        ref->element(SerializeKey::Ref);
        ref->renderType(refract::IElement::rCompact);

        refract::MemberElement* href = new refract::MemberElement;
        href->set(SerializeKey::Href, refract::IElement::Create(mixin.section.typeSpecification.name.symbol.literal));
        ref->push_back(href);

        refract::MemberElement* path = new refract::MemberElement;
        path->set(SerializeKey::Path,refract::IElement::Create(SerializeKey::Content));
        ref->push_back(path);

        return ref;
    }

    static refract::IElement* MsonElementToRefract(const SectionInfo<mson::Element>& mse, const mson::BaseTypeName defaultNestedType/* = mson::StringTypeName */)
    {
        switch (mse.section.klass) {
            case mson::Element::PropertyClass:
                return MsonMemberToRefract<PropertyTrait>(MakeSectionInfo(mse.section.content.property, mse.sourceMap.property, mse.hasSourceMap()), defaultNestedType);

            case mson::Element::ValueClass:
                return MsonMemberToRefract<ValueTrait>(MakeSectionInfo(mse.section.content.value, mse.sourceMap.value, mse.hasSourceMap()), defaultNestedType);

            case mson::Element::MixinClass:
                return MsonMixinToRefract(MakeSectionInfo(mse.section.content.mixin, mse.sourceMap.mixin, mse.hasSourceMap()));

            case mson::Element::OneOfClass:
                return MsonOneofToRefract(MakeSectionInfo(mse.section.content.oneOf(), mse.sourceMap.oneOf(), mse.hasSourceMap()));

            case mson::Element::GroupClass:
                throw snowcrash::Error("unable to handle element group", snowcrash::MSONError);

            default:
                throw snowcrash::Error("unknown type of mson element", snowcrash::MSONError);
        }
    }

    template<typename T>
    refract::IElement* RefractElementFromMSON(const SectionInfo<snowcrash::DataStructure>& ds)
    {
        using namespace refract;
        typedef T ElementType;

        ElementType* element = new ElementType;
        SetElementType(element, ds.section.typeDefinition);

        if (!ds.section.name.symbol.literal.empty()) {
            snowcrash::SourceMap<mson::Literal> sourceMap = SectionInfo<mson::Literal>::NullSourceMap();

            sourceMap.sourceMap.append(ds.sourceMap.name.sourceMap);
            
            element->meta[SerializeKey::Id] = PrimitiveToRefract(MakeSectionInfo(ds.section.name.symbol.literal, sourceMap, ds.hasSourceMap()));
        }

        ElementData<T> data;

        SectionInfoCollection<mson::TypeSections> typeSections(ds.section.sections, ds.sourceMap.sections);

        std::for_each(typeSections.sections.begin(), typeSections.sections.end(), ExtractTypeSection<T>(data, ds));

        TransformElementData<T>(element, data, ds.hasSourceMap());

        if (refract::IElement* description = DescriptionToRefract(data)) {
            element->meta[SerializeKey::Description] = description;
        }

        return element;
    }

    refract::IElement* MSONToRefract(const SectionInfo<snowcrash::DataStructure>& dataStructure)
    {
        if (dataStructure.section.empty()) {
            return NULL;
        }

        using namespace refract;
        IElement* element = NULL;

        mson::BaseTypeName nameType = GetType(dataStructure.section);
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
            case mson::ArrayTypeName:
                element = RefractElementFromMSON<refract::ArrayElement>(dataStructure);
                break;

            case mson::ObjectTypeName:
            case mson::UndefinedTypeName:
                element = RefractElementFromMSON<refract::ObjectElement>(dataStructure);
                break;

            default:
                throw std::runtime_error("Unhandled type of DataStructure");
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
