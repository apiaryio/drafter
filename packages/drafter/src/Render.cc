//
//  Render.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 18/08/15.
//  Copyright (c) 2015 Apiary. All rights reserved.
//

#include "Render.h"

#include <apib/parser/MediaTypeParser.h>
#include "RefractDataStructure.h"

#include "BlueprintUtility.h"
#include "RegexMatch.h"
#include "SourceAnnotation.h"

#include "ConversionContext.h"

#include "refract/JsonSchema.h"
#include "refract/JsonValue.h"
#include "refract/SerializeSo.h"

#include "utils/log/Trivial.h"
#include "utils/so/JsonIo.h"

using namespace snowcrash;
using namespace drafter::utils::log;

namespace drafter
{

    RenderFormat findRenderFormat(const std::string& contentType)
    {

        const auto mediaType = apib::parser::parseMediaType(contentType);

        if (apib::isJSONSchema(mediaType)) {
            return JSONSchemaRenderFormat;
        } else if (apib::isJSON(mediaType)) {
            return JSONRenderFormat;
        }

        return UndefinedRenderFormat;
    }

    std::string getContentTypeFromHeaders(const Headers& headers)
    {
        Collection<Header>::const_iterator header;

        header = std::find_if(headers.begin(),
            headers.end(),
            std::bind(MatchFirstWith<Header, std::string>(), std::placeholders::_1, HTTPHeaderName::ContentType));

        if (header != headers.end()) {
            return header->second;
        }

        return "";
    }
} // namespace drafter
