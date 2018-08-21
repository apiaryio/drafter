//
//  refract/dsd/Number.h
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_DSD_NUMBER_H
#define REFRACT_DSD_NUMBER_H

#include <string>
#include <cstdint>
#include <type_traits>

namespace refract
{
    namespace dsd
    {
        ///
        /// Data structure definition (DSD) of a Refract Number Element
        ///
        /// @remark Defined by its value
        /// TODO move value to attributes and leave this defined by its type
        ///
        class Number final
        {
            std::string value_ = "0"; //< value

        public:
            static const char* name; //< syntactical name of the DSD

        public:
            ///
            /// Initialize a Number DSD with zero value
            ///
            Number() = default;

            ///
            /// Initialize a Number DSD from a value
            ///
            /// @value  value to be consumed
            ///
            explicit Number(std::string v) noexcept;

            template <typename N, typename = typename std::enable_if<std::is_integral<N>::value>::type>
            explicit Number(N v) noexcept : value_(std::to_string(v))
            {
            }

            ///
            /// Query the value of this Number DSD
            ///
            /// @returns the value
            ///
            const std::string& get() const noexcept;

            ///
            /// Parse this Number DSD as an integer
            ///
            explicit operator std::int64_t() const noexcept;
        };

        bool operator==(const Number&, const Number&) noexcept;
        bool operator!=(const Number&, const Number&) noexcept;
    } // namespace dsd
} // namespace refract

#endif
