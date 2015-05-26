//
//  RefractAST.cc
//  drafter
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "RefractAST.h"
#include "refract/Element.h"

namespace drafter
{

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

    static refract::ArrayElement* MsonAttributesToRefract(const mson::TypeAttributes& ta)
    {
        refract::ArrayElement* attr = new refract::ArrayElement;

        if (ta & mson::RequiredTypeAttribute) {
            attr->push_back(refract::IElement::Create("required"));
        }
        if (ta & mson::OptionalTypeAttribute) {
            attr->push_back(refract::IElement::Create("optional"));
        }
        if (ta & mson::DefaultTypeAttribute) {
            attr->push_back(refract::IElement::Create("default"));
        }
        if (ta & mson::SampleTypeAttribute) {
            attr->push_back(refract::IElement::Create("sample"));
        }
        if (ta & mson::FixedTypeAttribute) {
            attr->push_back(refract::IElement::Create("fixed"));
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
        return literal == "true";
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

    static refract::IElement* SimplifyRefractContainer(const std::vector<refract::IElement*>& container)
    {
        if (container.empty()) {
            return NULL;
        }

        if (container.size() == 1) {
            return container[0];
        }

        refract::ArrayElement* array = new refract::ArrayElement;
        for (std::vector<refract::IElement*>::const_iterator it = container.begin(); it != container.end(); ++it) {
            array->push_back(*it);
        }
        return array;
    }

    struct RefractElementFactory
    {
        virtual ~RefractElementFactory() {}
        virtual refract::IElement* Create(const std::string& literal) = 0;
    };

    template <typename E>
    struct RefractElementFactoryImpl : RefractElementFactory
    {
        virtual refract::IElement* Create(const std::string& literal)
        {
            E* element = new E;
            element->set(LiteralTo<typename E::ValueType>(literal));
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


    std::vector<refract::IElement*> MsonElementsToRefract(const mson::Elements& elements) {
        std::vector<refract::IElement*> result;
        for (mson::Elements::const_iterator it = elements.begin(); it != elements.end(); ++it) {
            result.push_back(MsonElementToRefract(*it));
        }
        return result;
    }

    template <typename T>
    struct ExtractTypeSection<T, std::vector<refract::IElement*> >
    { // this will handle Array && Object because of underlaying type
        const mson::TypeSection& ts;
        typedef std::vector<refract::IElement*> V;

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

        ExtractValueMember(const mson::ValueMember& v) : vm(v) {}

        operator T*()
        {
            ElementType* element = new ElementType;
            mson::TypeAttributes attrs = vm.valueDefinition.typeDefinition.attributes;

            if (vm.valueDefinition.values.size() > 1) {
                throw std::logic_error("For primitive types is just one value supported");
            } 

            if(!vm.valueDefinition.values.empty()) {
                const mson::Value& value = *vm.valueDefinition.values.begin();
                element->set(LiteralTo<V>(value.literal));
                if (value.variable) {
                    attrs |= mson::SampleTypeAttribute;
                }
            }

            if (refract::IElement* attributes = MsonAttributesToRefract(attrs)) {
                element->attributes["typeAttributes"] = attributes;
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
    struct ExtractValueMember<T, std::vector<refract::IElement*> >
    { // this will handle Array && Object because of underlaying type
        const mson::ValueMember& vm;
        typedef std::vector<refract::IElement*> V;
        typedef T ElementType;

        ExtractValueMember(const mson::ValueMember& v) : vm(v) {}

        operator T*()
        {
            ElementType* element = new ElementType;

            if (!vm.valueDefinition.values.empty()) {
                mson::BaseTypeName type = SelectNestedTypeSpecification(vm.valueDefinition.typeDefinition.typeSpecification.nestedTypes);

                RefractElementFactory& elementFactory = FactoryFromType(type);
                const mson::Values& values = vm.valueDefinition.values;

                V result;
                for (mson::Values::const_iterator it = values.begin(); it != values.end(); ++it) {
                    refract::IElement* element = elementFactory.Create(it->literal);
                    if(it->variable) {
                        element->attributes["typeAttributes"] = MsonAttributesToRefract(mson::SampleTypeAttribute);
                    }

                    result.push_back(element);
                }
                element->set(result);

            }

            mson::TypeAttributes attrs = vm.valueDefinition.typeDefinition.attributes;
            if (refract::IElement* attributes = MsonAttributesToRefract(attrs)) {
                element->attributes["typeAttributes"] = attributes;
            }

            return element;
        }
    };

    template <typename T>
    refract::IElement* RefractElementFromValue(const mson::ValueMember& value)
    {
        using namespace refract;
        typedef T ElementType;
        ElementType* element = ExtractValueMember<ElementType>(value);
        std::string description;

        if (!value.description.empty()) {
            description = value.description;
        }

        SetElementType(value.valueDefinition.typeDefinition, element);

        if (!value.sections.empty()) {
            typedef std::vector<refract::IElement*> Elements;

            // FIXME: for Array/Enum - extract *type of element* from
            // value.valueDefinition.typeDefinition.typeSpecification.nestedTypes[];
            // and inject into ExtractTypeSection
            // reason - value defined as list eg.
            // - value: 1,2,3,4,5 (array[number, string])
            // does not hold type resp. is defaultly set as mson::UndefinedTypeName
            //
            // fallback for now - present all as refract::StringElement

            std::vector<refract::IElement*> defaults;
            std::vector<refract::IElement*> samples;

            for (mson::TypeSections::const_iterator it = value.sections.begin(); it != value.sections.end(); ++it) {

                if (it->klass == mson::TypeSection::MemberTypeClass) {
                    if (!element->empty()) {
                        throw std::logic_error("Element content was already set, you cannot fill it from 'memberType'");
                    }
                    element->set(ExtractTypeSection<T>(*it));
                    continue;
                }

                if (it->klass == mson::TypeSection::BlockDescriptionClass){ 
                    const std::string& desc = it->content.description;
                    description.reserve(desc.length() + 1); // +1 for newline
                    description.append("\n");
                    description.append(desc);
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

            if (IElement* e = SimplifyRefractContainer(samples)) {
                element->attributes["sample"] = e;
            }

            if (IElement* e = SimplifyRefractContainer(defaults)) {
                element->attributes["default"] = e;
            }
        }

        if(!description.empty()) {
            element->meta["description"] = IElement::Create(description);
        }

        return element;
    }

    template <typename T>
    refract::IElement* RefractElementFromProperty(const mson::PropertyMember& property)
    {
        refract::IElement* element = RefractElementFromValue<T>(property);
        if (!property.name.literal.empty()) {
            element->meta["name"] = refract::IElement::Create(property.name.literal);
        }
        else if (!property.name.variable.values.empty()) {
            if (property.name.variable.values.size() > 1) {
                // FIXME: is there example for multiple variables?
                throw std::logic_error("Multiple variables in property definition");
            }
            element->meta["name"] = refract::IElement::Create(property.name.variable.values[0].literal);
            element->attributes["variable"] = refract::IElement::Create(true);
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
                refract::IElement* element = RefractElementFromProperty<refract::ArrayElement>(property);
                if(element) {
                    element->element("enum");
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
                else if (ValueHasMembers(property)) {
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

            // FIXME: not found examples for complex structures 
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
        select->element("select");
        for (mson::Elements::const_iterator it = oneOf.begin(); it != oneOf.end(); ++it) {
            refract::ArrayElement* option = new refract::ArrayElement;
            option->element("option");
            // we can not use MsonElementToRefract() for groups, 
            // "option" element handles directly all elements in group
            if(it->klass == mson::Element::GroupClass) { 
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
        ref->element("ref");
        ref->renderCompactContent(true);

        refract::StringElement* path = new refract::StringElement;
        path->set("content");
        path->meta["name"] = refract::IElement::Create("path");
        ref->push_back(path);

        refract::StringElement* href = new refract::StringElement;
        href->set(mixin.typeSpecification.name.symbol.literal);
        href->meta["name"] = refract::IElement::Create("href");
        ref->push_back(href);

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

    template <typename T, typename V = typename T::ValueType>
    struct AppendDecorator {
        typedef T ElementType;
        typedef V ValueType;
        ElementType*& element;
        AppendDecorator(ElementType*& e) : element(e) 
        {
        }

        void append(const V& value) 
        {
            //throw std::logic_error("Can not append to primitive type");
            // FIXME: snowcrash warn about "Primitive type can not have member"
            // but in real it create "empty" member
            //
            // solution for now: silently ignore
        }
    };

    template <typename T>
    struct AppendDecorator<T, std::string> {
        typedef T ElementType;
        typedef typename T::ValueType ValueType;
        ElementType*& element;

        AppendDecorator(ElementType*& e) : element(e) 
        {
        }

        void append(const std::string& value) {
            if(!value.empty()) {
                element->value.append(value);
            }
        }
    };

    template <typename T>
    struct AppendDecorator<T, std::vector<refract::IElement*> > {
        typedef T ElementType;
        typedef typename T::ValueType ValueType;
        ElementType*& element;

        AppendDecorator(ElementType*& e) : element(e) 
        {
        }

        void append(const ValueType& value)
        {
            for(std::vector<refract::IElement*>::const_iterator it = value.begin() ; it != value.end() ; ++it) {
                element->value.push_back(*it);
            }
            element->hasContent = true;
        }
    };

    template<typename T>
    refract::IElement* RefractElementFromDataStructure(const snowcrash::DataStructure& ds)
    {
        using namespace refract;
        typedef T ElementType;

        ElementType* e = new ElementType;
        SetElementType(ds.typeDefinition, e);

        e->meta["id"] = IElement::Create(ds.name.symbol.literal);
        e->meta["title"] = IElement::Create(ds.name.symbol.literal);

        AppendDecorator<T> ae = AppendDecorator<T>(e);

        for (mson::TypeSections::const_iterator it = ds.sections.begin(); it != ds.sections.end(); ++it) {

            if (it->klass == mson::TypeSection::BlockDescriptionClass) {
                e->meta["description"] = IElement::Create(it->content.description);
                continue;
            }

            ae.append(ExtractTypeSection<T>(*it));
        }

        return e;
    }

    sos::Object DataStructureToRefract(const snowcrash::DataStructure& dataStructure)
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
                if(element) {
                    element->element("enum");
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

        if (!element) {
            return sos::Object();
        }


        SerializeVisitor serializer;
        serializer.visit(*element);
        delete element;

        return serializer.get();
    }
}
