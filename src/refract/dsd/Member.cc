//
//  refract/dsd/Member.cc
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "Member.h"

#include "Traits.h"
#include "../ElementFwd.h"
#include "../Element.h"
#include "String.h"

using namespace refract;
using namespace dsd;

const char* Member::name = "member";

static_assert(!supports_erase<Member>::value, "");
static_assert(!supports_empty<Member>::value, "");
static_assert(!supports_insert<Member>::value, "");
static_assert(!supports_push_back<Member>::value, "");
static_assert(!supports_begin<Member>::value, "");
static_assert(!supports_end<Member>::value, "");
static_assert(!supports_size<Member>::value, "");
static_assert(!supports_erase<Member>::value, "");
static_assert(!supports_merge<Member>::value, "");
static_assert(!is_iterable<Member>::value, "");
static_assert(supports_key<Member>::value, "");
static_assert(supports_value<Member>::value, "");
static_assert(is_pair<Member>::value, "");

Member::Member() : key_(), value_() {}

Member::Member(Member&& other) noexcept : Member()
{
    swap(*this, other);
}

Member::Member(const Member& other)
    : key_(other.key_ ? clone(*other.key_) : nullptr), value_(other.value_ ? clone(*other.value_) : nullptr)
{
}

Member::Member(std::unique_ptr<IElement> key, std::unique_ptr<IElement> value)
    : key_(std::move(key)), value_(std::move(value))
{
    assert(key_);
}

Member::Member(const std::string& key, std::unique_ptr<IElement> value)
    : key_(make_element<StringElement>(key)), value_(std::move(value))
{
}

Member& Member::operator=(Member rhs)
{
    swap(*this, rhs);
    return *this;
}

bool dsd::operator==(const Member& lhs, const Member& rhs) noexcept
{
    auto eq = [](const IElement* lptr, const IElement* rptr) {
        return (lptr == rptr) || (lptr && rptr && (*lptr == *rptr));
    };
    return eq(lhs.key(), rhs.key()) && eq(lhs.value(), rhs.value());
}

bool dsd::operator!=(const Member& lhs, const Member& rhs) noexcept
{
    return !(lhs == rhs);
}
