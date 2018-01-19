//
//  utils/so/ValueIo.h
//  librefract
//
//  Created by Thomas Jandecka on 23/01/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_UTILS_SO_VALUEIO_H
#define DRAFTER_UTILS_SO_VALUEIO_H

#include "JsonValue.h"
#include "YamlValue.h"

namespace drafter
{
    namespace utils
    {
        namespace so
        {
            std::ostream& operator<<(std::ostream& out, const JsonValue& obj);
            std::ostream& operator<<(std::ostream& out, const YamlValue& obj);
        }
    }
}
#endif
