//
//  refract/JsonSchema.h
//  librefract
//
//  Created by Thomas Jandecka on 16/02/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_JSON_SCHEMA_H
#define REFRACT_JSON_SCHEMA_H

#include "../utils/so/Value.h"

#include "ElementIfc.h"

namespace refract
{
    namespace schema
    {
        drafter::utils::so::Object generateJsonSchema(const IElement& el);
    }
}

#endif
