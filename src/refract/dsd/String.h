//
//  refract/dsd/String.h
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_DSD_STRING_H
#define REFRACT_DSD_STRING_H

#include <string>

namespace refract
{
    namespace dsd
    {
        ///
        /// Data structure definition (DSD) of a Refract String Element
        ///
        /// @remark Defined by its value
        /// TODO move value to attributes and leave this defined by its type
        ///
        class String final
        {
            std::string value_ = {}; //< value

        public:
            static const char* name; //< syntactical name of the DSD

        public:
            ///
            /// Initialize a String DSD with empty value
            ///
            String() = default;

            ///
            /// Initialize a String DSD from another by consuming its value
            ///
            /// @param other   String DSD to be consumed
            ///
            String(String&& other) = default;

            ///
            /// Initialize a String DSD from another by copying its value
            ///
            /// @param other   String DSD to be cloned from
            ///
            String(const String& other) = default;

            ///
            /// Initialize a String DSD from a value
            ///
            /// @value  value to be consumed
            ///
            String(std::string value) noexcept;

            ///
            /// Consume another String DSD's value
            ///
            /// @param other   String DSD to be consumed
            ///
            String& operator=(String&&) = default;

            ///
            /// Copy another String DSD's value
            ///
            /// @param rhs   String DSD to be copied from
            ///
            String& operator=(const String& rhs) = default;

        public:
            ///
            /// Check whether a String DSD is equal to a value
            ///
            /// @returns true iff the values of the DSD equals rhs
            ///
            friend bool operator==(const String& lhs, const std::string& rhs) noexcept
            {
                return lhs.value_ == rhs;
            }

        public:
            ///
            /// Query the value of this String DSD
            ///
            /// @returns the value
            ///
            const std::string& get() const noexcept
            {
                return value_;
            }

            ///
            /// Query whether this String DSD is empty
            ///
            /// @returns true iff the value is empty
            ///
            bool empty() const noexcept
            {
                return value_.empty();
            }

            ///
            /// Implicitly convert this DSD to its value
            ///
            /// @returns this DSD's value
            ///
            operator const std::string&() const
            {
                return value_;
            }
        };

        bool operator==(const String&, const String&) noexcept;
        bool operator!=(const String&, const String&) noexcept;
    }
}

#endif
