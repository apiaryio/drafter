//
//  Render.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 18/08/15.
//  Copyright (c) 2015 Apiary. All rights reserved.
//

#include "Render.h"
#include "RefractDataStructure.h"

#include "SourceAnnotation.h"
#include "BlueprintUtility.h"
#include "RegexMatch.h"

#include "ConversionContext.h"

#include "refract/RenderJSONVisitor.h"
#include "refract/JSONSchemaVisitor.h"

using namespace snowcrash;

namespace drafter {

    RenderFormat findRenderFormat(const std::string& contentType) {

        if (RegexMatch(contentType, JSONSchemaRegex)) {
            return JSONSchemaRenderFormat;
        }
        else if (RegexMatch(contentType, JSONRegex)) {
            return JSONRenderFormat;
        }

        return UndefinedRenderFormat;
    }

    std::string getContentTypeFromHeaders(const Headers& headers) {
        Collection<Header>::const_iterator header;

        header = std::find_if(headers.begin(), headers.end(),
                              std::bind2nd(MatchFirstWith<Header, std::string>(), HTTPHeaderName::ContentType));

        if (header != headers.end()) {
            return header->second;
        }

        return "";
    }

    NodeInfoByValue<Asset> renderPayloadBody(const NodeInfo<Payload>& payload,
                                             const NodeInfo<Action>& action,
                                             ConversionContext& context) {

        NodeInfoByValue<Asset> body = std::make_pair(payload.node->body, &payload.sourceMap->body);

        NodeInfo<Attributes> payloadAttributes = MAKE_NODE_INFO(payload, attributes);
        NodeInfo<Attributes> actionAttributes = MAKE_NODE_INFO(action, attributes);

        // hold attributes via pointer - because problems with assignment in NodeInfo<>
        NodeInfo<Attributes>* attributes = &payloadAttributes;

        if (payload.node->attributes.empty() && !action.isNull() && !action.node->attributes.empty()) {
           attributes = &actionAttributes;
        }

        RenderFormat renderFormat = findRenderFormat(getContentTypeFromHeaders(payload.node->headers));

        // Only continue down if we have a render format
        if (!payload.node->body.empty() || attributes->node->empty() || renderFormat == UndefinedRenderFormat) {
            return body;
        }

        // Expand MSON into Refract
        refract::IElement* element = MSONToRefract(*attributes, context);

        if (!element) {
            return body;
        }

        refract::IElement* expanded = ExpandRefract(element, context);

        if (!expanded) {
            return body;
        }

        // One of this will always execute since we have a catch above for not having render format
        switch (renderFormat) {
            case JSONRenderFormat:
            {
                refract::RenderJSONVisitor renderer;
                refract::Visit(renderer, *expanded);

                delete expanded;

                return std::make_pair(renderer.getString(), NodeInfo<Asset>::NullSourceMap());
            }

            case JSONSchemaRenderFormat:
            {
                refract::JSONSchemaVisitor renderer;
                std::string result = renderer.getSchema(*expanded);

                delete expanded;

                return std::make_pair(result, NodeInfo<Asset>::NullSourceMap());
            }

            case UndefinedRenderFormat:
                break;
        }

        // Throw exception
        throw snowcrash::Error("unknown content type for messageBody to be rendered", snowcrash::ApplicationError);
    }

    NodeInfoByValue<Asset> renderPayloadSchema(const NodeInfo<snowcrash::Payload>& payload,
                                               const NodeInfo<snowcrash::Action>& action,
                                               ConversionContext& context) {

        NodeInfoByValue<Asset> schema = std::make_pair(payload.node->schema, &payload.sourceMap->schema);

        NodeInfo<Attributes> payloadAttributes = MAKE_NODE_INFO(payload, attributes);
        NodeInfo<Attributes> actionAttributes = MAKE_NODE_INFO(action, attributes);

        // hold attributes via pointer - because problems with assignment in NodeInfo<>
        NodeInfo<Attributes>* attributes = &payloadAttributes;

        if (payload.node->attributes.empty() && !action.isNull() && !action.node->attributes.empty()) {
            attributes = &actionAttributes;
        }

        // Generate Schema only if Body content type is JSON
        if (!payload.node->schema.empty() || payload.node->attributes.empty() ||
            findRenderFormat(getContentTypeFromHeaders(payload.node->headers)) != JSONRenderFormat) {

            return schema;
        }

        refract::JSONSchemaVisitor renderer;
        refract::IElement* element = MSONToRefract(*attributes, context);

        if (!element) {
            return schema;
        }

        refract::IElement* expanded = ExpandRefract(element, context);

        if (!expanded) {
            return schema;
        }

        std::string result = renderer.getSchema(*expanded);
        delete expanded;

        return std::make_pair(result, NodeInfo<Asset>::NullSourceMap());
    }
}
