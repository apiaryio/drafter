//
//  format/MediatypeFmt.h
//  apib::format
//
//  Created by Thomas Jandecka on 02/21/2020
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//

#ifndef APIB_BACKEND_MEDIA_TYPE_H
#define APIB_BACKEND_MEDIA_TYPE_H

#include <ostream>

namespace apib
{
    // FIXME: avoid depending on parser from format by isolating MediaType
    //          definition; then forward declare as, e. g., `apib::mediatype`
    namespace parser
    {
        namespace mediatype
        {
            struct state;
        }
    }

    namespace format
    {
        std::ostream& operator<<(std::ostream&, const apib::parser::mediatype::state&);
    }
}
#endif
