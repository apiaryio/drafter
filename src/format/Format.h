//
//  backend/Format.h
//  apib::format
//
//  Created by Thomas Jandecka on 02/23/2020
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//

#ifndef APIB_FORMAT_H
#define APIB_FORMAT_H

#include <sstream>

namespace apib
{
    namespace format
    {
        template <typename T>
        std::string serialize(const T& obj)
        {
            std::ostringstream s;
            s << obj;
            return s.str();
        }
    }
}
#endif
