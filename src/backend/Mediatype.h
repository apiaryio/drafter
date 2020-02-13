//
//  backend/Mediatype.h
//  apib::backend
//
//  Created by Thomas Jandecka on 02/21/2020
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//

#ifndef APIB_BACKEND_MEDIA_TYPE_H
#define APIB_BACKEND_MEDIA_TYPE_H

#include <ostream>

#include <parser/Mediatype.h>

namespace apib
{
    namespace backend
    {
        std::ostream& operator<<(std::ostream&, const apib::parser::mediatype::state&);
    }
}
#endif
