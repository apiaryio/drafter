//
//  Render.h
//  drafter
//
//  Created by Pavan Kumar Sunkara on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_RENDER_H
#define DRAFTER_RENDER_H

#include "Serialize.h"

namespace drafter {

    const char* const JSONSchemaContentType = "application/schema+json";
    const char* const JSONRegex = "^[[:blank:]]*application/(.*\\+)?json[[:blank:]]*(;.*|$)";
    const char* const JSONSchemaRegex = "^[[:blank:]]*application/schema\\+json[[:blank:]]*(;.*|$)";

    enum RenderFormat {
        UndefinedRenderFormat = 0,   // Undefined format
        JSONRenderFormat = 1,        // JSON format
        JSONSchemaRenderFormat = 2   // JSON Schema format
    };

    class ConversionContext;

    RenderFormat findRenderFormat(const std::string& contentType);
    std::string getContentTypeFromHeaders(const snowcrash::Headers& headers);

    NodeInfoByValue<snowcrash::Asset> renderPayloadBody(const NodeInfo<snowcrash::Payload>& payload,
                                                        const NodeInfo<snowcrash::Action>& action,
                                                        ConversionContext& context);

    NodeInfoByValue<snowcrash::Asset> renderPayloadSchema(const NodeInfo<snowcrash::Payload>& payload,
                                                          const NodeInfo<snowcrash::Action>& action,
                                                          ConversionContext& context);
}

#endif
