//
//  refract/dsd/Number.cc
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "Number.h"

#include "Traits.h"

using namespace refract;
using namespace dsd;

const char* Number::name = "number";

static_assert(!supports_erase<Number>::value, "");
static_assert(!supports_empty<Number>::value, "");
static_assert(!supports_insert<Number>::value, "");
static_assert(!supports_push_back<Number>::value, "");
static_assert(!supports_begin<Number>::value, "");
static_assert(!supports_end<Number>::value, "");
static_assert(!supports_size<Number>::value, "");
static_assert(!supports_erase<Number>::value, "");
static_assert(!supports_key<Number>::value, "");
static_assert(!supports_value<Number>::value, "");
static_assert(!supports_merge<Number>::value, "");
static_assert(!is_iterable<Number>::value, "");
static_assert(!is_pair<Number>::value, "");

bool dsd::operator==(const Number& lhs, const Number& rhs) noexcept
{
    return lhs.get() == rhs.get();
}
