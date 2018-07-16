//
//  refract/JsonValue.h
//  librefract
//
//  Created by Thomas Jandecka on 12/07/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_JSON_VALUE_H
#define REFRACT_JSON_VALUE_H

#include "../utils/so/Value.h"

#include "ElementIfc.h"

namespace refract
{
    drafter::utils::so::Value generateJsonValue(const IElement& el);
} // namespace refract

#endif
