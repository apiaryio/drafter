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
#include "RefractSourceMap.h"

#include "refract/Exception.h"
#include "refract/JsonValue.h"
#include "refract/JsonSchema.h"

#include "utils/log/Trivial.h"
#include "utils/so/JsonIo.h"

#include "backend/MediaTypeS11n.h"
#include "backend/Backend.h"

#include <iterator>
#include <set>

#include "NamedTypesRegistry.h"
#include "ConversionContext.h"

using namespace drafter;
using namespace refract;
using namespace drafter::utils::log;

// Forward Declarations
std::unique_ptr<IElement> ElementToRefract(const NodeInfo<snowcrash::Element>& element, ConversionContext& context);

namespace
{
    struct is_nullptr {
        template <typename T>
        constexpr bool operator()(const T* ptr) const noexcept
        {
            return ptr == nullptr;
        }

        template <typename T>
        constexpr bool operator()(const std::unique_ptr<T>& ptr) const noexcept
        {
            return ptr == nullptr;
        }
    };

    template <typename Collection>
    void RemoveEmptyElements(Collection& elements)
    {
        elements.erase(std::remove_if(elements.begin(), elements.end(), is_nullptr{}), elements.end());
    }

    template <typename T, typename DataT, typename Functor>
    void NodeInfoToElements(
        const NodeInfo<T>& nodeInfo, const Functor& transformFunctor, DataT& content, ConversionContext& context)
    {
        NodeInfoCollection<T> nodeInfoCollection(nodeInfo);

        std::transform(nodeInfoCollection.begin(),
            nodeInfoCollection.end(),
            std::back_inserter(content),
            [&transformFunctor, &context](const typename NodeInfoCollection<T>::value_type& nodeInfo) { //
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

        return element;
    }

    bool isRequest(const NodeInfo<snowcrash::Action>& action)
    {
        return !action.isNull() && !action.node->method.empty();
    }
} // namespace

std::unique_ptr<IElement> drafter::DataStructureToRefract(
    const NodeInfo<snowcrash::DataStructure>& dataStructure, ConversionContext& context)
{
    // TODO: Check for already expanded MSON in context.registry and use it if possible.
    // We aren't doing it yet because APIB AST with MSON Refract will start getting sourcemaps
    // which is a breaking change. Once we remove APIB AST code, we can move forward with this.
    auto msonElement = MSONToRefract(dataStructure, context);

    if (context.expandMson()) {
        auto msonExpanded = ExpandRefract(std::move(msonElement), context);
        msonElement = std::move(msonExpanded);
    }

    return msonElement ?                                                                                      //
        refract::make_unique<HolderElement>(SerializeKey::DataStructure, dsd::Holder(std::move(msonElement))) //
        :
        nullptr;
}

std::unique_ptr<IElement> MetadataToRefract(const NodeInfo<snowcrash::Metadata>& metadata, ConversionContext& context)
{
    auto element = make_element<MemberElement>( //
        from_primitive(metadata.node->first),
        from_primitive(metadata.node->second));

    element->meta().set(SerializeKey::Classes,
        generate_element<ArrayElement>([](dsd::Array& data) { //
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

    return element;
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
        element->attributes().set(SerializeKey::Default,
            make_element<EnumElement>(LiteralToRefract(MAKE_NODE_INFO(parameter, defaultValue), context)));
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

namespace
{
    std::unique_ptr<StringElement> make_asset_element( //
        std::string content,                           //
        std::string klass,                             //
        std::string contentType,                       //
        const mdp::CharactersRangeSet* sourceMap = nullptr)
    {
        auto result = from_primitive_t(SerializeKey::Asset, std::move(content));

        if (!klass.empty())
            result->meta().set(SerializeKey::Classes, //
                make_element<ArrayElement>(from_primitive(std::move(klass))));

        if (sourceMap && !sourceMap->empty())
            result->attributes().set(          //
                SerializeKey::SourceMap,       //
                SourceMapToRefract(*sourceMap) //
            );

        if (!contentType.empty())
            result->attributes().set(SerializeKey::ContentType, //
                from_primitive(std::move(contentType)));

        return result;
    }
}

namespace
{
    using MediaType = apib::parser::mediatype::state;

    apib::parser::mediatype::state jsonSchemaType()
    {
        return apib::parser::mediatype::state{ "application", "schema", "json", {} };
    }

    apib::parser::mediatype::state textPlainType()
    {
        return apib::parser::mediatype::state{ "text", "plain", "", {} };
    }

    bool IsAnyJSONContentType(const apib::parser::mediatype::state& t)
    {
        return IsJSONContentType(t) || IsJSONSchemaContentType(t);
    }

    void generateValueAsset( //
        ArrayElement::ValueType& out,
        const ConversionContext& context,
        const IElement& expanded,
        const apib::parser::mediatype::state& mediaType)
    {
        using apib::backend::serialize;
        if (IsAnyJSONContentType(mediaType)) {
            std::stringstream ss{};
            drafter::utils::so::serialize_json(ss, refract::generateJsonValue(expanded));
            out.push_back(make_asset_element(ss.str(), SerializeKey::MessageBody, serialize(mediaType)));
        }
    }

    void generateSchemaAsset( //
        ArrayElement::ValueType& out,
        const ConversionContext& context,
        const IElement& expanded,
        const apib::parser::mediatype::state& mediaType)
    {
        using apib::backend::serialize;
        if (IsAnyJSONContentType(mediaType)) {
            std::stringstream ss{};
            drafter::utils::so::serialize_json(ss, refract::schema::generateJsonSchema(expanded));
            out.push_back(make_asset_element(ss.str(), SerializeKey::MessageBodySchema, serialize(jsonSchemaType())));
        }
    }

    void attachDataStructure(std::unique_ptr<IElement> ds, ArrayElement::ValueType& out)
    {
        out.push_back(refract::make_unique<HolderElement>(SerializeKey::DataStructure, dsd::Holder(std::move(ds))));
    }

}

std::unique_ptr<IElement> PayloadToRefract( //
    const NodeInfo<snowcrash::Payload>& payload,
    const NodeInfo<snowcrash::Action>& action,
    ConversionContext& context)
{
    using namespace snowcrash;
    using apib::backend::serialize;

    auto result = make_element<ArrayElement>();

    if (isRequest(action)) {
        result->element(SerializeKey::HTTPRequest);
        result->attributes().set(SerializeKey::Method, PrimitiveToRefract(MAKE_NODE_INFO(action, method)));

        if (!payload.isNull() && !payload.node->name.empty()) {
            result->meta().set(SerializeKey::Title, PrimitiveToRefract(MAKE_NODE_INFO(payload, name)));
        }
    } else {
        result->element(SerializeKey::HTTPResponse);

        // FIXME: tests pass without commented out part of condition
        // delivery test to see this part is required else remove it
        // related discussion: https://github.com/apiaryio/drafter/pull/148/files#r42275194
        if (!payload.isNull() /* && !payload.node->name.empty() */) {
            result->attributes().set(SerializeKey::StatusCode, PrimitiveToRefract(MAKE_NODE_INFO(payload, name)));
        }
    }

    AttachSourceMap(*result, payload);

    // If no payload, return immediately
    if (payload.isNull()) {
        return std::move(result);
    }

    if (!payload.node->parameters.empty()) {
        result->attributes().set(
            SerializeKey::HrefVariables, ParametersToRefract(MAKE_NODE_INFO(payload, parameters), context));
    }

    if (!payload.node->headers.empty()) {
        result->attributes().set(SerializeKey::Headers,
            CollectionToRefract<ArrayElement>(
                MAKE_NODE_INFO(payload, headers), context, HeaderToRefract, SerializeKey::HTTPHeaders));
    }

    auto& content = result->get();

    if (!payload.node->description.empty())
        content.push_back(CopyToRefract(MAKE_NODE_INFO(payload, description)));

    auto dataStructure = payload.node->attributes.empty() ? //
        nullptr :                                           //
        MSONToRefract(MAKE_NODE_INFO(payload, attributes), context);

    // Push dataStructure
    if (dataStructure) {
        if (context.expandMson()) { // TODO: remove/avoid, only used for unit tests
            if (auto expanded = ExpandRefract(clone(*dataStructure), context)) {
                attachDataStructure(std::move(expanded), content);
            }
        } else {
            attachDataStructure(clone(*dataStructure), content);
        }
    }

    // Get content type
    const auto mediaType = parseMediaType(getContentTypeFromHeaders(payload.node->headers));

    // Determine any MSON to generate value/schema
    if (!dataStructure && !action.isNull() && !action.node->attributes.empty())
        dataStructure = MSONToRefract(MAKE_NODE_INFO(action, attributes), context);
    auto dataStructureExpanded = dataStructure ? ExpandRefract(std::move(dataStructure), context) : nullptr;

    // Push Body Asset
    if (!payload.node->body.empty()) {
        content.push_back(make_asset_element( //
            payload.node->body,
            SerializeKey::MessageBody,
            serialize(mediaType),
            &payload.sourceMap->body.sourceMap));

    } else if (dataStructureExpanded && !is_skip_gen_bodies(context.options())) {
        // otherwise, generate one from attributes
        generateValueAsset(content, context, *dataStructureExpanded, mediaType);
    }

    // Push Schema Asset
    if (!payload.node->schema.empty()) {
        content.push_back(make_asset_element( //
            payload.node->schema,
            SerializeKey::MessageBodySchema,
            serialize(IsAnyJSONContentType(mediaType) ? jsonSchemaType() : textPlainType()),
            &payload.sourceMap->schema.sourceMap));

    } else if (dataStructureExpanded && !is_skip_gen_body_schemas(context.options())) {
        // otherwise, generate one from attributes
        generateSchemaAsset(content, context, *dataStructureExpanded, mediaType);
    }

    return std::move(result);
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

    if (!resource.node->attributes.empty()) {
        content.push_back(DataStructureToRefract(MAKE_NODE_INFO(resource, attributes), context));
    }
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
    const snowcrash::SourceAnnotation& annotation, const std::string& key, ConversionContext& context)
{
    auto element = from_primitive(annotation.message);

    element->element(SerializeKey::Annotation);

    element->meta().set(SerializeKey::Classes, make_element<ArrayElement>(from_primitive(key)));

    element->attributes().set(SerializeKey::AnnotationCode, from_primitive(annotation.code));
    element->attributes().set(
        SerializeKey::SourceMap, SourceMapToRefractWithColumnLineInfo(annotation.location, context));

    return std::move(element);
}
