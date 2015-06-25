//
//  Render.h
//  libdrafter
//
//  Created by Pavan Kumar Sunkara on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_RENDER_H
#define DRAFTER_RENDER_H

#include <string>
#include "BlueprintSourcemap.h"
#include "BlueprintUtility.h"
#include "RenderJSONVisitor.h"

using namespace snowcrash;

namespace drafter {

    enum RenderFormat {
        UndefinedRenderFormat = 0,   // Undefined format
        JSONRenderFormat = 1,        // JSON format
        JSONSchemaRenderFormat = 2   // JSON Schema format (not used yet)
    };

    RenderFormat findRenderFormat(std::string contentType) {

        if (contentType == "application/json") {
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

    Asset renderPayloadBody(const Payload& payload) {
        Asset body = payload.body;
        RenderFormat renderFormat = findRenderFormat(getContentTypeFromHeaders(payload.headers));

        if (!payload.body.empty() || payload.attributes.empty() || renderFormat == UndefinedRenderFormat) {
            return body;
        }

#if _WITH_REFRACT_
        switch (renderFormat) {
            case JSONRenderFormat:
            {
                RenderJSONVisitor renderer;
                refract::IElement* element = DataStructureToRefract(payload.attributes);

                renderer.visit(*element);
                return renderer.get();
            }

            default:
                break;
        }
#endif

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

#endif