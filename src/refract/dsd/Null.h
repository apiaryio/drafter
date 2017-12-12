//
//  refract/dsd/Null.h
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_DSD_NULL_H
#define REFRACT_DSD_NULL_H

namespace refract
{
    namespace dsd
    {
        ///
        /// Data structure definition (DSD) of a Refract Null Element
        ///
        /// @remark Defined by its type
        ///
        class Null final
        {
        public:
            static const char* name; //< syntactical name of the DSD
        };

        bool operator==(const Null&, const Null&) noexcept;
    }
}

#endif
