//
//  utils/so/YamlValue.h
//  librefract
//
//  Created by Thomas Jandecka on 17/01/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_UTILS_SO_YAMLVALUE_H
#define DRAFTER_UTILS_SO_YAMLVALUE_H

#include "Value.h"

namespace drafter
{
    namespace utils
    {
        namespace so
        {
            struct YamlValue {
                Value value;
                explicit YamlValue(Value v) : value(std::move(v)) {}
            };
        }
    }
}

#endif
