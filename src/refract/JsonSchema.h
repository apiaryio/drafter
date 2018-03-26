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

        using PassSchema = drafter::utils::so::True;
        using FailSchema = drafter::utils::so::False;

        using Boolean = drafter::utils::variant< //
            drafter::utils::so::True,
            drafter::utils::so::False>;

        using Schema = drafter::utils::so::Value;

        Schema generateJsonSchema(const IElement& el);
    }
}

#endif
