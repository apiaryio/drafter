//
//  SerializeSourcemap.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 18/01/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "SerializeSourcemap.h"

using namespace drafter;

using snowcrash::SourceMapBase;
using snowcrash::SourceMap;
using snowcrash::Collection;

using snowcrash::DataStructure;
using snowcrash::Asset;
using snowcrash::Payload;
using snowcrash::Header;
using snowcrash::Parameter;
using snowcrash::Value;
using snowcrash::TransactionExample;
using snowcrash::Request;
using snowcrash::Response;
using snowcrash::Action;
using snowcrash::Resource;
using snowcrash::Element;
using snowcrash::Description;
using snowcrash::Blueprint;
using snowcrash::Metadata;

sos::Array WrapSourcemap(const SourceMapBase& value)
{
    sos::Array sourceMap;

    for (mdp::RangeSet<mdp::BytesRange>::const_iterator it = value.sourceMap.begin();
         it != value.sourceMap.end();
         ++it) {

        sos::Array sourceMapRow;

        sourceMapRow.push(sos::Number(it->location));
        sourceMapRow.push(sos::Number(it->length));

        sourceMap.push(sourceMapRow);
    }

    return sourceMap;
}

// Forward declarations
sos::Array WrapTypeSectionSourcemap(const SourceMap<mson::TypeSection>& typeSection);

sos::Object WrapPropertyMemberSourcemap(const SourceMap<mson::PropertyMember>& propertyMember)
{
    sos::Object propertyMemberObject;

    // Name
    propertyMemberObject.set(SerializeKey::Name, WrapSourcemap(propertyMember.name));

    // Description
    propertyMemberObject.set(SerializeKey::Description, WrapSourcemap(propertyMember.description));

    // Value Definition
    propertyMemberObject.set(SerializeKey::ValueDefinition, WrapSourcemap(propertyMember.valueDefinition));

    // Type Sections
    propertyMemberObject.set(SerializeKey::Sections, 
                             WrapCollection<mson::TypeSection>()(propertyMember.sections.collection, WrapTypeSectionSourcemap));

    return propertyMemberObject;
}

sos::Object WrapValueMemberSourcemap(const SourceMap<mson::ValueMember>& valueMember)
{
    sos::Object valueMemberObject;

    // Description
    valueMemberObject.set(SerializeKey::Description, WrapSourcemap(valueMember.description));

    // Value Definition
    valueMemberObject.set(SerializeKey::ValueDefinition, WrapSourcemap(valueMember.valueDefinition));

    // Type Sections
    valueMemberObject.set(SerializeKey::Sections, 
                          WrapCollection<mson::TypeSection>()(valueMember.sections.collection, WrapTypeSectionSourcemap));

    return valueMemberObject;
}

sos::Array WrapMixinSourcemap(const SourceMap<mson::Mixin>& mixin)
{
    return WrapSourcemap(mixin);
}

sos::Base WrapMSONElementSourcemap(const SourceMap<mson::Element>& element)
{
    if (!element.elements().collection.empty()) {
        // Same for oneOf
        return WrapCollection<mson::Element>()(element.elements().collection, WrapMSONElementSourcemap);
    }
    else if (!element.mixin.sourceMap.empty()) {
        return WrapMixinSourcemap(element.mixin);             // return sos::Array
    }
    else if (!element.value.empty()) {
        return WrapValueMemberSourcemap(element.value);       // return sos::Object
    }
    else if (!element.property.empty()) {
        return WrapPropertyMemberSourcemap(element.property); // return sos::Object
    }

    return sos::Null();                                       // return sos::Null
}

sos::Array WrapTypeSectionSourcemap(const SourceMap<mson::TypeSection>& section)
{
    if (!section.description.sourceMap.empty()) {
        return WrapSourcemap(section.description);
    }
    else if (!section.value.sourceMap.empty()) {
        return WrapSourcemap(section.value);
    }
    else if (!section.elements().collection.empty()) {
        return WrapCollection<mson::Element>()(section.elements().collection, WrapMSONElementSourcemap);
    }

    return sos::Array();
}

sos::Object WrapDataStructureSourcemap(const SourceMap<DataStructure>& dataStructure)
{
    sos::Object dataStructureObject;

    // Name
    dataStructureObject.set(SerializeKey::Name, WrapSourcemap(dataStructure.name));

    // Type Definition
    dataStructureObject.set(SerializeKey::TypeDefinition, WrapSourcemap(dataStructure.typeDefinition));

    // Type Sections
    dataStructureObject.set(SerializeKey::Sections, 
                            WrapCollection<mson::TypeSection>()(dataStructure.sections.collection, WrapTypeSectionSourcemap));

    return dataStructureObject;
}

sos::Object WrapAssetSourcemap(const SourceMap<Asset>& asset)
{
    sos::Object assetObject;

    // Content
    assetObject.set(SerializeKey::Content, WrapSourcemap(asset));

    return assetObject;
}

sos::Object WrapPayloadSourcemap(const SourceMap<Payload>& payload)
{
    sos::Object payloadObject;

    // Reference
    if (!payload.reference.sourceMap.empty()) {
        payloadObject.set(SerializeKey::Reference, WrapSourcemap(payload.reference));
    }

    // Name
    payloadObject.set(SerializeKey::Name, WrapSourcemap(payload.name));

    // Description
    payloadObject.set(SerializeKey::Description, WrapSourcemap(payload.description));

    // Headers
    payloadObject.set(SerializeKey::Headers, 
                      WrapCollection<Header>()(payload.headers.collection, WrapSourcemap));

    // Body
    payloadObject.set(SerializeKey::Body, WrapSourcemap(payload.body));

    // Schema
    payloadObject.set(SerializeKey::Schema, WrapSourcemap(payload.schema));

    // Content
    sos::Array content;

    /// Attributes
    if (!payload.attributes.empty()) {
        content.push(WrapDataStructureSourcemap(payload.attributes));
    }

    /// Asset 'bodyExample'
    if (!payload.body.sourceMap.empty()) {
        content.push(WrapAssetSourcemap(payload.body));
    }

    /// Asset 'bodySchema'
    if (!payload.schema.sourceMap.empty()) {
        content.push(WrapAssetSourcemap(payload.schema));
    }

    payloadObject.set(SerializeKey::Content, content);

    return payloadObject;
}

sos::Object WrapParameterValueSourceMap(const SourceMap<Value>& value)
{
    sos::Object object;
    object.set(SerializeKey::Value, WrapSourcemap(value));
    return object;
}

sos::Object WrapParameterSourcemap(const SourceMap<Parameter>& parameter) 
{
    sos::Object object;

    // Name
    object.set(SerializeKey::Name, WrapSourcemap(parameter.name));

    // Description
    object.set(SerializeKey::Description, WrapSourcemap(parameter.description));

    // Type
    object.set(SerializeKey::Type, WrapSourcemap(parameter.type));

    // Use
    object.set(SerializeKey::Required, WrapSourcemap(parameter.use));

    // Example Value
    object.set(SerializeKey::Example, WrapSourcemap(parameter.exampleValue));

    // Default Value
    object.set(SerializeKey::Default, WrapSourcemap(parameter.defaultValue));
    
    // Nullable Value
    object.set(SerializeKey::Nullable, WrapSourcemap(parameter.nullableValue));

    // Values
    object.set(SerializeKey::Values,
               WrapCollection<Value>()(parameter.values.collection, WrapParameterValueSourceMap));

    return object;
}

sos::Object WrapTransactionExampleSourcemap(const SourceMap<TransactionExample>& example)
{
    sos::Object exampleObject;

    // Name
    exampleObject.set(SerializeKey::Name, WrapSourcemap(example.name));

    // Description
    exampleObject.set(SerializeKey::Description, WrapSourcemap(example.description));

    // Requests
    exampleObject.set(SerializeKey::Requests, 
                      WrapCollection<Request>()(example.requests.collection, WrapPayloadSourcemap));

    // Responses
    exampleObject.set(SerializeKey::Responses, 
                      WrapCollection<Response>()(example.responses.collection, WrapPayloadSourcemap));

    return exampleObject;
}

sos::Object WrapActionSourcemap(const SourceMap<Action>& action)
{
    sos::Object actionObject;

    // Name
    actionObject.set(SerializeKey::Name, WrapSourcemap(action.name));

    // Description
    actionObject.set(SerializeKey::Description, WrapSourcemap(action.description));

    // HTTP Method
    actionObject.set(SerializeKey::Method, WrapSourcemap(action.method));

    // Parameters
    actionObject.set(SerializeKey::Parameters, 
                     WrapCollection<Parameter>()(action.parameters.collection, WrapParameterSourcemap));

    // Transaction Examples
    actionObject.set(SerializeKey::Examples, 
                     WrapCollection<TransactionExample>()(action.examples.collection, WrapTransactionExampleSourcemap));

    // Attributes
    sos::Object attributes;

    /// Relation
    attributes.set(SerializeKey::Relation, WrapSourcemap(action.relation));

    /// URI Template
    attributes.set(SerializeKey::URITemplate, WrapSourcemap(action.uriTemplate));

    actionObject.set(SerializeKey::Attributes, attributes);

    // Content
    sos::Array content;

    /// Attributes
    if (!action.attributes.empty()) {
        content.push(WrapDataStructureSourcemap(action.attributes));
    }

    actionObject.set(SerializeKey::Content, content);

    return actionObject;
}

sos::Object WrapResourceSourcemap(const SourceMap<Resource>& resource)
{
    sos::Object resourceObject;

    // Name
    resourceObject.set(SerializeKey::Name, WrapSourcemap(resource.name));

    // Description
    resourceObject.set(SerializeKey::Description, WrapSourcemap(resource.description));

    // URI Template
    resourceObject.set(SerializeKey::URITemplate, WrapSourcemap(resource.uriTemplate));

    // Model
    sos::Object model = (resource.model.name.sourceMap.empty() ? sos::Object() : WrapPayloadSourcemap(resource.model));
    resourceObject.set(SerializeKey::Model, model);

    // Parameters
    resourceObject.set(SerializeKey::Parameters, 
                       WrapCollection<Parameter>()(resource.parameters.collection, WrapParameterSourcemap));

    // Actions
    resourceObject.set(SerializeKey::Actions, 
                       WrapCollection<Action>()(resource.actions.collection, WrapActionSourcemap));

    // Content
    sos::Array content;

    /// Attributes
    if (!resource.attributes.empty()) {
        content.push(WrapDataStructureSourcemap(resource.attributes));
    }

    resourceObject.set(SerializeKey::Content, content);

    return resourceObject;
}

sos::Object WrapResourceGroupSourcemap(const SourceMap<Element>& resourceGroup)
{
    sos::Object resourceGroupObject;

    // Name
    resourceGroupObject.set(SerializeKey::Name, WrapSourcemap(resourceGroup.attributes.name));

    // Description & Resources
    SourceMap<Description> description;
    sos::Array resources;

    for (Collection<SourceMap<Element> >::const_iterator it = resourceGroup.content.elements().collection.begin();
         it != resourceGroup.content.elements().collection.end();
         ++it) {

        if (it->element == Element::ResourceElement) {
            resources.push(WrapResourceSourcemap(it->content.resource));
        }
        else if (it->element == Element::CopyElement) {
            description.sourceMap.append(it->content.copy.sourceMap);
        }
    }

    resourceGroupObject.set(SerializeKey::Description, WrapSourcemap(description));
    resourceGroupObject.set(SerializeKey::Resources, resources);

    return resourceGroupObject;
}

sos::Object WrapElementSourcemap(const SourceMap<Element>& element)
{
    sos::Object elementObject;

    if (!element.attributes.name.sourceMap.empty()) {

        sos::Object attributes;

        attributes.set(SerializeKey::Name, WrapSourcemap(element.attributes.name));
        elementObject.set(SerializeKey::Attributes, attributes);
    }

    switch (element.element) {
        case Element::CopyElement:
        {
            elementObject.set(SerializeKey::Content, WrapSourcemap(element.content.copy));
            break;
        }

        case Element::DataStructureElement:
        {
            return WrapDataStructureSourcemap(element.content.dataStructure);
        }

        case Element::ResourceElement:
        {
            return WrapResourceSourcemap(element.content.resource);
        }

        case Element::CategoryElement:
        {
            elementObject.set(SerializeKey::Content, 
                              WrapCollection<Element>()(element.content.elements().collection, WrapElementSourcemap));
            break;
        }

        default:
            break;
    }

    return elementObject;
}

bool IsElementResourceGroup(const SourceMap<Element>& element)
{
    return element.element == Element::CategoryElement && element.category == Element::ResourceGroupCategory;
}

sos::Object drafter::WrapBlueprintSourcemap(const SourceMap<Blueprint>& blueprint)
{
    sos::Object blueprintObject;

    // Metadata
    blueprintObject.set(SerializeKey::Metadata, 
                        WrapCollection<Metadata>()(blueprint.metadata.collection, WrapSourcemap));

    // Name
    blueprintObject.set(SerializeKey::Name, WrapSourcemap(blueprint.name));

    // Description
    blueprintObject.set(SerializeKey::Description, WrapSourcemap(blueprint.description));

    // Resource Groups
    blueprintObject.set(SerializeKey::ResourceGroups, 
                        WrapCollection<Element>()(blueprint.content.elements().collection, WrapResourceGroupSourcemap, IsElementResourceGroup));

    // Content
    blueprintObject.set(SerializeKey::Content, 
                        WrapCollection<Element>()(blueprint.content.elements().collection, WrapElementSourcemap));

    return blueprintObject;
}
