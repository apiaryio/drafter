//
//  RefractAST.cc
//  drafter
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "RefractAST.h"
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
        const std::string Sample = "sample";
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
        const std::string HRef = "href";
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

    static refract::IElement* SimplifyRefractContainer(const RefractElements& container)
    {
        if (container.empty()) {
            return NULL;
        }

        if (container.size() == 1) {
            return container[0];
        }

        refract::ArrayElement* array = new refract::ArrayElement;
       
        for (RefractElements::const_iterator it = container.begin(); it != container.end(); ++it) {
            array->push_back(*it);
        }
        return array;
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
            if (sample) {
                element->attributes[key::Sample] = refract::IElement::Create(LiteralTo<typename E::ValueType>(literal));
            }
            else {
                element->set(LiteralTo<typename E::ValueType>(literal));
            }
            return element;
        }
    };

    RefractElementFactory& FactoryFromType(const mson::BaseTypeName typeName)
    {
        static RefractElementFactoryImpl<refract::BooleanElement> bef;
        static RefractElementFactoryImpl<refract::NumberElement> nef;
        static RefractElementFactoryImpl<refract::StringElement> sef;

        switch (typeName) {
            case mson::BooleanTypeName:
                return bef;
            case mson::NumberTypeName:
                return nef;
            case mson::StringTypeName:
                return sef;
            default:
                ; // do nothing
        }

        throw std::logic_error("Out of scope - ElementFactory for type not implemted");
    }

    static refract::IElement* MsonElementToRefract(const mson::Element& mse);

    // FIXME: check against original behavioration
    template <typename T, typename V = typename T::ValueType>
    struct ExtractTypeSection
    { // This will handle primitive elements
        const mson::TypeSection& ts;

        ExtractTypeSection(const mson::TypeSection& v) : ts(v)
        {
        }

        operator V()
        {
            return LiteralTo<V>(ts.content.value);
        }
    };


    RefractElements MsonElementsToRefract(const mson::Elements& elements) {
        RefractElements result;
        for (mson::Elements::const_iterator it = elements.begin(); it != elements.end(); ++it) {
            result.push_back(MsonElementToRefract(*it));
        }
        return result;
    }

    template <typename T>
    struct ExtractTypeSection<T, RefractElements>
    { // this will handle Array && Object because of underlaying type
        const mson::TypeSection& ts;
        typedef RefractElements V;

        ExtractTypeSection(const mson::TypeSection& v) : ts(v)
        {
        }

        operator V()
        {
            return MsonElementsToRefract(ts.content.elements());
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

            if (!vm.valueDefinition.values.empty()) {
                mson::BaseTypeName type = SelectNestedTypeSpecification(vm.valueDefinition.typeDefinition.typeSpecification.nestedTypes);

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
                else {
                    element->set(result);
                }
            }
            else if (!vm.valueDefinition.typeDefinition.typeSpecification.nestedTypes.empty() 
                    && vm.valueDefinition.typeDefinition.typeSpecification.nestedTypes.begin()->symbol.variable) {
               refract::IElement* s = refract::IElement::Create(vm.valueDefinition.typeDefinition.typeSpecification.nestedTypes.begin()->symbol.literal);
               s->element(key::Generic);
               element->push_back(s);
            }

            return element;
        }
    };

    template <typename T>
    refract::IElement* RefractElementFromValue(const mson::ValueMember& value)
    {
        using namespace refract;
        typedef T ElementType;

        RefractElements defaults;
        RefractElements samples;

        ElementType* element = ExtractValueMember<ElementType>(value, defaults, samples);

        SetElementType(value.valueDefinition.typeDefinition, element);

        if (!value.sections.empty()) {
            typedef RefractElements Elements;

            // FIXME: for Array/Enum - extract *type of element* from
            // value.valueDefinition.typeDefinition.typeSpecification.nestedTypes[];
            // and inject into ExtractTypeSection
            // reason - value defined as list eg.
            // - value: 1,2,3,4,5 (array[number, string])
            // does not hold type resp. is defaultly set as mson::UndefinedTypeName
            //
            // fallback for now - present all as refract::StringElement

            for (mson::TypeSections::const_iterator it = value.sections.begin(); it != value.sections.end(); ++it) {

                if (it->klass == mson::TypeSection::MemberTypeClass) {
                    if (!element->empty()) {
                        throw std::logic_error("Element content was already set, you cannot fill it from 'memberType'");
                    }
                    element->set(ExtractTypeSection<T>(*it));
                    continue;
                }

                if (it->klass == mson::TypeSection::BlockDescriptionClass){ 
                    // do nothing, Description must be handled one more level up
                    // it is part of Property (not Value)
                    continue;
                }

                ElementType* e = new ElementType;
                e->set(ExtractTypeSection<T>(*it));

                if (it->klass == mson::TypeSection::SampleClass) {
                    samples.push_back(e);
                } 
                else if (it->klass == mson::TypeSection::DefaultClass) {
                    defaults.push_back(e);
                }
                else {
                    throw std::logic_error("Unexpected section type for property");
                }
            }

        }

        if (IElement* e = SimplifyRefractContainer(samples)) {
            element->attributes[key::Sample] = e;
        }

        if (IElement* e = SimplifyRefractContainer(defaults)) {
            element->attributes[key::Default] = e;
        }


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

            // FIXME: 
            // https://github.com/refractproject/refract-spec/blob/master/namespaces/mson-namespace.md#variable-property-name
            // ["object", {}, {}, [
            //   ["member", {}, {}, {
            //       "key": ["Relation", {}, {"variable": true}, "rel"],
            //       "value": ["string", {}, {"typeAttributes": ["sample"]}, null]
            //   }]
            // ]]}
            //
            // but typeAttributes.sample -  is proposed for remove
            // possible solutions:
            //   - deny proposal of remove `typeAttribute.sample` in specification
            //   - replace `typeAttribute` by `sample` attribute with `[null]` value
            //
            // I prefer 2nd solution but, it is for wide discussion
            //
            // current state: "value" is has not set "sample" attribute
        } 
        else {
            throw ("No property name");
        }

        mson::TypeAttributes attrs = property.valueDefinition.typeDefinition.attributes;
        if (refract::IElement* attributes = MsonTypeAttributesToRefract(attrs)) {
            element->attributes[key::TypeAttributes] = attributes;
        }

        std::string description;
        if (!property.description.empty()) {
            description = property.description;
        }

        for (mson::TypeSections::const_iterator it = property.sections.begin(); it != property.sections.end(); ++it) {
            if (it->klass == mson::TypeSection::BlockDescriptionClass){ 
                const std::string& desc = it->content.description;
                if (!description.empty()) {
                  description.reserve(desc.length() + 1); // +1 for newline
                  description.append("\n");
                }
                description.append(desc);
                continue;
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

    static refract::IElement* MsonPropertyToRefract(const mson::PropertyMember& property)
    {
        mson::BaseTypeName nameType = GetType(property.valueDefinition);
        switch (nameType) {
            case mson::BooleanTypeName:
                return RefractElementFromProperty<refract::BooleanElement>(property);

            case mson::NumberTypeName:
                return RefractElementFromProperty<refract::NumberElement>(property);

            case mson::StringTypeName:
                return RefractElementFromProperty<refract::StringElement>(property);

            case mson::EnumTypeName: {
                refract::MemberElement* element = RefractElementFromProperty<refract::ArrayElement>(property);
                if (element && element->value.second) {
                    element->value.second->element(key::Enum);
                }
                return element;
            }

            case mson::ArrayTypeName:
                return RefractElementFromProperty<refract::ArrayElement>(property);

            case mson::ObjectTypeName:
                return RefractElementFromProperty<refract::ObjectElement>(property);

            case mson::UndefinedTypeName:
                if (ValueHasChildren(property)) {
                    return RefractElementFromProperty<refract::ArrayElement>(property);
                }
                else if (!property.valueDefinition.typeDefinition.typeSpecification.name.symbol.literal.empty() || ValueHasMembers(property)) {
                    return RefractElementFromProperty<refract::ObjectElement>(property);
                }
                return RefractElementFromProperty<refract::StringElement>(property);

            default:
                throw std::runtime_error("Unhandled type of PropertyMember");
        }
    }

    refract::IElement* MsonValueToRefract(const mson::ValueMember& value)
    {
        mson::BaseTypeName typeName = GetType(value.valueDefinition);
        switch (typeName) {
            case mson::BooleanTypeName:
                return RefractElementFromValue<refract::BooleanElement>(value);

            case mson::NumberTypeName:
                return RefractElementFromValue<refract::NumberElement>(value);

            case mson::StringTypeName:
            case mson::UndefinedTypeName:
                return RefractElementFromValue<refract::StringElement>(value);

            // FIXME: I did not find examples of Values w/ complex structures 
            // - confirm they are not exists, or provide some valid example
            case mson::EnumTypeName :
            case mson::ArrayTypeName :
            case mson::ObjectTypeName :
            default:
                throw std::runtime_error("Unhandled type of ValueMember");
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
        ref->renderCompactContent(true);

        refract::MemberElement* href = new refract::MemberElement;
        href->set(key::HRef, refract::IElement::Create(mixin.typeSpecification.name.symbol.literal));
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
                return MsonPropertyToRefract(mse.content.property);

            case mson::Element::ValueClass:
                return MsonValueToRefract(mse.content.value);

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

        e->meta[key::Id] = IElement::Create(ds.name.symbol.literal);

        // FIXME: "title" is temporary commented, until clear refract spec 
        // in few examples for named object is "title" attribute used
        // sometime is not used.
        
        //e->meta[key::Title] = IElement::Create(ds.name.symbol.literal);

        refract::AppendDecorator<T> append = refract::AppendDecorator<T>(e);

        for (mson::TypeSections::const_iterator it = ds.sections.begin(); it != ds.sections.end(); ++it) {

            if (it->klass == mson::TypeSection::BlockDescriptionClass) {
                e->meta[key::Description] = IElement::Create(it->content.description);
                continue;
            }

            append(ExtractTypeSection<T>(*it));
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
