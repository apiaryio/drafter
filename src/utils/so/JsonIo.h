//
//  utils/so/JsonIo.h
//  librefract
//
//  Created by Thomas Jandecka on 23/01/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_UTILS_SO_JSONIO_H
#define DRAFTER_UTILS_SO_JSONIO_H

#include "Value.h"

namespace drafter
{
    namespace utils
    {
        namespace so
        {
            struct packed {
            };

            std::ostream& serialize_json(std::ostream& out, const Value& obj);
            std::ostream& serialize_json(std::ostream& out, const Value& obj, packed);
        }
    }
}
#endif
