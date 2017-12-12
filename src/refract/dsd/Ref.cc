//
//  refract/dsd/Ref.cc
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "Ref.h"

#include "Traits.h"

using namespace refract;
using namespace dsd;

const char* Ref::name = "ref";

static_assert(!supports_erase<Ref>::value, "");
static_assert(!supports_empty<Ref>::value, "");
static_assert(!supports_insert<Ref>::value, "");
static_assert(!supports_push_back<Ref>::value, "");
static_assert(!supports_begin<Ref>::value, "");
static_assert(!supports_end<Ref>::value, "");
static_assert(!supports_size<Ref>::value, "");
static_assert(!supports_erase<Ref>::value, "");
static_assert(!is_iterable<Ref>::value, "");
static_assert(!supports_key<Ref>::value, "");
static_assert(!supports_value<Ref>::value, "");
static_assert(!supports_merge<Ref>::value, "");
static_assert(!is_pair<Ref>::value, "");

Ref::Ref(std::string symbol) : data_(std::move(symbol)) {}

bool dsd::operator==(const Ref& lhs, const Ref& rhs) noexcept
{
    return lhs.symbol() == rhs.symbol();
}

bool dsd::operator!=(const Ref& lhs, const Ref& rhs) noexcept
{
    return !(lhs == rhs);
}
