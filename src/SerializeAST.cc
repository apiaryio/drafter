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
#include "ConversionContext.h"

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

sos::Object WrapDataStructure(const DataStructure& dataStructure, ConversionContext& context)
{
    sos::Object dataStructureObject;

    refract::IElement *element = DataStructureToRefract(MakeNodeInfoWithoutSourceMap(dataStructure), context);
    dataStructureObject = SerializeRefract(element, context);

    if (element) {
        delete element;
    }

    return dataStructureObject;
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

sos::Object WrapPayload(const Payload& payload, const Action* action, ConversionContext& context)
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

    snowcrash::Asset payloadBody = renderPayloadBody(MakeNodeInfoWithoutSourceMap(payload), 
                                                     action ? MakeNodeInfoWithoutSourceMap(*action) : NodeInfo<Action>(),
                                                     context).first;

    snowcrash::Asset payloadSchema = renderPayloadSchema(MakeNodeInfoWithoutSourceMap(payload),
                                                         action ? MakeNodeInfoWithoutSourceMap(*action) : NodeInfo<Action>(),
                                                         context).first;

    // Body
    payloadObject.set(SerializeKey::Body, sos::String(payloadBody));

    // Schema
    payloadObject.set(SerializeKey::Schema, sos::String(payloadSchema));

    // Content
    sos::Array content;

    /// Attributes
    if (!payload.attributes.empty()) {
        content.push(WrapDataStructure(payload.attributes, context));
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

sos::Object WrapTransactionExample(const TransactionExample& example, const Action& action, ConversionContext& context)
{
    sos::Object exampleObject;

    // Name
    exampleObject.set(SerializeKey::Name, sos::String(example.name));

    // Description
    exampleObject.set(SerializeKey::Description, sos::String(example.description));

    // Requests
    exampleObject.set(SerializeKey::Requests,
                      WrapCollection<Request>()(example.requests, WrapPayload, &action, std::ref(context)));

    // Responses
    exampleObject.set(SerializeKey::Responses,
                      WrapCollection<Response>()(example.responses, WrapPayload, (Action *) NULL, std::ref(context)));

    return exampleObject;
}

sos::Object WrapAction(const Action& action, ConversionContext& context)
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
        content.push(WrapDataStructure(action.attributes, context));
    }

    actionObject.set(SerializeKey::Content, content);

    // Transaction Examples
    actionObject.set(SerializeKey::Examples,
                     WrapCollection<TransactionExample>()(action.examples, WrapTransactionExample, action, std::ref(context)));

    return actionObject;
}

sos::Object WrapResource(const Resource& resource, ConversionContext& context)
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
    sos::Object model = (resource.model.name.empty() ? sos::Object() : WrapPayload(resource.model, NULL, context));
    resourceObject.set(SerializeKey::Model, model);

    // Parameters
    resourceObject.set(SerializeKey::Parameters,
                       WrapCollection<Parameter>()(resource.parameters, WrapParameter));

    // Actions
    resourceObject.set(SerializeKey::Actions,
                       WrapCollection<Action>()(resource.actions, WrapAction, std::ref(context)));

    // Content
    sos::Array content;

    if (!resource.attributes.empty()) {
        content.push(WrapDataStructure(resource.attributes, context));
    }

    resourceObject.set(SerializeKey::Content, content);

    return resourceObject;
}

sos::Object WrapResourceGroup(const Element& resourceGroup, ConversionContext& context)
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
            resources.push(WrapResource(it->content.resource, context));
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

sos::Object WrapElement(const Element& element, ConversionContext& context)
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
                              WrapCollection<Element>()(element.content.elements(), WrapElement, std::ref(context)));
            break;
        }

        case Element::DataStructureElement:
        {
            return WrapDataStructure(element.content.dataStructure, context);
        }

        case Element::ResourceElement:
        {
            return WrapResource(element.content.resource, context);
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

sos::Object WrapBlueprintAST(const Blueprint& blueprint, ConversionContext& context)
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
                        WrapCollectionIf<Element>()(blueprint.content.elements(), WrapResourceGroup, IsElementResourceGroup, context));

    // Content
    blueprintObject.set(SerializeKey::Content,
                        WrapCollection<Element>()(blueprint.content.elements(), WrapElement, std::ref(context)));

    return blueprintObject;
}

sos::Object drafter::WrapBlueprint(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint, ConversionContext& context)
{
    sos::Object blueprintObject;
    snowcrash::Error error;

    if (blueprint.report.error.code != snowcrash::Error::OK) {
        throw blueprint.report.error;
    }

    try {
        RegisterNamedTypes(MakeNodeInfo(blueprint.node.content.elements(), blueprint.sourceMap.content.elements()), context);
        blueprintObject = WrapBlueprintAST(blueprint.node, context);
    }
    catch (std::exception& e) {
        error = snowcrash::Error(e.what(), snowcrash::MSONError);
    }
    catch (snowcrash::Error& e) {
        error = e;
    }

    context.GetNamedTypesRegistry().clearAll(true);

    if (error.code != snowcrash::Error::OK) {
        throw error;
    }

    return blueprintObject;
}
