//
//  utils/so/YamlIo.h
//  librefract
//
//  Created by Thomas Jandecka on 07/02/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_UTILS_SO_YAMLIO_H
#define DRAFTER_UTILS_SO_YAMLIO_H

#include "Value.h"

namespace drafter
{
    namespace utils
    {
        namespace so
        {
            std::ostream& serialize_yaml(std::ostream& out, const Value& obj);
        }
    }
}
#endif
