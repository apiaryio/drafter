//
//  refract/dsd/Number.h
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_DSD_NUMBER_H
#define REFRACT_DSD_NUMBER_H

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
            double value_ = 0; //< value

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
            explicit Number(double v) noexcept : value_(v) {}

            ///
            /// Query the value of this Number DSD
            ///
            /// @returns the value
            ///
            double get() const noexcept
            {
                return value_;
            }

            ///
            /// Implicitly convert this DSD to its value
            ///
            /// @returns this DSD's value
            ///
            operator double() const noexcept
            {
                return value_;
            }
        };

        bool operator==(const Number&, const Number&) noexcept;
    }
}

#endif
