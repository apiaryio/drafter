//
//  refract/dsd/Enum.cc
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "Enum.h"

#include <algorithm>
#include <cassert>
#include "../Utils.h"

using namespace refract;
using namespace dsd;

const char* Enum::name = "enum";

static_assert(!supports_erase<Enum>::value, "");
static_assert(!supports_empty<Enum>::value, "");
static_assert(!supports_insert<Enum>::value, "");
static_assert(!supports_push_back<Enum>::value, "");
static_assert(!supports_begin<Enum>::value, "");
static_assert(!supports_end<Enum>::value, "");
static_assert(!supports_size<Enum>::value, "");
static_assert(!supports_erase<Enum>::value, "");
static_assert(!is_iterable<Enum>::value, "");

static_assert(!supports_key<Enum>::value, "");
static_assert(supports_value<Enum>::value, "");
static_assert(!supports_merge<Enum>::value, "");
static_assert(!is_pair<Enum>::value, "");

Enum::Enum() : value_() {}

Enum::Enum(Enum::Data value) : value_(std::move(value))
{
    assert(value_);
}

Enum::Enum(const Enum& other) : value_(other.value_ ? other.value_->clone() : nullptr) {}

Enum::Enum(Enum&& other) : Enum()
{
    swap(*this, other);
}

Enum& Enum::operator=(Enum rhs)
{
    swap(*this, rhs);
    return *this;
}

bool dsd::operator==(const Enum& l, const Enum& r) noexcept
{
    return *l.value() == *r.value();
}
