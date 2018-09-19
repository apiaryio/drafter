//
//  refract/dsd/String.cc
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "String.h"

#include "Traits.h"

using namespace refract;
using namespace dsd;

const char* String::name = "string";

static_assert(!supports_erase<String>::value, "");
static_assert(!supports_empty<String>::value, "");
static_assert(!supports_insert<String>::value, "");
static_assert(!supports_push_back<String>::value, "");
static_assert(!supports_begin<String>::value, "");
static_assert(!supports_end<String>::value, "");
static_assert(!supports_size<String>::value, "");
static_assert(!supports_erase<String>::value, "");
static_assert(!supports_key<String>::value, "");
static_assert(!supports_value<String>::value, "");
static_assert(!supports_merge<String>::value, "");
static_assert(!is_iterable<String>::value, "");
static_assert(!is_pair<String>::value, "");

String::String(std::string s) noexcept : value_(std::move(s)) {}

bool dsd::operator==(const String& lhs, const String& rhs) noexcept
{
    return lhs.get() == rhs.get();
}

bool dsd::operator!=(const String& lhs, const String& rhs) noexcept
{
    return !(lhs == rhs);
}
