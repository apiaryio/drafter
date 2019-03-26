//
//  refract/JsonUtils.h
//  librefract
//
//  Created by Thomas Jandecka on 16/08/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_JSON_UTILS_H
#define REFRACT_JSON_UTILS_H

#include "../utils/so/Value.h"
#include "ElementFwd.h"

namespace refract
{
    namespace utils
    {
        drafter::utils::so::String instantiate(const dsd::String& e);
        drafter::utils::so::Number instantiate(const dsd::Number& e);
        drafter::utils::so::Value instantiate(const dsd::Boolean& e);

        drafter::utils::so::String instantiateEmpty(const StringElement& e);
        drafter::utils::so::Number instantiateEmpty(const NumberElement& e);
        drafter::utils::so::Value instantiateEmpty(const BooleanElement& e);
    } // namespace utils
} // namespace refract

#endif
