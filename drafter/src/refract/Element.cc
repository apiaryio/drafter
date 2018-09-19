//
//  refract/Element.cc
//  librefract
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#include "Element.h"
#include <cassert>

#include <set>
#include <map>
#include <string>
#include <cstring>
#include <array>

#include "ComparableVisitor.h"
#include "TypeQueryVisitor.h"

using namespace refract;

namespace
{
    constexpr std::array<const char*, 13> reserved_ = {
        "array",   //
        "boolean", //
        "enum",    //
        "extend",  //
        "generic", //
        "member",  //
        "null",    //
        "number",  //
        "object",  //
        "option",  //
        "ref",     //
        "select",  //
        "string",  //
    };
}

bool refract::isReserved(const char* w) noexcept
{
    assert(w);
    return std::binary_search(reserved_.begin(), reserved_.end(), w, [](const char* first, const char* second) {
        return (0 > std::strcmp(first, second));
    });
}

bool refract::isReserved(const std::string& w) noexcept
{
    return isReserved(w.c_str());
}
