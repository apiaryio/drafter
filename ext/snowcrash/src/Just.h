//
//  Just.h
//  snowcrash
//
//  Created by Thomas Jandecka on 11/7/19.
//  Copyright (c) 2019 Apiary Inc. All rights reserved.
//

#ifndef SNOWCRASH_JUST_H
#define SNOWCRASH_JUST_H

#include <utility>

namespace mson
{
    template <typename T, typename>
    struct just {

        T content;

        just() noexcept(T()) : content{} {}

        explicit just(T&& c) noexcept : content{ std::move(c) } {}
        explicit just(const T& c) : content{ c } {}

        const T* operator->() const noexcept
        {
            return &content;
        }

        T* operator->() noexcept
        {
            return &content;
        }

        const T& operator*() const noexcept
        {
            return content;
        }

        T& operator*() noexcept
        {
            return content;
        }
    };
}

#endif
