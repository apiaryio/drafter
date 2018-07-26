//
//  refract/Asset.h
//  librefract
//
//  Created by Thomas Jandecka on 20/07/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_ASSET_H
#define REFRACT_ASSET_H

#include "Element.h"

namespace refract
{
    std::unique_ptr<StringElement> generateJsonValueAsset(
        const IElement& expanded, const std::string& mime = "application/json");

    std::unique_ptr<StringElement> generateJsonSchemaAsset(
        const IElement& expanded, const std::string& mime = "application/schema+json");

} // namespace refract

#endif
