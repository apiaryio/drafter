//
//  refract/dsd/Object.cc
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "Object.h"

#include <algorithm>
#include "Traits.h"
#include "../Element.h"
#include "../Utils.h"

using namespace refract;
using namespace dsd;

const char* Object::name = "object";

static_assert(supports_erase<Object>::value, "");
static_assert(supports_empty<Object>::value, "");
static_assert(supports_insert<Object>::value, "");
static_assert(supports_push_back<Object>::value, "");
static_assert(supports_begin<Object>::value, "");
static_assert(supports_end<Object>::value, "");
static_assert(supports_size<Object>::value, "");
static_assert(supports_erase<Object>::value, "");
static_assert(is_iterable<Object>::value, "");

static_assert(!supports_key<Object>::value, "");
static_assert(!supports_value<Object>::value, "");
static_assert(!supports_merge<Object>::value, "");
static_assert(!is_pair<Object>::value, "");

Object::Object(const Object& other) : elements_()
{
    elements_.reserve(other.elements_.size());
    std::transform(other.elements_.begin(),
        other.elements_.end(),
        std::back_inserter(elements_),
        [](const auto& el) -> std::unique_ptr<IElement> {
            if (!el)
                return nullptr;
            return el->clone();
        });
}

Object& Object::operator=(const Object& rhs)
{
    Object a(rhs);
    std::swap(a, *this);
    return *this;
}

Object::iterator Object::insert(Object::const_iterator it, std::unique_ptr<IElement> el)
{
    assert(it >= begin());
    assert(it <= end());
    assert(el);

    return elements_.insert(it, std::move(el));
}

Object::iterator Object::erase(Object::const_iterator b, Object::const_iterator e)
{
    return elements_.erase(b, e);
}

Object::iterator Object::addMember(std::string name, std::unique_ptr<IElement> value)
{
    return insert(end(), make_element<MemberElement>(name, std::move(value)));
}

bool dsd::operator==(const Object& lhs, const Object& rhs) noexcept
{
    return std::equal( //
        lhs.begin(),
        lhs.end(),
        rhs.begin(),
        rhs.end(),
        [](const auto& l, const auto& r) { return *l == *r; });
}

bool dsd::operator!=(const Object& lhs, const Object& rhs) noexcept
{
    return !(lhs == rhs);
}
