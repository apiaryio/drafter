//
//  RefractDataStructure.cc
//  drafter
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "RefractDataStructure.h"
#include "refract/AppendDecorator.h"

namespace drafter {

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

        if (attr->value.empty()) {
            delete attr;
            attr = NULL;
        }

        return attr;
    }

    template <>
    bool LiteralTo<bool>(const mson::Literal& literal)
    {
        return literal == SerializeKey::True;
    }

    template <>
    double LiteralTo<double>(const mson::Literal& literal)
    {
        return atof(literal.c_str());
    }

    template <>
    std::string LiteralTo<std::string>(const mson::Literal& literal)
    {
        return literal;
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

        throw std::logic_error("Out of scope - ElementFactory for type not implemented");
    }

    static refract::IElement* MsonElementToRefract(const mson::Element& mse, mson::BaseTypeName defaultNestedType = mson::StringTypeName);

    RefractElements MsonElementsToRefract(const mson::Elements& elements, mson::BaseTypeName defaultNestedType = mson::StringTypeName)
    {
        RefractElements result;

        // FIXME: should be used instead of "for loop" below, but there is some problem with
        // std::bind2nd && enum, will be fixed
        //
        //std::transform(elements.begin(), elements.end(),
        //               std::back_inserter(result),
        //               std::bind2nd(std::ptr_fun(MsonElementToRefract), nestedTypeName));

        for (mson::Elements::const_iterator it = elements.begin() ; it != elements.end() ; ++it) {
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
    struct TypeSectionData {
        std::vector<typename T::ValueType>  values;
        RefractElements defaults;
        RefractElements samples;
        std::vector<std::string> descriptions;
    };

    template <typename T>
    class ExtractTypeSection
    {
        TypeSectionData<T>& data;
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
            U operator()(const mson::TypeSection& t, const mson::BaseTypeName& defaultNestedType) {
                return LiteralTo<U>(t.content.value);
            }
        };

        template<bool dummy>
        struct Fetch<RefractElements, dummy> {
            RefractElements operator()(const mson::TypeSection& t, const mson::BaseTypeName& defaultNestedType) {
                return MsonElementsToRefract(t.content.elements(), defaultNestedType);
            }
        };

        template <typename U, bool dummy = true>
        struct FetchTypeDefinition {};

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
        ExtractTypeSection(TypeSectionData<T>& data, const U& sectionHolder)
          : data(data),
            elementTypeName(FetchTypeDefinition<U>()(sectionHolder).typeSpecification.name.base),
            defaultNestedType(SelectNestedTypeSpecification(FetchTypeDefinition<U>()(sectionHolder).typeSpecification.nestedTypes))
        {}

        void operator()(const mson::TypeSection& ts) {
            Fetch<typename T::ValueType> fetch;
            Store<typename T::ValueType> store;

            switch (ts.klass) {

                case mson::TypeSection::MemberTypeClass:
                    data.values.push_back(fetch(ts, defaultNestedType));
                    break;

                case mson::TypeSection::SampleClass:
                    store(data.samples, fetch(ts, defaultNestedType));
                    break;

                case mson::TypeSection::DefaultClass:
                    store(data.defaults, fetch(ts, defaultNestedType));
                    break;

                case mson::TypeSection::BlockDescriptionClass:
                    data.descriptions.push_back(ts.content.description);
                    break;

                default:
                    throw std::logic_error("Unexpected section type for property");
            }
        }
    };


    template <typename T, typename V = typename T::ValueType>
    struct ExtractValueMember
    { // This will handle primitive elements
        const mson::ValueMember& vm;
        typedef T ElementType;

        RefractElements& defaults;
        RefractElements& samples;

        ExtractValueMember(const mson::ValueMember& v, RefractElements& defaults, RefractElements& samples, const mson::BaseTypeName)
            : vm(v), defaults(defaults), samples(samples) {}

        operator T*()
        {
            ElementType* element = new ElementType;

            if (vm.valueDefinition.values.size() > 1) {
                throw std::logic_error("For primitive types is just one value supported");
            }

            if (!vm.valueDefinition.values.empty()) {
                mson::TypeAttributes attrs = vm.valueDefinition.typeDefinition.attributes;
                const mson::Value& value = *vm.valueDefinition.values.begin();

                if (attrs & mson::DefaultTypeAttribute) {
                    defaults.push_back(refract::IElement::Create(LiteralTo<V>(value.literal)));
                }
                else if ((attrs & mson::SampleTypeAttribute) || (value.variable)) {
                    samples.push_back(refract::IElement::Create(LiteralTo<V>(value.literal)));
                }
                else {
                    element->set(LiteralTo<V>(value.literal));
                }

            }

            if (!vm.description.empty()) {
                element->meta[SerializeKey::Description] = refract::IElement::Create(vm.description);
            }

            return element;
        }
    };

    template <typename T>
    struct ExtractValueMember<T, RefractElements>
    {
        const mson::ValueMember& vm;
        typedef RefractElements V;
        typedef T ElementType;

        RefractElements& defaults;
        RefractElements& samples;

        mson::BaseTypeName defaultNestedType;

        ExtractValueMember(const mson::ValueMember& v, RefractElements& defaults, RefractElements& samples, const mson::BaseTypeName defaultNestedType)
            : vm(v),
              defaults(defaults),
              samples(samples),
              defaultNestedType(defaultNestedType)
        {}

        operator T*()
        {
            ElementType* element = new ElementType;
            const mson::TypeNames& nestedTypes = vm.valueDefinition.typeDefinition.typeSpecification.nestedTypes;

            if (!vm.valueDefinition.values.empty()) {
                mson::BaseTypeName type = SelectNestedTypeSpecification(nestedTypes, defaultNestedType);

                RefractElementFactory& elementFactory = FactoryFromType(type);
                const mson::Values& values = vm.valueDefinition.values;

                V result;
                for (mson::Values::const_iterator it = values.begin(); it != values.end(); ++it) {
                    refract::IElement* element = elementFactory.Create(it->literal, it->variable);
                    result.push_back(element);
                }

                mson::TypeAttributes attrs = vm.valueDefinition.typeDefinition.attributes;
                if (attrs & mson::SampleTypeAttribute) {
                    ElementType* s = new ElementType;
                    s->set(result);
                    samples.push_back(s);
                }
                else if (attrs & mson::DefaultTypeAttribute) {
                    ElementType* d = new ElementType;
                    d->set(result);
                    defaults.push_back(d);
                }
                else {
                    element->set(result);
                }

                // Do not inject typeinfo if there is just one - we already use it in values
                if (nestedTypes.size() <= 1) {
                    return element;
                }

            }

            // inject type info into arrays [ "type", {}, {}, null ]
            // FIXME: what to do with `Enum`s (they hold members in `sections` instead of value
            if (!nestedTypes.empty() && GetType(vm.valueDefinition) != mson::EnumTypeName) {

                RefractElements types;
                for (mson::TypeNames::const_iterator it = nestedTypes.begin() ; it != nestedTypes.end(); ++it) {
                    RefractElementFactory& f = FactoryFromType(it->base);
                    //RefractElementFactory& f = FactoryFromType(defaultNestedType);
                    types.push_back(f.Create(it->symbol.literal, it->symbol.variable));
                }

                refract::AppendDecorator<T> append = refract::AppendDecorator<T>(element);
                append(types);

            }

            return element;
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

        template<typename T, typename U>
        void TransformTypeSectionData(const U& sectionsHolder, T* element, TypeSectionData<T>& data) {

            std::for_each(sectionsHolder.sections.begin(), sectionsHolder.sections.end(), ExtractTypeSection<T>(data, sectionsHolder));

            std::for_each(data.values.begin(), data.values.end(), refract::AppendDecorator<T>(element));

            SaveSamples(data.samples, element);

            SaveDefault(data.defaults, element);
        }
    }

    template <typename T>
    refract::IElement* RefractElementFromValue(const mson::ValueMember& value, const mson::BaseTypeName defaultNestedType)
    {
        using namespace refract;
        typedef T ElementType;

        TypeSectionData<T> data;

        ElementType* element = ExtractValueMember<ElementType>(value, data.defaults, data.samples, defaultNestedType);

        SetElementType(element, value.valueDefinition.typeDefinition);

        TransformTypeSectionData(value, element, data);

        return element;
    }

    template <typename T>
    refract::MemberElement* RefractElementFromProperty(const mson::PropertyMember& property, const mson::BaseTypeName defaultNestedType)
    {
        refract::MemberElement* element = new refract::MemberElement;
        refract::IElement* value = RefractElementFromValue<T>(property, defaultNestedType);

        if (!property.name.literal.empty()) {
            element->set(property.name.literal, value);
        }
        else if (!property.name.variable.values.empty()) {

            if (property.name.variable.values.size() > 1) {
                // FIXME: is there example for multiple variables?
                throw std::logic_error("Multiple variables in property definition");
            }

            element->set(property.name.variable.values.begin()->literal, value);
            element->value.first->attributes[SerializeKey::Variable] = refract::IElement::Create(true);
            SetElementType(element->value.first, property.name.variable.typeDefinition);
        }
        else {
            throw std::logic_error("No property name");
        }

        mson::TypeAttributes attrs = property.valueDefinition.typeDefinition.attributes;
        if (refract::IElement* attributes = MsonTypeAttributesToRefract(attrs)) {
            element->attributes[SerializeKey::TypeAttributes] = attributes;
        }

        std::string description;
        std::string& descriptionRef = description;
        Join join(descriptionRef);

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
        }
        else {
            join(property.description);
        }

        bool addNewLine = false;
        if (!descriptionRef.empty()) {
            addNewLine = true;
        }

        for (mson::TypeSections::const_iterator it = property.sections.begin(); it != property.sections.end(); ++it) {
            if (it->klass == mson::TypeSection::BlockDescriptionClass) {
                if (addNewLine) {
                    descriptionRef.append("\n");
                    addNewLine = false;
                }
                join(it->content.description);
            }
        }

        if (!description.empty()) {
            element->meta[SerializeKey::Description] = refract::IElement::Create(description);
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
        typedef mson::PropertyMember InputType;

        template<typename T> static ElementType* Invoke(const InputType& prop, const mson::BaseTypeName defaultNestedType) {
                return RefractElementFromProperty<T>(prop, defaultNestedType);
        }
    };

    struct ValueTrait {
        typedef refract::IElement ElementType;
        typedef mson::ValueMember InputType;

        template<typename T> static ElementType* Invoke (const InputType& val, const mson::BaseTypeName defaultNestedType) {
                return RefractElementFromValue<T>(val, defaultNestedType);
        }
    };

    template <typename Trait>
    static refract::IElement* MsonMemberToRefract(const typename Trait::InputType& input, const mson::BaseTypeName defaultNestedType) {
        mson::BaseTypeName nameType = GetType(input.valueDefinition);
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
                if (ValueHasChildren(input)) {
                    return Trait::template Invoke<refract::ArrayElement>(input, defaultNestedType);
                }
                else if (ValueHasName(input) || ValueHasMembers(input)) {
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
                       throw std::logic_error("Nested complex types are not Implemented");
                }
            }

            default:
                throw std::runtime_error("Unhandled type of Member");
        }
    }

    static refract::IElement* MsonOneofToRefract(const mson::OneOf& oneOf)
    {
        refract::ArrayElement* select = new refract::ArrayElement;
        select->element(SerializeKey::Select);
        for (mson::Elements::const_iterator it = oneOf.begin(); it != oneOf.end(); ++it) {
            refract::ArrayElement* option = new refract::ArrayElement;
            option->element(SerializeKey::Option);
            // we can not use MsonElementToRefract() for groups,
            // "option" element handles directly all elements in group
            if (it->klass == mson::Element::GroupClass) {
                option->set(MsonElementsToRefract(it->content.elements()));
            }
            else {
                option->push_back(MsonElementToRefract(*it, mson::StringTypeName));
            }
            select->push_back(option);
        }
        return select;
    }

    static refract::IElement* MsonMixinToRefract(const mson::Mixin& mixin)
    {
        refract::ObjectElement* ref = new refract::ObjectElement;
        ref->element(SerializeKey::Ref);
        ref->renderType(refract::IElement::rCompact);

        refract::MemberElement* href = new refract::MemberElement;
        href->set(SerializeKey::Href, refract::IElement::Create(mixin.typeSpecification.name.symbol.literal));
        ref->push_back(href);

        refract::MemberElement* path = new refract::MemberElement;
        path->set(SerializeKey::Path,refract::IElement::Create(SerializeKey::Content));
        ref->push_back(path);

        return ref;
    }

    static refract::IElement* MsonElementToRefract(const mson::Element& mse, const mson::BaseTypeName defaultNestedType/* = mson::StringTypeName */)
    {
        switch (mse.klass) {
            case mson::Element::PropertyClass:
                return MsonMemberToRefract<PropertyTrait>(mse.content.property, defaultNestedType);

            case mson::Element::ValueClass:
                return MsonMemberToRefract<ValueTrait>(mse.content.value, defaultNestedType);

            case mson::Element::MixinClass:
                return MsonMixinToRefract(mse.content.mixin);

            case mson::Element::OneOfClass:
                return MsonOneofToRefract(mse.content.oneOf());

            case mson::Element::GroupClass:
                throw std::logic_error("Group must be handled individualy");

            default:
                throw std::logic_error("Unhandled type of MSON element");
        }
    }

    template<typename T>
    refract::IElement* RefractElementFromMSON(const snowcrash::DataStructure& ds)
    {
        using namespace refract;
        typedef T ElementType;

        ElementType* element = new ElementType;
        SetElementType(element, ds.typeDefinition);

        if (!ds.name.symbol.literal.empty()) {
            element->meta[SerializeKey::Id] = IElement::Create(ds.name.symbol.literal);
        }

        TypeSectionData<T> data;

        TransformTypeSectionData<T>(ds, element, data);

        std::string description;
        std::for_each(data.descriptions.begin(), data.descriptions.end(), Join(description));

        if(!description.empty()) {
            element->meta[SerializeKey::Description] = IElement::Create(description);
        }

        return element;
    }


    refract::IElement* MSONToRefract(const snowcrash::DataStructure& dataStructure)
    {
        if (dataStructure.empty()) {
            return NULL;
        }

        using namespace refract;
        IElement* element = NULL;

        mson::BaseTypeName nameType = GetType(dataStructure);
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
