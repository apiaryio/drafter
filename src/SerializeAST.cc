//
//  SerializeAST.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 18/01/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "StringUtility.h"
#include "SerializeAST.h"

#define _WITH_REFRACT_ 1

#include "refract/Element.h"

#include <stdlib.h>

using namespace drafter;

using snowcrash::AssetRole;
using snowcrash::BodyExampleAssetRole;
using snowcrash::BodySchemaAssetRole;

using snowcrash::Element;
using snowcrash::Elements;
using snowcrash::KeyValuePair;
using snowcrash::Metadata;
using snowcrash::Header;
using snowcrash::Reference;
using snowcrash::DataStructure;
using snowcrash::Asset;
using snowcrash::Payload;
using snowcrash::Value;
using snowcrash::Parameter;
using snowcrash::TransactionExample;
using snowcrash::Request;
using snowcrash::Response;
using snowcrash::Action;
using snowcrash::Resource;
using snowcrash::Blueprint;

sos::Object WrapValue(const mson::Value& value)
{
    sos::Object valueObject;

    // Literal
    valueObject.set(SerializeKey::Literal, sos::String(value.literal));

    // Variable
    valueObject.set(SerializeKey::Variable, sos::Boolean(value.variable));

    return valueObject;
}

sos::Object WrapSymbol(const mson::Symbol& symbol)
{
    sos::Object symbolObject;

    // Literal
    symbolObject.set(SerializeKey::Literal, sos::String(symbol.literal));

    // Variable
    symbolObject.set(SerializeKey::Variable, sos::Boolean(symbol.variable));

    return symbolObject;
}

sos::Base WrapTypeName(const mson::TypeName& typeName)
{
    if (typeName.empty()) {
        return sos::Null();
    }

    if (typeName.base != mson::UndefinedTypeName) {

        std::string baseTypeName;

        switch (typeName.base) {

            case mson::BooleanTypeName:
                baseTypeName = "boolean";
                break;

            case mson::StringTypeName:
                baseTypeName = "string";
                break;

            case mson::NumberTypeName:
                baseTypeName = "number";
                break;

            case mson::ArrayTypeName:
                baseTypeName = "array";
                break;

            case mson::EnumTypeName:
                baseTypeName = "enum";
                break;

            case mson::ObjectTypeName:
                baseTypeName = "object";
                break;

            default:
                break;
        }

        return sos::String(baseTypeName);
    }

    return WrapSymbol(typeName.symbol);
}


sos::Object WrapTypeSpecification(const mson::TypeSpecification& typeSpecification)
{
    sos::Object typeSpecificationObject;

    // Name
    typeSpecificationObject.set(SerializeKey::Name, WrapTypeName(typeSpecification.name));

    // Nested Types
    typeSpecificationObject.set(SerializeKey::NestedTypes, 
                                WrapCollection<mson::TypeName>()(typeSpecification.nestedTypes, WrapTypeName));

    return typeSpecificationObject;
}

sos::Array WrapTypeAttributes(const mson::TypeAttributes& typeAttributes)
{
    sos::Array typeAttributesArray;

    if (typeAttributes & mson::RequiredTypeAttribute) {
        typeAttributesArray.push(sos::String("required"));
    }
    else if (typeAttributes & mson::OptionalTypeAttribute) {
        typeAttributesArray.push(sos::String("optional"));
    }
    else if (typeAttributes & mson::DefaultTypeAttribute) {
        typeAttributesArray.push(sos::String("default"));
    }
    else if (typeAttributes & mson::SampleTypeAttribute) {
        typeAttributesArray.push(sos::String("sample"));
    }
    else if (typeAttributes & mson::FixedTypeAttribute) {
        typeAttributesArray.push(sos::String("fixed"));
    }

    return typeAttributesArray;
}

sos::Object WrapTypeDefinition(const mson::TypeDefinition& typeDefinition)
{
    sos::Object typeDefinitionObject;

    // Type Specification
    typeDefinitionObject.set(SerializeKey::TypeSpecification, WrapTypeSpecification(typeDefinition.typeSpecification));

    // Type Attributes
    typeDefinitionObject.set(SerializeKey::Attributes, WrapTypeAttributes(typeDefinition.attributes));

    return typeDefinitionObject;
}

sos::Object WrapValueDefinition(const mson::ValueDefinition& valueDefinition)
{
    sos::Object valueDefinitionObject;

    // Values
    valueDefinitionObject.set(SerializeKey::Values,
                              WrapCollection<mson::Value>()(valueDefinition.values, WrapValue));

    // Type Definition
    valueDefinitionObject.set(SerializeKey::TypeDefinition, WrapTypeDefinition(valueDefinition.typeDefinition));

    return valueDefinitionObject;
}

sos::Object WrapPropertyName(const mson::PropertyName& propertyName)
{
    sos::Object propertyNameObject;

    if (!propertyName.literal.empty()) {
        propertyNameObject.set(SerializeKey::Literal, sos::String(propertyName.literal));
    }
    else if (!propertyName.variable.empty()) {
        propertyNameObject.set(SerializeKey::Variable, WrapValueDefinition(propertyName.variable));
    }

    return propertyNameObject;
}

// Forward declarations
sos::Object WrapTypeSection(const mson::TypeSection& typeSection);

const sos::String TypeSectionClassToString(const mson::TypeSection::Class& klass)
{
    switch (klass) {
        case mson::TypeSection::BlockDescriptionClass:
            return sos::String("blockDescription");

        case mson::TypeSection::MemberTypeClass:
            return sos::String("memberType");

        case mson::TypeSection::SampleClass:
            return sos::String("sample");

        case mson::TypeSection::DefaultClass:
            return sos::String("default");

        default:
            return sos::String();
    }

    return sos::String();
}

sos::String AssetRoleToString(const AssetRole& role)
{
    std::string str;

    switch (role) {
        case BodyExampleAssetRole:
            str = "bodyExample";
            break;

        case BodySchemaAssetRole:
            str = "bodySchema";
            break;

        default:
            break;
    }

    return sos::String(str);
}

sos::String ElementClassToString(const Element::Class& element)
{
    std::string str;

    switch (element) {
        case Element::CategoryElement:
            str = "category";
            break;

        case Element::CopyElement:
            str = "copy";
            break;

        case Element::ResourceElement:
            str = "resource";
            break;

        case Element::DataStructureElement:
            str = "dataStructure";
            break;

        case Element::AssetElement:
            str = "asset";
            break;

        default:
            break;
    }

    return sos::String(str);
}

sos::Object WrapNamedType(const mson::NamedType& namedType)
{
    sos::Object namedTypeObject;

    // Name
    namedTypeObject.set(SerializeKey::Name, WrapTypeName(namedType.name));

    // Type Definition
    namedTypeObject.set(SerializeKey::TypeDefinition, WrapTypeDefinition(namedType.typeDefinition));

    // Type Sections
    namedTypeObject.set(SerializeKey::Sections, 
                        WrapCollection<mson::TypeSection>()(namedType.sections, WrapTypeSection));

    return namedTypeObject;
}

sos::Object WrapKeyValue(const KeyValuePair& keyValue)
{
    sos::Object keyValueObject;

    // Name
    keyValueObject.set(SerializeKey::Name, sos::String(keyValue.first));

    // Value
    keyValueObject.set(SerializeKey::Value, sos::String(keyValue.second));

    return keyValueObject;
}

sos::Object WrapMetadata(const Metadata& metadata)
{
    return WrapKeyValue(metadata);
}

sos::Object WrapHeader(const Header& header)
{
    return WrapKeyValue(header);
}

sos::Object WrapReference(const Reference& reference)
{
    sos::Object referenceObject;

    // Id
    referenceObject.set(SerializeKey::Id, sos::String(reference.id));

    return referenceObject;
}

sos::Object WrapPropertyMember(const mson::PropertyMember& propertyMember)
{
    sos::Object propertyMemberObject;

    // Name
    propertyMemberObject.set(SerializeKey::Name, WrapPropertyName(propertyMember.name));

    // Description
    propertyMemberObject.set(SerializeKey::Description, sos::String(propertyMember.description));

    // Value Definition
    propertyMemberObject.set(SerializeKey::ValueDefinition, WrapValueDefinition(propertyMember.valueDefinition));

    // Type Sections
    propertyMemberObject.set(SerializeKey::Sections,
                             WrapCollection<mson::TypeSection>()(propertyMember.sections, WrapTypeSection));

    return propertyMemberObject;
}

sos::Object WrapValueMember(const mson::ValueMember& valueMember)
{
    sos::Object valueMemberObject;

    // Description
    valueMemberObject.set(SerializeKey::Description, sos::String(valueMember.description));

    // Value Definition
    valueMemberObject.set(SerializeKey::ValueDefinition, WrapValueDefinition(valueMember.valueDefinition));

    // Type Sections
    valueMemberObject.set(SerializeKey::Sections,
                          WrapCollection<mson::TypeSection>()(valueMember.sections, WrapTypeSection));

    return valueMemberObject;
}

sos::Object WrapMixin(const mson::Mixin& mixin)
{
    return WrapTypeDefinition(mixin);
}

sos::Object WrapMSONElement(const mson::Element& element)
{
    sos::Object elementObject;
    std::string klass;

    switch (element.klass) {

        case mson::Element::PropertyClass:
        {
            klass = "property";
            elementObject.set(SerializeKey::Content, WrapPropertyMember(element.content.property));
            break;
        }

        case mson::Element::ValueClass:
        {
            klass = "value";
            elementObject.set(SerializeKey::Content, WrapValueMember(element.content.value));
            break;
        }

        case mson::Element::MixinClass:
        {
            klass = "mixin";
            elementObject.set(SerializeKey::Content, WrapMixin(element.content.mixin));
            break;
        }

        case mson::Element::OneOfClass:
        {
            klass = "oneOf";
            elementObject.set(SerializeKey::Content, 
                              WrapCollection<mson::Element>()(element.content.oneOf(), WrapMSONElement));
            break;
        }

        case mson::Element::GroupClass:
        {
            klass = "group";
            elementObject.set(SerializeKey::Content, 
                              WrapCollection<mson::Element>()(element.content.elements(), WrapMSONElement));
            break;
        }

        default:
            break;
    }

    elementObject.set(SerializeKey::Class, sos::String(klass));

    return elementObject;
}

sos::Object WrapTypeSection(const mson::TypeSection& section)
{
    sos::Object object;

    // Class
    object.set(SerializeKey::Class, TypeSectionClassToString(section.klass));

    // Content
    if (!section.content.description.empty()) {
        object.set(SerializeKey::Content, sos::String(section.content.description));
    }
    else if (!section.content.value.empty()) {
        object.set(SerializeKey::Content, sos::String(section.content.value));
    }
    else if (!section.content.elements().empty()) {
        object.set(SerializeKey::Content, 
                   WrapCollection<mson::Element>()(section.content.elements(), WrapMSONElement));
    }

    return object;
}

static refract::ArrayElement* MsAttributesToRefract(const mson::TypeAttributes& ta)
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
    
    if(attr->value.empty()) {
        delete attr;
        attr = NULL;
    }

    return attr;
}

template <int N>
struct IntToType {
    enum { value = N };
};

template <typename T> T LiteralTo(const mson::Literal& literal);

template <> bool LiteralTo<bool>(const mson::Literal& literal) {
    return literal == "true";
}

template <> double LiteralTo<double>(const mson::Literal& literal) {
    return atof(literal.c_str());
}

template <> std::string LiteralTo<std::string>(const mson::Literal& literal) {
    return literal;
}

static refract::IElement* SimplifyRefractContainer(const std::vector<refract::IElement*>& container) {
    if(container.empty()) {
        return NULL;
    }

    if(container.size() == 1) {
        return container[0];
    }

    refract::ArrayElement* array = new refract::ArrayElement;
    for(std::vector<refract::IElement*>::const_iterator it = container.begin() ; it != container.end() ; ++ it ) {
        array->push_back(*it);
    }
    return array;
}

template <typename T> 
refract::IElement* RefractElementFromProperty(const mson::PropertyMember& property) {
    using namespace refract;
    typedef T ElementType;
    if(property.valueDefinition.values.size() > 1) {
        throw std::runtime_error("just one value supported");
    }

    ElementType* element = new ElementType;

    if(property.valueDefinition.values.size() == 1) {
        element->set(LiteralTo<typename ElementType::value_type>(property.valueDefinition.values[0].literal));
    }

    element->meta["name"] = IElement::Create(property.name.literal);

    if(IElement* attrs = MsAttributesToRefract(property.valueDefinition.typeDefinition.attributes)) {
        element->meta["typeAttributes"] = attrs;
    }

    if(!property.description.empty()) {
        element->meta["description"] = IElement::Create(property.description);
    }

    if (!property.sections.empty()) {
        typedef std::vector<refract::IElement*> Elements;

        std::vector<refract::IElement*> defaults;
        std::vector<refract::IElement*> samples;

        for(mson::TypeSections::const_iterator it = property.sections.begin() ; 
            it != property.sections.end() ; 
            ++it) {

            IElement* e = IElement::Create(LiteralTo<typename ElementType::value_type>(it->content.value));
            if (it->klass == mson::TypeSection::SampleClass) {
                samples.push_back(e);
            } else if(it->klass == mson::TypeSection::DefaultClass) {
                defaults.push_back(e);
            } else {
                throw std::runtime_error("Unexpected section for property");
            }
        }

        if(IElement* e = SimplifyRefractContainer(samples)) {
            element->meta["sample"] = e;
        }

        if(IElement* e = SimplifyRefractContainer(defaults)) {
            element->meta["default"] = e;
        }

    }

    return element;
}

template<int N> 
refract::IElement* RefractElementFromProperty(const mson::PropertyMember& property, IntToType<N>);

#define REFRACT_FROM_PROPERTY_IMPL(MSON_BASE_TYPE,REFRACT_TYPE) \
template<> refract::IElement* RefractElementFromProperty<MSON_BASE_TYPE>\
(const mson::PropertyMember& property, IntToType<MSON_BASE_TYPE>) {\
    return RefractElementFromProperty<REFRACT_TYPE>(property);\
}

REFRACT_FROM_PROPERTY_IMPL(mson::BooleanTypeName, refract::BooleanElement)
REFRACT_FROM_PROPERTY_IMPL(mson::NumberTypeName,  refract::NumberElement)
REFRACT_FROM_PROPERTY_IMPL(mson::StringTypeName,  refract::StringElement)

#undef REFRACT_FROM_PROPERTY_IMPL

static refract::IElement* MsonPropertyToRefract(const mson::PropertyMember& property) {
    refract::IElement *element = NULL;
    switch (property.valueDefinition.typeDefinition.typeSpecification.name.base) {
        case mson::BooleanTypeName :
            element = RefractElementFromProperty(property, IntToType<mson::BooleanTypeName>());
            break;
        case mson::NumberTypeName : 
            element = RefractElementFromProperty(property, IntToType<mson::NumberTypeName>());
            break;
        case mson::StringTypeName :
            element = RefractElementFromProperty(property, IntToType<mson::StringTypeName>());
        break;
        default:
            throw std::runtime_error("NI: complex type of Property member");

    }
    return element;
}

static refract::IElement* MsElementToRefract(const mson::Element& mse) {
    using namespace refract;

    std::string klass;

    switch (mse.klass) {
        case mson::Element::PropertyClass:
        {
            //elementObject.set(SerializeKey::Content, WrapPropertyMember(element.content.property));
            return MsonPropertyToRefract(mse.content.property);
            break;
        }

        case mson::Element::ValueClass:
        {
            klass = "value";
            //elementObject.set(SerializeKey::Content, WrapValueMember(element.content.value));
            break;
        }

        case mson::Element::MixinClass:
        {
            klass = "mixin";
            //elementObject.set(SerializeKey::Content, WrapMixin(element.content.mixin));
            break;
        }

        case mson::Element::OneOfClass:
        {
            klass = "oneOf";
            //elementObject.set(SerializeKey::Content, 
            //                  WrapCollection<mson::Element>()(element.content.oneOf(), WrapMSONElement));
            break;
        }

        case mson::Element::GroupClass:
        {
            klass = "group";
            //elementObject.set(SerializeKey::Content, 
            //                  WrapCollection<mson::Element>()(element.content.elements(), WrapMSONElement));
            break;
        }

        default:
            break;
    }

    throw std::runtime_error("NI unhandled element");
}


refract::IElement* ToRefract(const DataStructure& ds) {
    using namespace refract;

    ObjectElement* e = new ObjectElement;

    if (!ds.typeDefinition.typeSpecification.name.symbol.literal.empty()) {
        e->element(ds.typeDefinition.typeSpecification.name.symbol.literal);
    }
    
    e->meta["id"] = IElement::Create(ds.name.symbol.literal);
    e->meta["title"] = IElement::Create(ds.name.symbol.literal);

    for ( mson::TypeSections::const_iterator it = ds.sections.begin() ; it != ds.sections.end() ; ++it ) {

        if(it->klass == mson::TypeSection::BlockDescriptionClass) {
            e->meta["description"] = IElement::Create(it->content.description);
            continue;
        }

        for( mson::Elements::const_iterator eit = (*it).content.elements().begin() ; eit != (*it).content.elements().end() ; ++eit ) {
            e->push_back(MsElementToRefract(*eit));
        }

    }

    return e;
}


sos::Object WrapDataStructure(const DataStructure& dataStructure)
{
#if _WITH_REFRACT_
    using namespace refract;
    IElement* element = ToRefract(dataStructure);
    SerializeVisitor serializer;
    serializer.visit(*element);
    delete element;

    return serializer.get();
#else

    sos::Object dataStructureObject;

    // Element
    dataStructureObject.set(SerializeKey::Element, ElementClassToString(Element::DataStructureElement));

    // Name
    dataStructureObject.set(SerializeKey::Name, WrapTypeName(dataStructure.name));

    // Type Definition
    dataStructureObject.set(SerializeKey::TypeDefinition, WrapTypeDefinition(dataStructure.typeDefinition));

    // Type Sections
    dataStructureObject.set(SerializeKey::Sections,
                            WrapCollection<mson::TypeSection>()(dataStructure.sections, WrapTypeSection));
                            

    return dataStructureObject;
#endif    
}

sos::Object WrapAsset(const Asset& asset, const AssetRole& role)
{
    sos::Object assetObject;

    // Element
    assetObject.set(SerializeKey::Element, ElementClassToString(Element::AssetElement));

    // Attributes
    sos::Object attributes;

    /// Role
    attributes.set(SerializeKey::Role, AssetRoleToString(role));

    assetObject.set(SerializeKey::Attributes, attributes);

    // Content
    assetObject.set(SerializeKey::Content, sos::String(asset));

    return assetObject;
}

sos::Object WrapPayload(const Payload& payload)
{
    sos::Object payloadObject;

    // Reference
    if (!payload.reference.id.empty()) {
        payloadObject.set(SerializeKey::Reference, WrapReference(payload.reference));
    }

    // Name
    payloadObject.set(SerializeKey::Name, sos::String(payload.name));

    // Description
    payloadObject.set(SerializeKey::Description, sos::String(payload.description));

    // Headers
    payloadObject.set(SerializeKey::Headers,
                      WrapCollection<Header>()(payload.headers, WrapHeader));

    // Body
    payloadObject.set(SerializeKey::Body, sos::String(payload.body));

    // Schema
    payloadObject.set(SerializeKey::Schema, sos::String(payload.schema));

    // Content
    sos::Array content;

    /// Attributes
    if (!payload.attributes.empty()) {
        content.push(WrapDataStructure(payload.attributes));
    }

    /// Asset 'bodyExample'
    if (!payload.body.empty()) {
        content.push(WrapAsset(payload.body, BodyExampleAssetRole));
    }

    /// Asset 'bodySchema'
    if (!payload.schema.empty()) {
        content.push(WrapAsset(payload.schema, BodySchemaAssetRole));
    }

    payloadObject.set(SerializeKey::Content, content);

    return payloadObject;
}

sos::Object WrapParameterValue(const Value& value)
{
    sos::Object object;
    object.set(SerializeKey::Value, sos::String(value.c_str()));

    return object;
}

sos::Object WrapParameter(const Parameter& parameter)
{
    sos::Object parameterObject;

    // Name
    parameterObject.set(SerializeKey::Name, sos::String(parameter.name));

    // Description
    parameterObject.set(SerializeKey::Description, sos::String(parameter.description));

    // Type
    parameterObject.set(SerializeKey::Type, sos::String(parameter.type));

    // Use
    parameterObject.set(SerializeKey::Required, sos::Boolean(parameter.use != snowcrash::OptionalParameterUse));

    // Default Value
    parameterObject.set(SerializeKey::Default, sos::String(parameter.defaultValue));

    // Example Value
    parameterObject.set(SerializeKey::Example, sos::String(parameter.exampleValue));

    // Values
    parameterObject.set(SerializeKey::Values, 
                        WrapCollection<Value>()(parameter.values, WrapParameterValue));

    return parameterObject;
}

sos::Object WrapTransactionExample(const TransactionExample& example)
{
    sos::Object exampleObject;

    // Name
    exampleObject.set(SerializeKey::Name, sos::String(example.name));

    // Description
    exampleObject.set(SerializeKey::Description, sos::String(example.description));

    // Requests
    exampleObject.set(SerializeKey::Requests,
                      WrapCollection<Request>()(example.requests, WrapPayload));

    // Responses
    exampleObject.set(SerializeKey::Responses,
                      WrapCollection<Response>()(example.responses, WrapPayload));

    return exampleObject;
}

sos::Object WrapAction(const Action& action)
{
    sos::Object actionObject;

    // Name
    actionObject.set(SerializeKey::Name, sos::String(action.name));

    // Description
    actionObject.set(SerializeKey::Description, sos::String(action.description));

    // HTTP Method
    actionObject.set(SerializeKey::Method, sos::String(action.method));

    // Parameters
    actionObject.set(SerializeKey::Parameters,
                     WrapCollection<Parameter>()(action.parameters, WrapParameter));

    // Attributes
    sos::Object attributes;

    /// Relation
    attributes.set(SerializeKey::Relation, sos::String(action.relation.str));

    /// URI Template
    attributes.set(SerializeKey::URITemplate, sos::String(action.uriTemplate));

    actionObject.set(SerializeKey::Attributes, attributes);

    // Content
    sos::Array content;

    if (!action.attributes.empty()) {
        content.push(WrapDataStructure(action.attributes));
    }

    actionObject.set(SerializeKey::Content, content);

    // Transaction Examples
    actionObject.set(SerializeKey::Examples,
                     WrapCollection<TransactionExample>()(action.examples, WrapTransactionExample));

    return actionObject;
}

sos::Object WrapResource(const Resource& resource)
{
    sos::Object resourceObject;

    // Element
    resourceObject.set(SerializeKey::Element, ElementClassToString(Element::ResourceElement));

    // Name
    resourceObject.set(SerializeKey::Name, sos::String(resource.name));

    // Description
    resourceObject.set(SerializeKey::Description, sos::String(resource.description));

    // URI Template
    resourceObject.set(SerializeKey::URITemplate, sos::String(resource.uriTemplate));

    // Model
    sos::Object model = (resource.model.name.empty() ? sos::Object() : WrapPayload(resource.model));
    resourceObject.set(SerializeKey::Model, model);

    // Parameters
    resourceObject.set(SerializeKey::Parameters,
                       WrapCollection<Parameter>()(resource.parameters, WrapParameter));

    // Actions
    resourceObject.set(SerializeKey::Actions,
                       WrapCollection<Action>()(resource.actions, WrapAction));

    // Content
    sos::Array content;

    if (!resource.attributes.empty()) {
        content.push(WrapDataStructure(resource.attributes));
    }

    resourceObject.set(SerializeKey::Content, content);

    return resourceObject;
}

sos::Object WrapResourceGroup(const Element& resourceGroup)
{
    sos::Object resourceGroupObject;

    // Name
    resourceGroupObject.set(SerializeKey::Name, sos::String(resourceGroup.attributes.name));

    // Description && Resources
    std::string description;
    sos::Array resources;

    for (Elements::const_iterator it = resourceGroup.content.elements().begin();
         it != resourceGroup.content.elements().end();
         ++it) {

        if (it->element == Element::ResourceElement) {
            resources.push(WrapResource(it->content.resource));
        }
        else if (it->element == Element::CopyElement) {

            if (!description.empty()) {
                snowcrash::TwoNewLines(description);
            }

            description += it->content.copy;
        }
    }

    resourceGroupObject.set(SerializeKey::Description, sos::String(description));
    resourceGroupObject.set(SerializeKey::Resources, resources);

    return resourceGroupObject;
}

sos::Object WrapElement(const Element& element)
{
    sos::Object elementObject;

    elementObject.set(SerializeKey::Element, ElementClassToString(element.element));

    if (!element.attributes.name.empty()) {

        sos::Object attributes;

        attributes.set(SerializeKey::Name, sos::String(element.attributes.name));
        elementObject.set(SerializeKey::Attributes, attributes);
    }

    switch (element.element) {
        case Element::CopyElement:
        {
            elementObject.set(SerializeKey::Content, sos::String(element.content.copy));
            break;
        }

        case Element::CategoryElement:
        {
            elementObject.set(SerializeKey::Content,
                              WrapCollection<Element>()(element.content.elements(), WrapElement));
            break;
        }

        case Element::DataStructureElement:
        {
            return WrapDataStructure(element.content.dataStructure);
        }

        case Element::ResourceElement:
        {
            return WrapResource(element.content.resource);
        }

        default:
            break;
    }

    return elementObject;
}

bool IsElementResourceGroup(const Element& element)
{
    return element.element == Element::CategoryElement && element.category == Element::ResourceGroupCategory;
}

sos::Object drafter::WrapBlueprint(const Blueprint& blueprint)
{
    sos::Object blueprintObject;

    // Version
    blueprintObject.set(SerializeKey::Version, sos::String(AST_SERIALIZATION_VERSION));

    // Metadata
    blueprintObject.set(SerializeKey::Metadata, 
                        WrapCollection<Metadata>()(blueprint.metadata, WrapKeyValue));

    // Name
    blueprintObject.set(SerializeKey::Name, sos::String(blueprint.name));

    // Description
    blueprintObject.set(SerializeKey::Description, sos::String(blueprint.description));

    // Element
    blueprintObject.set(SerializeKey::Element, ElementClassToString(blueprint.element));

    // Resource Groups
    blueprintObject.set(SerializeKey::ResourceGroups, 
                        WrapCollection<Element>()(blueprint.content.elements(), WrapResourceGroup, IsElementResourceGroup));

    // Content
    blueprintObject.set(SerializeKey::Content,
                        WrapCollection<Element>()(blueprint.content.elements(), WrapElement));

    return blueprintObject;
}
