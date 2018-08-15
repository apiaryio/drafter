//
//  refract/dsd/Number.cc
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "Number.h"

#include "Traits.h"
#include "../../utils/log/Trivial.h"

using namespace refract;
using namespace dsd;
using namespace drafter::utils::log;

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

Number::Number(std::string v) noexcept : value_(v) {}

const std::string& Number::get() const noexcept
{
    return value_;
}

Number::operator std::int64_t() const noexcept
{
    std::string::size_type idx;
    std::int64_t result = std::stoi(value_, &idx);
    if (idx < value_.size())
        LOG(warning) << "dsd::Number to int; dropped trailing `" << value_.substr(idx) << "`";
    return result;
}

bool dsd::operator==(const Number& lhs, const Number& rhs) noexcept
{
    return lhs.get() == rhs.get();
}

bool dsd::operator!=(const Number& lhs, const Number& rhs) noexcept
{
    return !(lhs == rhs);
}
