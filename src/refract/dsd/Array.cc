//
//  refract/dsd/Array.cc
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "Array.h"

#include <cassert>
#include "../Utils.h"

using namespace refract;
using namespace dsd;

const char* Array::name = "array";

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

Array::Array() : elements_() {}

Array& Array::operator=(Array rhs)
{
    swap(*this, rhs);
    return *this;
}

Array::Array(Array&& other) : Array()
{
    swap(*this, other);
}

Array::Array(const Array& other) : Array()
{
    elements_.reserve(other.elements_.size());
    std::transform(other.elements_.begin(),
        other.elements_.end(),
        std::back_inserter(elements_),
        [](const value_type& el) -> std::unique_ptr<IElement> {
            assert(el);
            return std::unique_ptr<IElement>(el->clone());
        });
}

Array::iterator Array::insert(Array::iterator it, std::unique_ptr<IElement> el)
{
    assert(it >= begin());
    assert(it <= end());
    assert(el);

    return elements_.insert(it, std::move(el));
}

Array::iterator Array::erase(Array::iterator b, Array::iterator e)
{
    return elements_.erase(b, e);
}

bool dsd::operator==(const Array& lhs, const Array& rhs) noexcept
{
    return lhs.size() == rhs.size()
        && std::equal( //
               lhs.begin(),
               lhs.end(),
               rhs.begin(),
               [](const Array::value_type& l, const Array::value_type& r) {
                   assert(l);
                   assert(r);
                   return *l == *r;
               });
}

bool dsd::operator!=(const Array& lhs, const Array& rhs) noexcept
{
    return !(lhs == rhs);
}
