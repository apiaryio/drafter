//
//  SerializeAST.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 18/01/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "StringUtility.h"
#include "SerializeAST.h"

#include <stdlib.h>

#include "SourceAnnotation.h"
#include "RefractDataStructure.h"
#include "RefractAPI.h"
#include "Render.h"
#include "SectionProcessor.h"

#include "NamedTypesRegistry.h"

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

static bool ExpandMSON = false;

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
    else if (typeAttributes & mson::NullableTypeAttribute) {
        typeAttributesArray.push(sos::String("nullable"));
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

sos::Object WrapDataStructure(const DataStructure& dataStructure)
{
    sos::Object dataStructureObject;

#if _WITH_REFRACT_
    refract::IElement *element = DataStructureToRefract(MakeNodeInfoWithoutSourceMap(dataStructure), ExpandMSON);
    dataStructureObject = SerializeRefract(element);

    if (element) {
        delete element;
    }

    return dataStructureObject;
#else

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

sos::Object WrapPayload(const Payload& payload, const Action* action)
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

    snowcrash::Asset payloadBody = renderPayloadBody(MakeNodeInfoWithoutSourceMap(payload), action ? MakeNodeInfoWithoutSourceMap(*action) : NodeInfo<Action>(), GetNamedTypesRegistry()).first;
    snowcrash::Asset payloadSchema = renderPayloadSchema(MakeNodeInfoWithoutSourceMap(payload), action ? MakeNodeInfoWithoutSourceMap(*action) : NodeInfo<Action>(), GetNamedTypesRegistry()).first;

    // Body
    payloadObject.set(SerializeKey::Body, sos::String(payloadBody));

    // Schema
    payloadObject.set(SerializeKey::Schema, sos::String(payloadSchema));

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

sos::Object WrapTransactionExample(const TransactionExample& example, const Action& action)
{
    sos::Object exampleObject;

    // Name
    exampleObject.set(SerializeKey::Name, sos::String(example.name));

    // Description
    exampleObject.set(SerializeKey::Description, sos::String(example.description));

    // Requests
    exampleObject.set(SerializeKey::Requests,
                      WrapCollection<Request>()(example.requests, WrapPayload, &action));

    // Responses
    exampleObject.set(SerializeKey::Responses,
                      WrapCollection<Response>()(example.responses, WrapPayload, (Action *) NULL));

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
                     WrapCollection<TransactionExample>()(action.examples, WrapTransactionExample, action));

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
    sos::Object model = (resource.model.name.empty() ? sos::Object() : WrapPayload(resource.model, NULL));
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

sos::Object WrapBlueprintAST(const Blueprint& blueprint)
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
                        WrapCollection<Element>()(blueprint.content.elements(), WrapResourceGroup, IsElementResourceGroup, false));

    // Content
    blueprintObject.set(SerializeKey::Content,
                        WrapCollection<Element>()(blueprint.content.elements(), WrapElement));

    return blueprintObject;
}

sos::Object drafter::WrapBlueprint(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint, const bool expandMSON)
{
    sos::Object blueprintObject;
    snowcrash::Error error;

    try {
        ExpandMSON = expandMSON;
        bool hasSourceMap = (blueprint.node.content.elements().size() == blueprint.sourceMap.content.elements().collection.size());

        RegisterNamedTypes(MakeNodeInfo(blueprint.node.content.elements(), blueprint.sourceMap.content.elements(), hasSourceMap));
        blueprintObject = WrapBlueprintAST(blueprint.node);
    }
    catch (std::exception& e) {
        error = snowcrash::Error(e.what(), snowcrash::MSONError);
    }
    catch (snowcrash::Error& e) {
        error = e;
    }

    GetNamedTypesRegistry().clearAll(true);

    if (error.code != snowcrash::Error::OK) {
        throw error;
    }

    return blueprintObject;
}
