//
//  refract/dsd/Null.cc
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "Null.h"

#include "Traits.h"

using namespace refract;
using namespace dsd;

const char* Null::name = "null";

static_assert(!supports_erase<Null>::value, "");
static_assert(!supports_empty<Null>::value, "");
static_assert(!supports_insert<Null>::value, "");
static_assert(!supports_push_back<Null>::value, "");
static_assert(!supports_begin<Null>::value, "");
static_assert(!supports_end<Null>::value, "");
static_assert(!supports_size<Null>::value, "");
static_assert(!supports_erase<Null>::value, "");
static_assert(!supports_key<Null>::value, "");
static_assert(!supports_value<Null>::value, "");
static_assert(!supports_merge<Null>::value, "");
static_assert(!is_iterable<Null>::value, "");
static_assert(!is_pair<Null>::value, "");

bool dsd::operator==(const Null&, const Null&) noexcept
{
    return true;
}
