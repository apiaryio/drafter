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

    static std::string CreateJSONFromAttributes(const snowcrash::Attributes& attributes, const refract::Registry& registry)
    {
        const std::string null;
        refract::RenderJSONVisitor renderer;

        refract::IElement* element = MSONToRefract(MakeNodeInfoWithoutSourceMap(attributes));

        if (!element) {
            return null;
        }

        refract::IElement* expanded = ExpandRefract(element, registry);

        if (!expanded) {
            return null;
        }

        renderer.visit(*expanded);

        delete expanded;

        return renderer.getString();
    }

    NodeInfoByValue<snowcrash::Asset> renderPayloadBody(const NodeInfo<Payload>& payload, const NodeInfo<Action>& action, const refract::Registry& registry) {
        NodeInfoByValue<snowcrash::Asset> body = std::make_pair(payload.node.body, &payload.sourceMap.body);
        RenderFormat renderFormat = findRenderFormat(getContentTypeFromHeaders(payload.node.headers));

        const Attributes* attributes = &payload.node.attributes;

        if (payload.node.attributes.empty() && !action.isNull() && !action.node.attributes.empty()) {
           attributes = &action.node.attributes;
        }

        if (!body.first.empty() || attributes->empty() || renderFormat == UndefinedRenderFormat) {
            return body;
        }

        switch (renderFormat) {
            case JSONRenderFormat:
                return make_pair(CreateJSONFromAttributes(*attributes, registry), &NodeInfo<snowcrash::Asset>::NullSourceMap());

            default:
                break;
        }

        return body;
    }

    Asset renderPayloadSchema(const NodeInfo<Payload>& payload) {
        Asset schema = payload.node.schema;
        RenderFormat renderFormat = JSONSchemaRenderFormat;

        if (!payload.node.schema.empty() || payload.node.attributes.empty() || renderFormat == UndefinedRenderFormat) {
            return schema;
        }

        return schema;
    }
}
