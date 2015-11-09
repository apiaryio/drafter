//
//  Render.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 18/08/15.
//  Copyright (c) 2015 Apiary. All rights reserved.
//

#include "Render.h"
#include "RefractDataStructure.h"
#include "BlueprintUtility.h"
#include "RegexMatch.h"

using namespace snowcrash;

namespace drafter {

    // JSON content-type regex
    const char* const JSONRegex = "^[[:blank:]]*application/(.*\\+)?json";

    RenderFormat findRenderFormat(const std::string& contentType) {

        if (RegexMatch(contentType, JSONRegex)) {
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

    NodeInfo<Asset> renderPayloadBody(const NodeInfo<Payload>& payload, const NodeInfo<Action>& action, const refract::Registry& registry) {
        NodeInfo<Asset> body = MAKE_NODE_INFO(payload, body);
        RenderFormat renderFormat = findRenderFormat(getContentTypeFromHeaders(payload.node.headers));

        NodeInfo<Attributes> payloadAttributes = MAKE_NODE_INFO(payload, attributes);
        NodeInfo<Attributes> actionAttributes = MAKE_NODE_INFO(action, attributes);

        // hold attributes via pointer - because problems with assignment in NodeInfo<>
        NodeInfo<Attributes>* attributes = &payloadAttributes;

        if (payload.node.attributes.empty() && !action.isNull() && !action.node.attributes.empty()) {
           attributes = &actionAttributes;
        }

        if (!payload.node.body.empty() || attributes->node.empty() || renderFormat == UndefinedRenderFormat) {
            return body;
        }

        switch (renderFormat) {
            case JSONRenderFormat:
            {
                refract::RenderJSONVisitor renderer;

                refract::IElement* element = MSONToRefract(*attributes);

                if (!element) {
                    return body;
                }

                refract::IElement* expanded = ExpandRefract(element, registry);

                if (!expanded) {
                    return body;
                }

                renderer.visit(*expanded);

                delete expanded;

                return MakeNodeInfoWithoutSourceMap((Asset) renderer.getString());
            }

            default:
                break;
        }

        return body;
    }

    NodeInfo<Asset> renderPayloadSchema(const NodeInfo<Payload>& payload) {
        NodeInfo<Asset> schema = MAKE_NODE_INFO(payload, schema);
        RenderFormat renderFormat = JSONSchemaRenderFormat;

        if (!payload.node.schema.empty() || payload.node.attributes.empty() || renderFormat == UndefinedRenderFormat) {
            return schema;
        }

        return schema;
    }
}
