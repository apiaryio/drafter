//
//  refract/dsd/Holder.cc
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "Holder.h"

#include <cassert>
#include "Traits.h"
#include "../Utils.h"

using namespace refract;
using namespace dsd;

const char* Holder::name = "";

static_assert(!supports_erase<Holder>::value, "");
static_assert(!supports_empty<Holder>::value, "");
static_assert(!supports_insert<Holder>::value, "");
static_assert(!supports_push_back<Holder>::value, "");
static_assert(!supports_begin<Holder>::value, "");
static_assert(!supports_end<Holder>::value, "");
static_assert(!supports_size<Holder>::value, "");
static_assert(!supports_erase<Holder>::value, "");
static_assert(!supports_merge<Holder>::value, "");
static_assert(!is_iterable<Holder>::value, "");
static_assert(!supports_key<Holder>::value, "");
static_assert(!supports_value<Holder>::value, "");
static_assert(!is_pair<Holder>::value, "");

Holder::Holder() : data_() {}

Holder::Holder(Holder&& other) : Holder()
{
    swap(*this, other);
}

Holder::Holder(const Holder& other) : data_(other.data_ ? other.data_->clone() : nullptr) {}

Holder::Holder(std::unique_ptr<IElement> el) : data_(std::move(el))
{
    assert(data_);
}

Holder& Holder::operator=(Holder rhs)
{
    swap(*this, rhs);
    return *this;
}

bool dsd::operator==(const Holder& l, const Holder& r) noexcept
{
    return *l.data() == *r.data();
}
