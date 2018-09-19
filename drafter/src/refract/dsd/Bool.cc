//
//  refract/dsd/Bool.cc
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "Bool.h"

#include "Traits.h"

using namespace refract;
using namespace dsd;

const char* Boolean::name = "boolean";

static_assert(!supports_erase<Boolean>::value, "");
static_assert(!supports_empty<Boolean>::value, "");
static_assert(!supports_push_back<Boolean>::value, "");
static_assert(!supports_begin<Boolean>::value, "");
static_assert(!supports_end<Boolean>::value, "");
static_assert(!supports_size<Boolean>::value, "");
static_assert(!supports_erase<Boolean>::value, "");
static_assert(!supports_key<Boolean>::value, "");
static_assert(!supports_value<Boolean>::value, "");
static_assert(!supports_merge<Boolean>::value, "");
static_assert(!is_iterable<Boolean>::value, "");
static_assert(!is_pair<Boolean>::value, "");

bool dsd::operator==(const Boolean& l, const Boolean& r) noexcept
{
    return l.get() == r.get();
}
