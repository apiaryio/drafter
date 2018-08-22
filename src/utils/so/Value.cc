//
//  utils/so/Value.cc
//  librefract
//
//  Created by Thomas Jandecka on 22/08/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "Value.h"

#include <algorithm>

using namespace drafter::utils::so;

Value* drafter::utils::so::find(Object& c, const std::string& key)
{
    auto it = std::find_if( //
        c.data.begin(),
        c.data.end(),
        [&key](const auto& entry) { return entry.first == key; });

    if (it != c.data.end())
        return &it->second;
    return nullptr;
}
