//
//  refract/Cardinal.h
//  librefract
//
//  Created by Thomas Jandecka on 21/03/2019
//  Copyright (c) 2019 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_REFRACT_CARDINAL_H
#define DRAFTER_REFRACT_CARDINAL_H

#include <limits>

namespace refract
{
    class cardinal
    {
    public:
        using value_type = unsigned;

    private:
        value_type data_ = 0;

    public:
        constexpr cardinal() noexcept = default;
        explicit constexpr cardinal(value_type d) noexcept : data_{ d } {}

        constexpr value_type data() const noexcept
        {
            return data_;
        }

    public:
        static constexpr cardinal open() noexcept
        {
            return cardinal{ std::numeric_limits<value_type>::max() };
        }

        static constexpr cardinal empty() noexcept
        {
            return cardinal{ 0 };
        }
    };

    constexpr bool operator==(cardinal lhs, cardinal rhs) noexcept
    {
        return lhs.data() == rhs.data();
    }

    constexpr bool operator!=(cardinal lhs, cardinal rhs) noexcept
    {
        return lhs.data() != rhs.data();
    }

    constexpr bool finite(cardinal a) noexcept
    {
        return a != cardinal::open();
    }

    constexpr cardinal operator+(cardinal lhs, cardinal rhs) noexcept
    {
        return (lhs == cardinal::open() || rhs == cardinal::open()) ? cardinal::open() :
                                                                      cardinal{ lhs.data() + rhs.data() };
    }

    constexpr cardinal operator*(cardinal lhs, cardinal rhs)noexcept
    {
        return (lhs == cardinal::empty() || rhs == cardinal::empty()) ?
            cardinal::empty() :
            ((lhs == cardinal::open() || rhs == cardinal::open()) ? cardinal::open() :
                                                                    cardinal{ lhs.data() * rhs.data() });
    }
}

#endif
