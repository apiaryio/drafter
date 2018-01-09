//
//  refract/dsd/Select.cc
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "Select.h"

#include <algorithm>

#include "Traits.h"
#include "Option.h"
#include "../Element.h"
#include "../Utils.h"

using namespace refract;
using namespace dsd;

const char* Select::name = "select";

static_assert(supports_erase<Array>::value, "");
static_assert(supports_empty<Array>::value, "");
static_assert(supports_insert<Array>::value, "");
static_assert(supports_push_back<Array>::value, "");
static_assert(supports_begin<Array>::value, "");
static_assert(supports_end<Array>::value, "");
static_assert(supports_size<Array>::value, "");
static_assert(supports_erase<Array>::value, "");
static_assert(is_iterable<Array>::value, "");

static_assert(!supports_key<Array>::value, "");
static_assert(!supports_value<Array>::value, "");
static_assert(!supports_merge<Array>::value, "");
static_assert(!is_pair<Array>::value, "");

Select::Select(const Select& other) : elements_()
{
    elements_.reserve(other.elements_.size());
    std::transform(other.elements_.begin(),
        other.elements_.end(),
        std::back_inserter(elements_),
        [](const auto& el) -> std::unique_ptr<OptionElement> {
            if (!el)
                return nullptr;
            return clone(*el);
        });
}

Select& Select::operator=(const Select& rhs)
{
    Select a(rhs);
    std::swap(a, *this);
    return *this;
}

Select::iterator Select::insert(Select::const_iterator it, std::unique_ptr<OptionElement> el)
{
    assert(it >= begin());
    assert(it <= end());
    assert(el);

    return elements_.insert(it, std::move(el));
}

Select::iterator Select::erase(Select::const_iterator b, Select::const_iterator e)
{
    return elements_.erase(b, e);
}

Select::~Select() {}

bool dsd::operator==(const Select& lhs, const Select& rhs) noexcept
{
    return std::equal( //
        lhs.begin(),
        lhs.end(),
        rhs.begin(),
        rhs.end(),
        [](const auto& l, const auto& r) { return *l == *r; });
}

bool dsd::operator!=(const Select& lhs, const Select& rhs) noexcept
{
    return !(lhs == rhs);
}
