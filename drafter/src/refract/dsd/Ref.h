//
//  refract/dsd/Ref.h
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_DSD_REF_H
#define REFRACT_DSD_REF_H

#include <string>

namespace refract
{
    namespace dsd
    {
        ///
        /// Data structure definition (DSD) of a Refract Ref Element
        ///
        /// @remark Defined by a string symbol
        ///
        class Ref final
        {
        public:
            static const char* name; //< syntactical name of the DSD

        private:
            std::string data_ = {}; //< string symbol

        public:
            ///
            /// Initialize a Ref DSD with an empty symbol
            ///
            Ref() = default;

            ///
            /// Initialize a Ref DSD from a string symbol
            ///
            explicit Ref(std::string symbol);

            ///
            /// Query string symbol
            ///
            /// @return the Ref's string symbol
            ///
            const std::string& symbol() const noexcept
            {
                return data_;
            }

            ///
            /// Check whether a Ref Element is equivalent to a string symbol
            ///
            /// @return true iff the underlying string symbols are equivalent
            ///
            friend bool operator==(const Ref& lhs, const std::string& rhs)
            {
                return lhs.data_ == rhs;
            }
        };

        bool operator==(const Ref&, const Ref&) noexcept;
        bool operator!=(const Ref&, const Ref&) noexcept;
    }
}

#endif
