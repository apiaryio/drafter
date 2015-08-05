//
//  RefractDataStructure.cc
//  drafter
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include <iterator>

#include "RefractDataStructure.h"
#include "refract/Element.h"
#include "refract/Visitors.h"
#include "refract/AppendDecorator.h"

#include "refract/Registry.h"

namespace drafter
{
    namespace key 
    {

        // Refract meta
        const std::string Id = "id";
        const std::string Description = "description";
        const std::string Title = "title";

        // Refract MSON attributes
        const std::string Samples = "samples";
        const std::string Default = "default";
        const std::string Variable = "variable";
        const std::string TypeAttributes = "typeAttributes";

        // Refract MSON attribute "typeAttibute" values
        const std::string Required = "required";
        const std::string Optional = "optional";
        const std::string Fixed = "fixed";

        // Literal to Bool
        const std::string True = "true";

        // Refract MSON generic element
        const std::string Generic = "generic";

        // Refract (nontyped) element names
        // - maybe move into "librefract" 
        const std::string Enum = "enum";
        const std::string Select = "select";
        const std::string Option = "option";
        const std::string Ref = "ref";

        // Refract Ref Element - keys/values
        const std::string Href = "href";
        const std::string Path = "path";
        const std::string Content = "content";

    }
}

namespace drafter
{
    typedef std::vector<refract::IElement*> RefractElements;

    static void SetElementType(const mson::TypeDefinition& td, refract::IElement* element) 
    {
        if (!td.typeSpecification.name.symbol.literal.empty()) {
            element->element(td.typeSpecification.name.symbol.literal);
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
            attr->push_back(refract::IElement::Create(key::Required));
        }
        if (ta & mson::OptionalTypeAttribute) {
            attr->push_back(refract::IElement::Create(key::Optional));
        }
        if (ta & mson::FixedTypeAttribute) {
            attr->push_back(refract::IElement::Create(key::Fixed));
        }

        if (attr->value.empty()) {
            delete attr;
            attr = NULL;
        }

        return attr;
    }

    template <typename T>
    T LiteralTo(const mson::Literal& literal);

    template <>
    bool LiteralTo<bool>(const mson::Literal& literal)
    {
        return literal == key::True;
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
                refract::ArrayElement* a = new refract::ArrayElement;
                a->push_back(refract::IElement::Create(LiteralTo<typename E::ValueType>(literal)));
                element->attributes[key::Samples] = a;
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
                element->element(key::Generic);
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

    static refract::IElement* MsonElementToRefract(const mson::Element& mse);

    RefractElements MsonElementsToRefract(const mson::Elements& elements) {
        RefractElements result;
        std::transform(elements.begin(), elements.end(), std::back_inserter(result), MsonElementToRefract);
        return result;
    }

    struct TypeSectionData {
        RefractElements defaults;
        RefractElements samples;
        std::vector<std::string> descriptions;
    };

    template <typename T>
    struct ExtractTypeSection
    { 
        std::vector<typename T::ValueType> values;
        TypeSectionData& data;

        /**
         * Fetch<> is intended to extract value from TypeSection.
         * Generalized type is for primitive types
         * Specialized is for (Array|Object)Element because of underlying type.
         * `dummy` param is used because of specialization inside another struct
         */

        template <typename U, bool dummy = true>
        struct Fetch {
            U operator()(const mson::TypeSection& t) {
                return LiteralTo<U>(t.content.value);
            }
        };

        template<bool dummy> 
        struct Fetch<RefractElements, dummy> {
            RefractElements operator()(const mson::TypeSection& t) {
                return MsonElementsToRefract(t.content.elements());
            }
        };

        ExtractTypeSection(TypeSectionData& data) : data(data)
        {
        }

        void operator()(const mson::TypeSection& ts) {
            Fetch<typename T::ValueType> fetch;

            if (ts.klass == mson::TypeSection::MemberTypeClass) {
                values.push_back(fetch(ts));
                return;
            }

            if (ts.klass == mson::TypeSection::SampleClass) {
                T* e = new T;
                e->set(fetch(ts));
                data.samples.push_back(e);
                return;
            }

            if (ts.klass == mson::TypeSection::DefaultClass) {
                T* e = new T;
                e->set(fetch(ts));
                data.defaults.push_back(e);
                return;
            }

            if (ts.klass == mson::TypeSection::BlockDescriptionClass){ 
                data.descriptions.push_back(ts.content.description);
                return;
            }
            
            throw std::logic_error("Unexpected section type for property");
        }

    };


    template <typename T, typename V = typename T::ValueType>
    struct ExtractValueMember
    { // This will handle primitive elements
        const mson::ValueMember& vm;
        typedef T ElementType;

        RefractElements& defaults;
        RefractElements& samples;

        ExtractValueMember(const mson::ValueMember& v, RefractElements& defaults, RefractElements& samples) 
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

            return element;
        }
    };

    static mson::BaseTypeName SelectNestedTypeSpecification(const mson::TypeNames& nestedTypes) {
        mson::BaseTypeName type = mson::StringTypeName;
        // Found if type of element is specified.
        // if more types is used - fallback to "StringType"
        if (nestedTypes.size() == 1) {
            type = nestedTypes.begin()->base;
        }
        return type;
    }

    template <typename T>
    struct ExtractValueMember<T, RefractElements>
    { 
        const mson::ValueMember& vm;
        typedef RefractElements V;
        typedef T ElementType;

        RefractElements& defaults;
        RefractElements& samples;

        ExtractValueMember(const mson::ValueMember& v, RefractElements& defaults, RefractElements& samples) : vm(v), defaults(defaults), samples(samples) {}

        operator T*()
        {
            ElementType* element = new ElementType;
            const mson::TypeNames& nestedTypes = vm.valueDefinition.typeDefinition.typeSpecification.nestedTypes;

            if (!vm.valueDefinition.values.empty()) {
                mson::BaseTypeName type = SelectNestedTypeSpecification(nestedTypes);

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

                size_t reserve = append.length();

                if (!base.empty()) {
                    reserve += separator.length();
                    base.append(separator);
                }

                base.append(append);

            }
        };

        void SaveSamples(RefractElements& samples, refract::IElement* element) {
            if (!samples.empty()) {
                refract::ArrayElement* a = new refract::ArrayElement;
                a->set(samples);
                element->attributes[key::Samples] = a;
            }
        }

        void SaveDefault(RefractElements& defaults, refract::IElement* element) {

            if (!defaults.empty()) {
                refract::IElement* e = *defaults.rbegin();
                defaults.pop_back();
                // if more default values
                // use last one, all other we will drop
                element->attributes[key::Default] = e;

                std::for_each(defaults.begin(), defaults.end(), Deleter<refract::IElement>);
            }
        }

        template<typename T>
        void TransformTypeSectionData(const mson::TypeSections& sections, T* element, TypeSectionData& data) {

            // FIXME: for Array/Enum - extract *type of element* from
            // value.valueDefinition.typeDefinition.typeSpecification.nestedTypes[];
            // and inject into ExtractTypeSection
            // reason - value defined as list eg.
            // - value: 1,2,3,4,5 (array[number, string])
            // does not hold type resp. is defaultly set as mson::UndefinedTypeName
            //
            // fallback for now - present all as refract::StringElement

            ExtractTypeSection<T> extractor = std::for_each(sections.begin(), sections.end(), ExtractTypeSection<T>(data));

            std::for_each(extractor.values.begin(), extractor.values.end(), refract::AppendDecorator<T>(element));

            SaveSamples(data.samples, element);

            SaveDefault(data.defaults, element);
        }
    }

    template <typename T>
    refract::IElement* RefractElementFromValue(const mson::ValueMember& value)
    {
        using namespace refract;
        typedef T ElementType;

        TypeSectionData data;

        ElementType* element = ExtractValueMember<ElementType>(value, data.defaults, data.samples);

        SetElementType(value.valueDefinition.typeDefinition, element);

        TransformTypeSectionData(value.sections, element, data);

        return element;
    }

    template <typename T>
    refract::MemberElement* RefractElementFromProperty(const mson::PropertyMember& property)
    {
        refract::MemberElement* element = new refract::MemberElement;
        refract::IElement* value = RefractElementFromValue<T>(property);

        if (!property.name.literal.empty()) {
            element->set(property.name.literal, value);
        }
        else if (!property.name.variable.values.empty()) {

            if (property.name.variable.values.size() > 1) {
                // FIXME: is there example for multiple variables?
                throw std::logic_error("Multiple variables in property definition");
            }

            element->set(property.name.variable.values.begin()->literal, value);
            element->value.first->attributes[key::Variable] = refract::IElement::Create(true);
            SetElementType(property.name.variable.typeDefinition, element->value.first);
        } 
        else {
            throw std::logic_error("No property name");
        }

        mson::TypeAttributes attrs = property.valueDefinition.typeDefinition.attributes;
        if (refract::IElement* attributes = MsonTypeAttributesToRefract(attrs)) {
            element->attributes[key::TypeAttributes] = attributes;
        }

        std::string description;
        Join join(description);
        join(property.description);

        bool addNewLine = false;
        if (!description.empty()) {
            addNewLine = true;
        }

        for (mson::TypeSections::const_iterator it = property.sections.begin(); it != property.sections.end(); ++it) {
            if (it->klass == mson::TypeSection::BlockDescriptionClass){ 
                if (addNewLine) {
                    description.append("\n");
                    addNewLine = false;
                }
                join(it->content.description);
            }
        }

        if (!description.empty()) {
            element->meta[key::Description] = refract::IElement::Create(description);
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

        template<typename T> static ElementType* Invoke(const mson::PropertyMember& prop) {
                return RefractElementFromProperty<T>(prop);
        }

        static ElementType* makeEnum(ElementType* element) {
            if (element && element->value.second) {
                element->value.second->element(key::Enum);
            }
            return element;
        }
    };

    struct ValueTrait {
        typedef refract::IElement ElementType;

        template<typename T> static ElementType* Invoke (const mson::ValueMember& val) {
                return RefractElementFromValue<T>(val);
        }

        static ElementType* makeEnum(ElementType* element) {
            if (element) {
                element->element(key::Enum);
            }
            return element;
        }
    };

    template <typename Trait, typename Input>
    static refract::IElement* MsonMemberToRefract(const Input& input) {
        mson::BaseTypeName nameType = GetType(input.valueDefinition);
        switch (nameType) {
            case mson::BooleanTypeName:
                return Trait::template Invoke<refract::BooleanElement>(input);

            case mson::NumberTypeName:
                return Trait::template Invoke<refract::NumberElement>(input);

            case mson::StringTypeName:
                return Trait::template Invoke<refract::StringElement>(input);

            case mson::EnumTypeName:
                return Trait::makeEnum(Trait::template Invoke<refract::ArrayElement>(input));

            case mson::ArrayTypeName:
                return Trait::template Invoke<refract::ArrayElement>(input);

            case mson::ObjectTypeName:
                return Trait::template Invoke<refract::ObjectElement>(input);

            case mson::UndefinedTypeName:
                if (ValueHasChildren(input)) {
                    return Trait::template Invoke<refract::ArrayElement>(input);
                }
                else if (ValueHasName(input) || ValueHasMembers(input)) {
                    return Trait::template Invoke<refract::ObjectElement>(input);
                }
                return Trait::template Invoke<refract::StringElement>(input);

            default:
                throw std::runtime_error("Unhandled type of Member");
        }
    }

    static refract::IElement* MsonOneofToRefract(const mson::OneOf& oneOf)
    {
        refract::ArrayElement* select = new refract::ArrayElement;
        select->element(key::Select);
        for (mson::Elements::const_iterator it = oneOf.begin(); it != oneOf.end(); ++it) {
            refract::ArrayElement* option = new refract::ArrayElement;
            option->element(key::Option);
            // we can not use MsonElementToRefract() for groups, 
            // "option" element handles directly all elements in group
            if (it->klass == mson::Element::GroupClass) { 
                option->set(MsonElementsToRefract(it->content.elements()));
            }
            else {
                option->push_back(MsonElementToRefract(*it));
            }
            select->push_back(option);
        }
        return select;
    }

    static refract::IElement* MsonMixinToRefract(const mson::Mixin& mixin)
    {
        refract::ObjectElement* ref = new refract::ObjectElement;
        ref->element(key::Ref);
        ref->renderType(refract::IElement::rCompact);

        refract::MemberElement* href = new refract::MemberElement;
        href->set(key::Href, refract::IElement::Create(mixin.typeSpecification.name.symbol.literal));
        ref->push_back(href);

        refract::MemberElement* path = new refract::MemberElement;
        path->set(key::Path,refract::IElement::Create(key::Content));
        ref->push_back(path);

        return ref;
    }


    static refract::IElement* MsonElementToRefract(const mson::Element& mse)
    {
        switch (mse.klass) {
            case mson::Element::PropertyClass:
                return MsonMemberToRefract<PropertyTrait>(mse.content.property);

            case mson::Element::ValueClass:
                return MsonMemberToRefract<ValueTrait>(mse.content.value);

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
    refract::IElement* RefractElementFromDataStructure(const snowcrash::DataStructure& ds)
    {
        using namespace refract;
        typedef T ElementType;

        ElementType* e = new ElementType;
        SetElementType(ds.typeDefinition, e);

        if (!ds.name.symbol.literal.empty()) {
            e->meta[key::Id] = IElement::Create(ds.name.symbol.literal);
        }

        // FIXME: "title" is temporary commented, until clear refract spec 
        // in few examples for named object is "title" attribute used
        // sometime is not used.
        
        //e->meta[key::Title] = IElement::Create(ds.name.symbol.literal);

        TypeSectionData data;

        TransformTypeSectionData(ds.sections, e, data);

        std::string description;
        std::for_each(data.descriptions.begin(), data.descriptions.end(), Join(description));

        if(!description.empty()) {
            e->meta[key::Description] = IElement::Create(description);
        }

        return e;
    }


    refract::IElement* DataStructureToRefract(const snowcrash::DataStructure& dataStructure)
    {
        using namespace refract;
        IElement* element = NULL;

        mson::BaseTypeName nameType = GetType(dataStructure);
        switch (nameType) {
            case mson::BooleanTypeName:
                element = RefractElementFromDataStructure<refract::BooleanElement>(dataStructure);
                break;

            case mson::NumberTypeName:
                element = RefractElementFromDataStructure<refract::NumberElement>(dataStructure);
                break;

            case mson::StringTypeName:
                element = RefractElementFromDataStructure<refract::StringElement>(dataStructure);
                break;

            case mson::EnumTypeName: {
                element = RefractElementFromDataStructure<refract::ArrayElement>(dataStructure);
                if (element) {
                    element->element(key::Enum);
                }
                break;
            }

            case mson::ArrayTypeName:
                element = RefractElementFromDataStructure<refract::ArrayElement>(dataStructure);
                break;

            case mson::ObjectTypeName:
            case mson::UndefinedTypeName:
                element = RefractElementFromDataStructure<refract::ObjectElement>(dataStructure);
                break;

            default:
                throw std::runtime_error("Unhandled type of DataStructure");
        }

        return element;
    }

    sos::Object SerializeRefract(refract::IElement* element, const refract::Registry& registry) {

        if (!element) {
            return sos::Object();
        }

        refract::ExpandVisitor expander(registry);
        expander.visit(*element);

        if (refract::IElement* expanded = expander.get()) {
           element = expanded;
        }

        refract::SerializeVisitor serializer;
        serializer.visit(*element);

        if (expander.get()) {
           delete element;
        }

        return serializer.get();
    }

} // namespace drafter
