//
//  utils/so/Value.cc
//  librefract
//
//  Created by Thomas Jandecka on 22/08/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "Value.h"

#include <algorithm>

using namespace drafter::utils::so;

bool drafter::utils::so::operator==(const Null& lhs, const Null& rhs)
{
    return true;
}

bool drafter::utils::so::operator==(const True& lhs, const True& rhs)
{
    return true;
}

bool drafter::utils::so::operator==(const False& lhs, const False& rhs)
{
    return true;
}

bool drafter::utils::so::operator==(const String& lhs, const String& rhs)
{
    return lhs.data == rhs.data;
}

bool drafter::utils::so::operator==(const Number& lhs, const Number& rhs)
{
    return lhs.data == rhs.data;
}

bool drafter::utils::so::operator==(const Object& lhs, const Object& rhs)
{
    return lhs.data.size() == rhs.data.size() && std::equal(lhs.data.begin(), lhs.data.end(), rhs.data.begin());
}

bool drafter::utils::so::operator==(const Array& lhs, const Array& rhs)
{
    return lhs.data.size() == rhs.data.size() && std::equal(lhs.data.begin(), lhs.data.end(), rhs.data.begin());
}

bool drafter::utils::so::operator==(const Value& lhs, const Value& rhs)
{
    using namespace drafter::utils;
    if (lhs.index() != rhs.index())
        return false;
    return visit( //
        lhs,
        [](const auto& l, const Value& r) { return l == get<typename bare<decltype(l)>::type>(r); },
        rhs);
}

Value* drafter::utils::so::find(Object& c, const std::string& key)
{
    auto it = std::find_if( //
        c.data.begin(),
        c.data.end(),
        [&key](const auto& entry) { return entry.first == key; });

    if (it != c.data.end())
        return &it->second;
    return nullptr;
}

void drafter::utils::so::emplace_unique(Array& c, Value&& value)
{
    auto it = std::find_if(c.data.begin(), c.data.end(), [&value](const auto& entry) { return entry == value; });

    if (it == c.data.end())
        c.data.emplace_back(std::move(value));
}

void drafter::utils::so::emplace_unique(Object& c, Object::container_type::value_type&& property)
{
    auto it = std::find_if(
        c.data.begin(), c.data.end(), [& key = property.first](const auto& entry) { return entry.first == key; });
    if (it == c.data.end())
        c.data.emplace_back(std::move(property));
    else
        it->second = std::move(property.second);
}
