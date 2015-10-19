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

    Asset renderPayloadBody(const SectionInfo<Payload>& payload, const SectionInfo<Action>& action, const refract::Registry& registry) {
        Asset body = payload.section.body;
        RenderFormat renderFormat = findRenderFormat(getContentTypeFromHeaders(payload.section.headers));

        SectionInfo<Attributes> payloadAttributes = MAKE_SECTION_INFO(payload, attributes);
        SectionInfo<Attributes> actionAttributes = MAKE_SECTION_INFO(action, attributes);

        // hold attributes via pointer - because problems with assignment in SectionInfo<>
        SectionInfo<Attributes>* attributes = &payloadAttributes;

        if (payload.section.attributes.empty() && !action.isNull() && !action.section.attributes.empty()) {
           attributes = &actionAttributes;
        }

        if (!payload.section.body.empty() || attributes->section.empty() || renderFormat == UndefinedRenderFormat) {
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

                return renderer.getString();
            }

            default:
                break;
        }

        return body;
    }

    Asset renderPayloadSchema(const SectionInfo<Payload>& payload) {
        Asset schema = payload.section.schema;
        RenderFormat renderFormat = JSONSchemaRenderFormat;

        if (!payload.section.schema.empty() || payload.section.attributes.empty() || renderFormat == UndefinedRenderFormat) {
            return schema;
        }

        return schema;
    }
}
