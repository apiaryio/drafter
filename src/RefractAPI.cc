//
//  RefractAPI.cc
//  drafter
//
//  Created by Jiri Kratochvil on 31/07/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "SourceAnnotation.h"

#include "RefractDataStructure.h"
#include "RefractAPI.h"
#include "Render.h"
#include "refract/Exception.h"

#include "RefractSourceMap.h"

#include <iterator>
#include <set>

#include "NamedTypesRegistry.h"
#include "ConversionContext.h"

using namespace drafter;
using namespace refract;

// Forward Declarations
std::unique_ptr<IElement> ElementToRefract(const NodeInfo<snowcrash::Element>& element, ConversionContext& context);

namespace
{
    template <typename Collection>
    void RemoveEmptyElements(Collection& elements)
    {
        elements.erase(std::remove_if(elements.begin(), elements.end(), [](const auto& el) { return el == nullptr; }),
            elements.end());
    }

    template <typename T, typename DataT, typename Functor>
    void NodeInfoToElements(
        const NodeInfo<T>& nodeInfo, const Functor& transformFunctor, DataT& content, ConversionContext& context)
    {
        NodeInfoCollection<T> nodeInfoCollection(nodeInfo);

        std::transform(nodeInfoCollection.begin(),
            nodeInfoCollection.end(),
            std::back_inserter(content),
            [&transformFunctor, &context](const auto& nodeInfo) { //
                return transformFunctor(nodeInfo, context);
            });
    }

    // TODO make generator out of this?
    template <typename T, typename C, typename F>
    std::unique_ptr<T> CollectionToRefract(const NodeInfo<C>& collection,
        ConversionContext& context,
        const F& transformFunctor,
        const std::string& key = std::string())
    {
        auto element = make_element<T>();

        if (!key.empty()) {
            element->element(key);
        }

        {
            auto& content = element->get();

            NodeInfoToElements(collection, transformFunctor, content, context);

            RemoveEmptyElements(content);
        }

        return std::move(element);
    }
}

std::unique_ptr<IElement> drafter::DataStructureToRefract(
    const NodeInfo<snowcrash::DataStructure>& dataStructure, ConversionContext& context)
{
    // TODO: Check for already expanded MSON in context.registry and use it if possible.
    // We aren't doing it yet because APIB AST with MSON Refract will start getting sourcemaps
    // which is a breaking change. Once we remove APIB AST code, we can move forward with this.
    auto msonElement = MSONToRefract(dataStructure, context);

    if (context.options.expandMSON) {
        auto msonExpanded = ExpandRefract(std::move(msonElement), context);
        msonElement = std::move(msonExpanded);
    }

    if (!msonElement) {
        return nullptr;
    }

    return std::make_unique<HolderElement>(SerializeKey::DataStructure, dsd::Holder(std::move(msonElement)));
}

std::unique_ptr<IElement> MetadataToRefract(const NodeInfo<snowcrash::Metadata>& metadata, ConversionContext& context)
{
    auto element = make_element<MemberElement>( //
        from_primitive(metadata.node->first),
        from_primitive(metadata.node->second));

    element->meta().set(SerializeKey::Classes,
        generate_element<ArrayElement>([](auto& data) { //
            data.push_back(from_primitive(SerializeKey::User));
        }));

    AttachSourceMap(*element, metadata);

    return std::move(element);
}

std::unique_ptr<IElement> CopyToRefract(const NodeInfo<std::string>& copy)
{
    if (copy.node->empty()) {
        return nullptr;
    }

    auto element = PrimitiveToRefract(copy);
    element->element(SerializeKey::Copy);

    return std::move(element);
}

std::unique_ptr<IElement> ParameterValuesToRefract(
    const NodeInfo<snowcrash::Parameter>& parameter, ConversionContext& context)
{
    // Add sample value
    auto element = parameter.node->exampleValue.empty() ? //
        make_empty<EnumElement>() :
        make_element<EnumElement>(LiteralToRefract(MAKE_NODE_INFO(parameter, exampleValue), context));

    // Add default value
    if (!parameter.node->defaultValue.empty()) {
        element->attributes().set(
            SerializeKey::Default, LiteralToRefract(MAKE_NODE_INFO(parameter, defaultValue), context));
    }

    // Add enumerations
    element->attributes().set(SerializeKey::Enumerations,
        CollectionToRefract<ArrayElement>(MAKE_NODE_INFO(parameter, values), context, LiteralToRefract));

    return std::move(element);
}

// NOTE: We removed type specific templates from here in https://github.com/apiaryio/drafter/pull/447
std::unique_ptr<IElement> ExtractParameter(const NodeInfo<snowcrash::Parameter>& parameter, ConversionContext& context)
{
    std::unique_ptr<IElement> element = nullptr;

    if (parameter.node->values.empty()) {
        auto element = parameter.node->exampleValue.empty() ? //
            make_empty<StringElement>() :
            LiteralToRefract(MAKE_NODE_INFO(parameter, exampleValue), context);

        if (!parameter.node->defaultValue.empty()) {
            element->attributes().set(
                SerializeKey::Default, PrimitiveToRefract(MAKE_NODE_INFO(parameter, defaultValue)));
        }

        return std::move(element);
    } else {
        return ParameterValuesToRefract(parameter, context);
    }
}

std::unique_ptr<IElement> ParameterToRefract(
    const NodeInfo<snowcrash::Parameter>& parameter, ConversionContext& context)
{
    auto element = make_element<MemberElement>(
        PrimitiveToRefract(MAKE_NODE_INFO(parameter, name)), ExtractParameter(parameter, context));

    // Description
    if (!parameter.node->description.empty()) {
        element->meta().set(SerializeKey::Description, PrimitiveToRefract(MAKE_NODE_INFO(parameter, description)));
    }

    if (!parameter.node->type.empty()) {
        element->meta().set(SerializeKey::Title, PrimitiveToRefract(MAKE_NODE_INFO(parameter, type)));
    }

    // Parameter use
    {

        std::string use = SerializeKey::Required;

        if (parameter.node->use == snowcrash::OptionalParameterUse) {
            use = SerializeKey::Optional;
        }

        element->attributes().set(SerializeKey::TypeAttributes, make_element<ArrayElement>(from_primitive(use)));
    }

    return std::move(element);
}

std::unique_ptr<IElement> ParametersToRefract(
    const NodeInfo<snowcrash::Parameters>& parameters, ConversionContext& context)
{
    return CollectionToRefract<ObjectElement>(parameters, context, ParameterToRefract, SerializeKey::HrefVariables);
}

std::unique_ptr<IElement> HeaderToRefract(const NodeInfo<snowcrash::Header>& header, ConversionContext& context)
{
    auto element = make_element<MemberElement>(from_primitive(header.node->first), from_primitive(header.node->second));

    AttachSourceMap(*element, header);

    return std::move(element);
}

std::unique_ptr<IElement> AssetToRefract(
    const NodeInfo<snowcrash::Asset>& asset, const std::string& contentType, const std::string& metaClass)
{
    if (asset.node->empty()) {
        return nullptr;
    }

    auto element = PrimitiveToRefract(asset);

    element->element(SerializeKey::Asset);
    element->meta().set(SerializeKey::Classes, make_element<ArrayElement>(from_primitive(metaClass)));

    if (!contentType.empty()) {
        // FIXME: "contentType" has no sourceMap?
        element->attributes().set(SerializeKey::ContentType, from_primitive(contentType));
    }

    return std::move(element);
}

std::unique_ptr<IElement> PayloadToRefract(
    const NodeInfo<snowcrash::Payload>& payload, const NodeInfo<snowcrash::Action>& action, ConversionContext& context)
{
    auto element = make_element<ArrayElement>();

    // Use HTTP method to recognize if request or response
    if (action.isNull() || action.node->method.empty()) {
        element->element(SerializeKey::HTTPResponse);

        // FIXME: tests pass without commented out part of condition
        // delivery test to see this part is required else remove it
        // related discussion: https://github.com/apiaryio/drafter/pull/148/files#r42275194
        if (!payload.isNull() /* && !payload.node->name.empty() */) {
            element->attributes().set(SerializeKey::StatusCode, PrimitiveToRefract(MAKE_NODE_INFO(payload, name)));
        }
    } else {
        element->element(SerializeKey::HTTPRequest);
        element->attributes().set(SerializeKey::Method, PrimitiveToRefract(MAKE_NODE_INFO(action, method)));

        if (!payload.isNull() && !payload.node->name.empty()) {
            element->meta().set(SerializeKey::Title, PrimitiveToRefract(MAKE_NODE_INFO(payload, name)));
        }
    }

    AttachSourceMap(*element, payload);

    auto& content = element->get();

    // If no payload, return immediately
    if (payload.isNull()) {
        return std::move(element);
    }

    if (!payload.node->parameters.empty()) {
        element->attributes().set(
            SerializeKey::HrefVariables, ParametersToRefract(MAKE_NODE_INFO(payload, parameters), context));
    }

    if (!payload.node->headers.empty()) {
        element->attributes().set(SerializeKey::Headers,
            CollectionToRefract<ArrayElement>(
                MAKE_NODE_INFO(payload, headers), context, HeaderToRefract, SerializeKey::HTTPHeaders));
    }

    if (!payload.node->description.empty())
        content.push_back(CopyToRefract(MAKE_NODE_INFO(payload, description)));

    if (!payload.node->attributes.empty())
        content.push_back(DataStructureToRefract(MAKE_NODE_INFO(payload, attributes), context));

    // FIXME: This whole rendering should be done after converting to refract. Currently, both
    // the renders will do MSONToRefract individually on the same thing. So, basically, the attributes
    // in a payload gets converted to refract 3 times which is something we should fix.
    try {
        // Render using boutique
        NodeInfoByValue<snowcrash::Asset> payloadBody = renderPayloadBody(payload, action, context);
        NodeInfoByValue<snowcrash::Asset> payloadSchema = renderPayloadSchema(payload, action, context);

        // Get content type
        std::string contentType = getContentTypeFromHeaders(payload.node->headers);
        std::string schemaContentType
            = snowcrash::RegexMatch(contentType, JSONRegex) ? JSONSchemaContentType : contentType;

        // Push Body Asset
        if (!payloadBody.first.empty())
            content.push_back(AssetToRefract( //
                NodeInfo<snowcrash::Asset>(payloadBody),
                contentType,
                SerializeKey::MessageBody));

        // Render only if Body is JSON or Schema is defined
        if (!payloadSchema.first.empty()) {
            content.push_back(AssetToRefract( //
                NodeInfo<snowcrash::Asset>(payloadSchema),
                schemaContentType,
                SerializeKey::MessageBodySchema));
        }
    }

    // what to do?
    // we are not able to generate JSON and/or JSON Schema
    // this is not fatal, rethrow will finish conversion

    // ideal solution is add warning into collection
    // but there is no way how to do it
    // in current time we solve it by rethrow
    catch (snowcrash::Error& e) {
        context.warn(snowcrash::Warning("unable to render JSON/JSONSchema. " + e.message,
            snowcrash::ApplicationError,
            payload.sourceMap->sourceMap));
    } catch (LogicError& e) {
        context.warn(snowcrash::Warning(std::string("unable to render JSON/JSONSchema. ").append(e.what()),
            snowcrash::ApplicationError,
            payload.sourceMap->sourceMap));
    }

    RemoveEmptyElements(content);
    return std::move(element);
}

std::unique_ptr<ArrayElement> TransactionToRefract(const NodeInfo<snowcrash::TransactionExample>& transaction,
    const NodeInfo<snowcrash::Action>& action,
    const NodeInfo<snowcrash::Request>& request,
    const NodeInfo<snowcrash::Response>& response,
    ConversionContext& context)
{
    auto element = make_element<ArrayElement>();
    auto& content = element->get();

    element->element(SerializeKey::HTTPTransaction);

    if (!transaction.node->description.empty())
        content.push_back(CopyToRefract(MAKE_NODE_INFO(transaction, description)));
    content.push_back(PayloadToRefract(request, action, context));
    content.push_back(PayloadToRefract(response, NodeInfo<snowcrash::Action>(), context));

    RemoveEmptyElements(content);

    return element;
}

std::unique_ptr<ArrayElement> ActionToRefract(const NodeInfo<snowcrash::Action>& action, ConversionContext& context)
{
    auto element = make_element<ArrayElement>();

    element->element(SerializeKey::Transition);
    element->meta().set(SerializeKey::Title, PrimitiveToRefract(MAKE_NODE_INFO(action, name)));

    if (!action.node->relation.str.empty()) {
        // We can't use PrimitiveToRefract() because `action.node->relation` here is a struct Relation
        auto relation = from_primitive(action.node->relation.str);
        AttachSourceMap(*relation, MAKE_NODE_INFO(action, relation));
        element->attributes().set(SerializeKey::Relation, std::move(relation));
    }

    if (!action.node->uriTemplate.empty()) {
        element->attributes().set(SerializeKey::Href, PrimitiveToRefract(MAKE_NODE_INFO(action, uriTemplate)));
    }

    if (!action.node->parameters.empty()) {
        element->attributes().set(
            SerializeKey::HrefVariables, ParametersToRefract(MAKE_NODE_INFO(action, parameters), context));
    }

    if (!action.node->attributes.empty()) {
        element->attributes().set(
            SerializeKey::Data, DataStructureToRefract(MAKE_NODE_INFO(action, attributes), context));
    }

    auto& content = element->get();

    if (!action.node->description.empty())
        content.push_back(CopyToRefract(MAKE_NODE_INFO(action, description)));

    typedef NodeInfoCollection<snowcrash::TransactionExamples> ExamplesType;
    ExamplesType examples(MAKE_NODE_INFO(action, examples));

    for (const auto& example : examples) {

        // When there are only responses
        if (example.node->requests.empty() && !example.node->responses.empty()) {

            typedef NodeInfoCollection<snowcrash::Responses> ResponsesType;
            ResponsesType responses(example.node->responses, example.sourceMap->responses);

            for (const auto& response : responses) {
                content.push_back(
                    TransactionToRefract(example, action, NodeInfo<snowcrash::Request>(), response, context));
            }
        }

        // When there are only requests or both responses and requests
        typedef NodeInfoCollection<snowcrash::Requests> RequestsType;
        RequestsType requests(example.node->requests, example.sourceMap->requests);

        for (const auto& request : requests) {

            if (example.node->responses.empty()) {
                content.push_back(
                    TransactionToRefract(example, action, request, NodeInfo<snowcrash::Response>(), context));
            }

            typedef NodeInfoCollection<snowcrash::Responses> ResponsesType;
            ResponsesType responses(example.node->responses, example.sourceMap->responses);

            for (const auto& response : responses) {
                content.push_back(TransactionToRefract(example, action, request, response, context));
            }
        }
    }

    RemoveEmptyElements(content);

    return element;
}

std::unique_ptr<ArrayElement> ResourceToRefract(
    const NodeInfo<snowcrash::Resource>& resource, ConversionContext& context)
{
    auto element = make_element<ArrayElement>();

    element->element(SerializeKey::Resource);

    element->meta().set(SerializeKey::Title, PrimitiveToRefract(MAKE_NODE_INFO(resource, name)));
    element->attributes().set(SerializeKey::Href, PrimitiveToRefract(MAKE_NODE_INFO(resource, uriTemplate)));

    if (!resource.node->parameters.empty()) {
        element->attributes().set(
            SerializeKey::HrefVariables, ParametersToRefract(MAKE_NODE_INFO(resource, parameters), context));
    }

    auto& content = element->get();

    if (!resource.node->description.empty())
        content.push_back(CopyToRefract(MAKE_NODE_INFO(resource, description)));

    if (!resource.node->attributes.empty())
        content.push_back(DataStructureToRefract(MAKE_NODE_INFO(resource, attributes), context));
    NodeInfoToElements(MAKE_NODE_INFO(resource, actions), ActionToRefract, content, context);

    RemoveEmptyElements(content);

    return element;
}

const snowcrash::SourceMap<snowcrash::Elements>* GetElementChildrenSourceMap(
    const NodeInfo<snowcrash::Element>& element)
{
    return element.sourceMap->content.elements().collection.empty() ? NodeInfo<snowcrash::Elements>::NullSourceMap() :
                                                                      &element.sourceMap->content.elements();
}

std::unique_ptr<ArrayElement> CategoryToRefract(const NodeInfo<snowcrash::Element>& element, ConversionContext& context)
{
    auto category = make_element<ArrayElement>();

    category->element(SerializeKey::Category);

    if (element.node->category == snowcrash::Element::ResourceGroupCategory) {
        category->meta().set(
            SerializeKey::Classes, make_element<ArrayElement>(from_primitive(SerializeKey::ResourceGroup)));
        category->meta().set(SerializeKey::Title, PrimitiveToRefract(MAKE_NODE_INFO(element, attributes.name)));
    } else if (element.node->category == snowcrash::Element::DataStructureGroupCategory) {
        category->meta().set(
            SerializeKey::Classes, make_element<ArrayElement>(from_primitive(SerializeKey::DataStructures)));
    }

    auto& content = category->get();

    if (!element.node->content.elements().empty()) {
        const NodeInfo<snowcrash::Elements> elementsNodeInfo
            = MakeNodeInfo(&element.node->content.elements(), GetElementChildrenSourceMap(element));

        NodeInfoToElements(elementsNodeInfo, ElementToRefract, content, context);
    }

    RemoveEmptyElements(content);

    return category;
}

std::unique_ptr<IElement> ElementToRefract(const NodeInfo<snowcrash::Element>& element, ConversionContext& context)
{
    switch (element.node->element) {
        case snowcrash::Element::ResourceElement:
            return ResourceToRefract(MAKE_NODE_INFO(element, content.resource), context);
        case snowcrash::Element::DataStructureElement:
            return DataStructureToRefract(MAKE_NODE_INFO(element, content.dataStructure), context);
        case snowcrash::Element::CopyElement:
            return CopyToRefract(MAKE_NODE_INFO(element, content.copy));
        case snowcrash::Element::CategoryElement:
            return CategoryToRefract(element, context);
        default:
            // we are not able to get sourcemap info there
            // It is strongly dependent on type of element.
            // For unknown type of Element we are not able to locate SourceMap
            // with adequate effort we do not provide it to upper level
            throw snowcrash::Error("unknown type of api description element", snowcrash::ApplicationError);
    }
}

std::unique_ptr<IElement> drafter::BlueprintToRefract(
    const NodeInfo<snowcrash::Blueprint>& blueprint, ConversionContext& context)
{
    auto ast = make_element<ArrayElement>();

    ast->element(SerializeKey::Category);

    ast->meta().set(SerializeKey::Classes, make_element<ArrayElement>(from_primitive(SerializeKey::API)));
    ast->meta().set(SerializeKey::Title, PrimitiveToRefract(MAKE_NODE_INFO(blueprint, name)));

    auto& content = ast->get();

    if (!blueprint.node->description.empty())
        content.push_back(CopyToRefract(MAKE_NODE_INFO(blueprint, description)));

    if (!blueprint.node->metadata.empty()) {
        ast->attributes().set(SerializeKey::Metadata,
            CollectionToRefract<ArrayElement>(MAKE_NODE_INFO(blueprint, metadata), context, MetadataToRefract));
    }

    NodeInfoToElements(MAKE_NODE_INFO(blueprint, content.elements()), ElementToRefract, content, context);

    RemoveEmptyElements(content);

    return std::move(ast);
}

std::unique_ptr<IElement> drafter::AnnotationToRefract(
    const snowcrash::SourceAnnotation& annotation, const std::string& key)
{
    auto element = from_primitive(annotation.message);

    element->element(SerializeKey::Annotation);

    element->meta().set(SerializeKey::Classes, make_element<ArrayElement>(from_primitive(key)));

    element->attributes().set(SerializeKey::AnnotationCode, from_primitive(static_cast<double>(annotation.code)));
    element->attributes().set(SerializeKey::SourceMap, SourceMapToRefract(annotation.location));

    return std::move(element);
}
