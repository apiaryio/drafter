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

    Asset renderPayloadBody(const Payload& payload, const refract::Registry& registry) {
        Asset body = payload.body;
        RenderFormat renderFormat = findRenderFormat(getContentTypeFromHeaders(payload.headers));

        if (!payload.body.empty() || payload.attributes.empty() || renderFormat == UndefinedRenderFormat) {
            return body;
        }

        switch (renderFormat) {
            case JSONRenderFormat:
            {
                refract::RenderJSONVisitor renderer;
                refract::IElement* element = MSONToRefract(payload.attributes);
                refract::IElement* expanded = ExpandRefract(element, registry);

                renderer.visit(*expanded);
                delete expanded;

                return renderer.getString();
            }

            default:
                break;
        }

        return body;
    }

    Asset renderPayloadSchema(const Payload& payload) {
        Asset schema = payload.schema;
        RenderFormat renderFormat = JSONSchemaRenderFormat;

        if (!payload.schema.empty() || payload.attributes.empty() || renderFormat == UndefinedRenderFormat) {
            return schema;
        }

        return schema;
    }
}
