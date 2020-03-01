//
//  PayloadToApie.cc
//  apib2apie
//
//  Created by Thomas Jandecka on 25/02/20.
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//

#include "PayloadToApie.h"

#include <iterator>

#include <SourceAnnotation.h>

#include "../RefractDataStructure.h"
#include "../Render.h"
#include "../RefractSourceMap.h"

#include "../refract/JsonValue.h"
#include "../refract/JsonSchema.h"

#include "../utils/so/JsonIo.h"

#include "../format/MediatypeFmt.h"
#include "../format/Format.h"

#include "../ConversionContext.h"

#include "ParametersToApie.h"
#include "CopyToApie.h"
#include "CollectionToApie.h"

using namespace drafter;
using namespace refract;
using namespace apib2apie;

namespace
{
    std::unique_ptr<IElement> HeaderToApie( //
        const NodeInfo<snowcrash::Header>& header,
        ConversionContext& context)
    {
        auto element = make_element<MemberElement>( //
            from_primitive(header.node->first),
            from_primitive(header.node->second));

        AttachSourceMap(*element, header);

        return std::move(element);
    }

    bool isRequest(const NodeInfo<snowcrash::Action>& action)
    {
        return !action.isNull() && !action.node->method.empty();
    }

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

    apib::parser::mediatype::state addSchemaSubtype(apib::parser::mediatype::state m)
    {
        if (m.suffix.empty())
            m.suffix = m.subtype;
        m.subtype = "schema";
        return m;
    }

    void generateAttachments(const IElement& expanded,   //
        const apib::parser::mediatype::state& mediaType, //
        ArrayElement::ValueType& out)
    {
        using apib::format::serialize;

        if (IsJSONContentType(mediaType)) {
            std::stringstream ss{};
            drafter::utils::so::serialize_json(ss, refract::generateJsonValue(expanded));
            out.push_back(make_asset_element( //
                ss.str(),                     //
                SerializeKey::MessageBody,    //
                serialize(mediaType)));
        }
        if (IsJSONContentType(mediaType) || IsJSONSchemaContentType(mediaType)) {
            std::stringstream ss{};
            drafter::utils::so::serialize_json(ss, refract::schema::generateJsonSchema(expanded));
            out.push_back(make_asset_element(        //
                ss.str(),                            //
                SerializeKey::MessageBodySchema,     //
                IsJSONSchemaContentType(mediaType) ? //
                    serialize(mediaType) :           //
                    serialize(addSchemaSubtype(mediaType))));
        }
    }

    void generateAttachments(ConversionContext& context, //
        std::unique_ptr<IElement> unexpanded,            //
        const apib::parser::mediatype::state& mediaType, //
        ArrayElement::ValueType& out)
    {
        if (!unexpanded)
            return;

        auto expanded = ExpandRefract(std::move(unexpanded), context);

        if (expanded)
            generateAttachments(*expanded, mediaType, out);
    }

    void generateAttachments(ConversionContext& context, //
        const NodeInfo<snowcrash::DataStructure>& ds,    //
        const apib::parser::mediatype::state& mediaType, //
        ArrayElement::ValueType& out)
    {
        assert(!ds.empty());

        auto unexpanded = MSONToRefract(ds, context);

        if (!unexpanded)
            return;

        generateAttachments(*unexpanded, mediaType, out);
    }

    void attachDataStructure(std::unique_ptr<IElement> ds, ArrayElement::ValueType& out)
    {
        out.push_back(refract::make_unique<HolderElement>(SerializeKey::DataStructure, dsd::Holder(std::move(ds))));
    }

    void PayloadContentToApie(                       //
        const NodeInfo<snowcrash::Payload>& payload, //
        const NodeInfo<snowcrash::Action>& action,   //
        ConversionContext& context,                  //
        ArrayElement::ValueType& content)
    {
        using apib::format::serialize;

        if (!payload.node->description.empty())
            content.push_back(CopyToApie(MAKE_NODE_INFO(payload, description)));

        auto unexpandedAttrs = payload.node->attributes.empty() ? //
            nullptr :                                             //
            MSONToRefract(MAKE_NODE_INFO(payload, attributes), context);

        // Push dataStructure
        if (unexpandedAttrs) {
            // TODO: avoid expandMson branch, only used in unit tests
            if (context.expandMson()) {
                if (auto expanded = ExpandRefract(clone(*unexpandedAttrs), context)) {
                    attachDataStructure(std::move(expanded), content);
                }
            } else {
                attachDataStructure(clone(*unexpandedAttrs), content);
            }
        }

        // Get content type
        const auto mediaType = parseMediaType(getContentTypeFromHeaders(payload.node->headers));

        // Push Body Asset
        if (!payload.node->body.empty()) {
            content.push_back(make_asset_element( //
                payload.node->body,               //
                SerializeKey::MessageBody,        //
                serialize(mediaType),             //
                &payload.sourceMap->body.sourceMap));
        }

        // Push Schema Asset
        if (!payload.node->schema.empty()) {
            content.push_back(make_asset_element(       //
                payload.node->schema,                   //
                SerializeKey::MessageBodySchema,        //
                serialize(addSchemaSubtype(mediaType)), //
                &payload.sourceMap->schema.sourceMap));
        }

        // Generate Assets
        if (!unexpandedAttrs && !action.isNull() && !action.node->attributes.empty()) {

            // If no payload attributes, try generating from action attributes
            generateAttachments(                    //
                context,                            //
                MAKE_NODE_INFO(action, attributes), //
                mediaType,                          //
                content);

        } else {

            // else use already translated
            generateAttachments(            //
                context,                    //
                std::move(unexpandedAttrs), //
                mediaType,                  //
                content);
        }
    }
}

std::unique_ptr<IElement> apib2apie::PayloadToApie( //
    const NodeInfo<snowcrash::Payload>& payload,
    const NodeInfo<snowcrash::Action>& action,
    ConversionContext& context)
{
    using namespace snowcrash;

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
            SerializeKey::HrefVariables, ParametersToApie(MAKE_NODE_INFO(payload, parameters), context));
    }

    if (!payload.node->headers.empty()) {
        result->attributes().set(SerializeKey::Headers,
            CollectionToApie<ArrayElement>(
                MAKE_NODE_INFO(payload, headers), context, HeaderToApie, SerializeKey::HTTPHeaders));
    }

    PayloadContentToApie(payload, action, context, result->get());

    return std::move(result);
}
