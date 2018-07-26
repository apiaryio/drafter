//
//  refract/Asset.cc
//  librefract
//
//  Created by Thomas Jandecka on 20/07/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "Asset.h"

#include "../utils/so/JsonIo.h"
#include "JsonSchema.h"
#include "JsonValue.h"
#include "../Serialize.h"
#include <sstream>

using namespace drafter::utils::so;
using namespace drafter;
using namespace refract::schema;
using namespace refract;

std::unique_ptr<StringElement> refract::generateJsonValueAsset(const IElement& expanded, const std::string& mime)
{
    std::ostringstream ss;
    serialize_json(ss, generateJsonValue(expanded));

    auto asset = from_primitive(ss.str());
    asset->element(SerializeKey::Asset);

    asset->meta().set(SerializeKey::Classes,          //
        make_element<ArrayElement>(                   //
            from_primitive(SerializeKey::MessageBody) //
            )                                         //
    );                                                //

    if (!mime.empty())
    asset->attributes().set(SerializeKey::ContentType, //
        from_primitive(mime));

    return asset;
}

std::unique_ptr<StringElement> refract::generateJsonSchemaAsset(const IElement& expanded, const std::string& mime)
{
    std::ostringstream ss;
    serialize_json(ss, generateJsonSchema(expanded));

    auto asset = from_primitive(ss.str());
    asset->element(SerializeKey::Asset);

    asset->meta().set(SerializeKey::Classes,                //
        make_element<ArrayElement>(                         //
            from_primitive(SerializeKey::MessageBodySchema) //
            )                                               //
    );                                                      //

    if (!mime.empty())
        asset->attributes().set(SerializeKey::ContentType, //
            from_primitive(mime)                           //
        );

    return asset;
}
