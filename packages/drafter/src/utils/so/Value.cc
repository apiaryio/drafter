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

namespace
{
    bool hasEqualMembers(const Object& lhs, const Object& rhs)
    {
        using itemType = typename Object::container_type::const_reference;
        for (itemType item : lhs.data) {

            if (std::find_if(rhs.data.begin(), rhs.data.end(), [&item](itemType rItem) { return item == rItem; })
                == rhs.data.end()) {
                return false;
            }
        }

        return true;
    }
}

bool drafter::utils::so::operator==(const Object& lhs, const Object& rhs)
{
    return lhs.data.size() == rhs.data.size() && hasEqualMembers(lhs, rhs);
}

bool drafter::utils::so::operator==(const Array& lhs, const Array& rhs)
{
    return lhs.data.size() == rhs.data.size() && std::equal(lhs.data.begin(), lhs.data.end(), rhs.data.begin());
}

namespace
{
    struct same_equal {
        const Value& rhs;
        template <typename Specific>
        bool operator()(const Specific& lhs) const noexcept
        {
            return lhs == mpark::get<Specific>(rhs);
        }
    };
}

bool drafter::utils::so::operator==(const Value& lhs, const Value& rhs)
{
    if (lhs.index() != rhs.index())
        return false;

    return mpark::visit(same_equal{ rhs }, lhs);
}

Value* drafter::utils::so::find(Object& c, const std::string& key)
{
    auto it = std::find_if( //
        c.data.begin(),
        c.data.end(),
        [&key](const Object::container_type::value_type& entry) { return entry.first == key; });

    if (it != c.data.end())
        return &it->second;
    return nullptr;
}

void drafter::utils::so::emplace_unique(Array& c, Value&& value)
{
    auto it = std::find_if(c.data.begin(), c.data.end(), [&value](const Array::container_type::value_type& entry) {
        return entry == value;
    });

    if (it == c.data.end())
        c.data.emplace_back(std::move(value));
}

void drafter::utils::so::emplace_unique(Object& c, Object::container_type::value_type&& property)
{
    const auto& key = property.first;
    auto it = std::find_if(c.data.begin(), c.data.end(), [&key](const Object::container_type::value_type& entry) {
        return entry.first == key;
    });
    if (it == c.data.end())
        c.data.emplace_back(std::move(property));
    else
        it->second = std::move(property.second);
}
