//
//  refract/dsd/Option.cc
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "Option.h"

#include <algorithm>
#include <cassert>

#include "Traits.h"
#include "../Utils.h"

using namespace refract;
using namespace dsd;

const char* Option::name = "option";

static_assert(supports_erase<Option>::value, "");
static_assert(supports_empty<Option>::value, "");
static_assert(supports_insert<Option>::value, "");
static_assert(supports_push_back<Option>::value, "");
static_assert(supports_begin<Option>::value, "");
static_assert(supports_end<Option>::value, "");
static_assert(supports_size<Option>::value, "");
static_assert(supports_erase<Option>::value, "");
static_assert(is_iterable<Option>::value, "");

static_assert(!supports_key<Option>::value, "");
static_assert(!supports_value<Option>::value, "");
static_assert(!supports_merge<Option>::value, "");
static_assert(!is_pair<Option>::value, "");

Option::Option() : elements_() {}

Option::Option(Option&& other) : Option()
{
    swap(*this, other);
}

Option::Option(const Option& other) : elements_()
{
    elements_.reserve(other.elements_.size());
    std::transform(other.elements_.begin(),
        other.elements_.end(),
        std::back_inserter(elements_),
        [](const value_type& el) -> std::unique_ptr<IElement> {
            assert(el);
            return el->clone();
        });
}

Option& Option::operator=(Option rhs)
{
    swap(*this, rhs);
    return *this;
}

Option::iterator Option::insert(Option::iterator it, std::unique_ptr<IElement> el)
{
    assert(it >= begin());
    assert(it <= end());
    assert(el);

    return elements_.insert(it, std::move(el));
}

Option::iterator Option::erase(Option::iterator b, Option::iterator e)
{
    return elements_.erase(b, e);
}

bool dsd::operator==(const Option& lhs, const Option& rhs) noexcept
{
    return lhs.size() == rhs.size()
        && std::equal( //
               lhs.begin(),
               lhs.end(),
               rhs.begin(),
               [](const Option::value_type& l, const Option::value_type& r) { return *l == *r; });
}

bool dsd::operator!=(const Option& lhs, const Option& rhs) noexcept
{
    return !(lhs == rhs);
}
