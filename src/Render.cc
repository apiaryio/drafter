//
//  Render.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 18/08/15.
//  Copyright (c) 2015 Apiary. All rights reserved.
//

#include "RefractDataStructure.h"
#include "Render.h"

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

        if (RegexMatch(contentType, JSONSchemaRegex)) {
            return JSONSchemaRenderFormat;
        } else if (RegexMatch(contentType, JSONRegex)) {
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

    NodeInfoByValue<Asset> renderPayloadSchema(const NodeInfo<snowcrash::Payload>& payload,
        const NodeInfo<snowcrash::Action>& action,
        ConversionContext& context)
    {
        LOG(debug) << "renderPayloadSchema";

        auto schema = NodeInfoByValue<Asset>{ payload.node->schema, &payload.sourceMap->schema };

        NodeInfo<Attributes> payloadAttributes = MAKE_NODE_INFO(payload, attributes);
        NodeInfo<Attributes> actionAttributes = MAKE_NODE_INFO(action, attributes);

        // hold attributes via pointer - because problems with assignment in NodeInfo<>
        NodeInfo<Attributes>* attributes = &payloadAttributes;

        if (payload.node->attributes.empty() && !action.isNull() && !action.node->attributes.empty()) {
            attributes = &actionAttributes;
        }

        // Generate Schema only if Body content type is JSON
        if (payload.node->schema.empty() && !payload.node->attributes.empty()
            && findRenderFormat(getContentTypeFromHeaders(payload.node->headers)) == JSONRenderFormat) {

            if (auto element = MSONToRefract(*attributes, context)) {

                if (auto expanded = ExpandRefract(std::move(element), context)) {
                    std::stringstream ss{};
                    utils::so::serialize_json(ss, refract::schema::generateJsonSchema(*expanded));

                    schema = NodeInfoByValue<Asset>{ ss.str(), NodeInfo<Asset>::NullSourceMap() };
                }
            }
        }

        return schema;
    }
} // namespace drafter
