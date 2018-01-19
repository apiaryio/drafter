//
//  utils/so/JsonValue.h
//  librefract
//
//  Created by Thomas Jandecka on 17/01/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_UTILS_SO_JSONVALUE_H
#define DRAFTER_UTILS_SO_JSONVALUE_H

#include "Value.h"

namespace drafter
{
    namespace utils
    {
        namespace so
        {
            struct JsonValue {
                Value value;
                explicit JsonValue(Value v) : value(std::move(v)) {}
            };
        }
    }
}

#endif
