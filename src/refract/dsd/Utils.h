//
//  refract/dsd/Utils.h
//  librefract
//
//  Created by Thomas Jandecka on 07/11/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_DSD_UTILS_H
#define REFRACT_DSD_UTILS_H

#include <memory>

namespace refract
{
    namespace utils
    {
        template <typename Container>
        void move_back(Container&) noexcept
        {
            // noop
        }

        template <typename Container, typename Arg, typename... Args>
        void move_back(Container& c, Arg&& arg, Args&&... args)
        {
            c.emplace_back(std::forward<Arg>(arg));
            move_back(c, std::forward<Args>(args)...);
        }
    }
}

#endif
