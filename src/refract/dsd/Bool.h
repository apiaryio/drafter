//
//  refract/dsd/Bool.h
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_DSD_BOOL_H
#define REFRACT_DSD_BOOL_H

namespace refract
{
    namespace dsd
    {
        ///
        /// Data structure definition (DSD) of a Refract Bool Element
        ///
        /// @remark Defined by its value
        /// TODO move value to attributes and leave this defined by its type
        ///
        class Boolean final
        {
            bool data_ = false; // literal value

        public:
            static const char* name; //< syntactical name of the DSD

        public:
            ///
            /// Initialize a Bool DSD with literal `false`
            ///
            Boolean() = default;
            //
            ///
            /// Initialize a Bool DSD with custom literal
            ///
            explicit Boolean(bool v) noexcept : data_(v) {}
            Boolean(int v) = delete; // avoid construction from integer

        public:
            ///
            /// Get the value of this Bool DSD
            ///
            /// @return the value
            ///
            bool get() const noexcept
            {
                return data_;
            }

        public:
            ///
            /// Implicitly convert this DSD to its value
            ///
            /// @returns this DSD's value
            ///
            operator bool() const noexcept
            {
                return data_;
            }
        };

        bool operator==(const Boolean&, const Boolean&) noexcept;
    }
}

#endif
