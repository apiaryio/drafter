//
//  refract/dsd/InfoElements.h
//  librefract
//
//  Created by Thomas Jandecka on 21/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "InfoElements.h"

#include <cassert>
#include <algorithm>
#include "Element.h"
#include "dsd/ElementData.h"
#include "TypeQueryVisitor.h"

namespace refract
{
    InfoElements::InfoElements() : elements() {}

    InfoElements::InfoElements(InfoElements&& other) : InfoElements()
    {
        swap(*this, other);
    }

    InfoElements& InfoElements::operator=(InfoElements rhs)
    {
        swap(*this, rhs);
        return *this;
    }

    InfoElements::const_iterator InfoElements::begin() const noexcept
    {
        return elements.begin();
    }

    InfoElements::iterator InfoElements::begin() noexcept
    {
        return elements.begin();
    }

    InfoElements::const_iterator InfoElements::end() const noexcept
    {
        return elements.end();
    }

    InfoElements::iterator InfoElements::end() noexcept
    {
        return elements.end();
    }

    void InfoElements::erase(iterator it)
    {
        elements.erase(it);
    }

    void InfoElements::clear()
    {
        elements.clear();
    }

    bool InfoElements::empty() const noexcept
    {
        return elements.empty();
    }

    InfoElements::Container::size_type InfoElements::size() const noexcept
    {
        return elements.size();
    }

    InfoElements::InfoElements(const InfoElements& other) : elements()
    {
        elements.reserve(other.size());
        std::transform(
            other.begin(), other.end(), std::back_inserter(elements), [](const InfoElements::value_type& el) {
                assert(el.second);
                return std::make_pair(el.first, refract::clone(*el.second));
            });
    }

    void InfoElements::clone(const InfoElements& other)
    {
        std::transform(
            other.begin(), other.end(), std::back_inserter(elements), [](const InfoElements::value_type& el) {
                assert(el.second);
                return std::make_pair(el.first, refract::clone(*el.second));
            });
    }

    void InfoElements::erase(const std::string& key)
    {
        elements.erase(std::remove_if(elements.begin(),
                           elements.end(),
                           [&key](const InfoElements::value_type& keyValue) { return keyValue.first == key; }),
            elements.end());
    }

    IElement& InfoElements::set(const std::string& key, std::unique_ptr<IElement> value)
    {
        auto& valueRef = *value;

        auto it = find(key);
        if (it == end())
            elements.emplace_back(key, std::move(value));
        else
            it->second = std::move(value);

        return valueRef;
    }

    IElement& InfoElements::set(const std::string& key, const IElement& value)
    {
        return set(key, refract::clone(value));
    }

    std::unique_ptr<IElement> InfoElements::claim(const std::string& key)
    {
        auto member = find(key);
        if (member != elements.end()) {
            return claim(member);
        }
        return nullptr;
    }

    std::unique_ptr<IElement> InfoElements::claim(iterator it)
    {
        std::unique_ptr<IElement> result(it->second.release());
        elements.erase(it);

        return result;
    }

    InfoElements::const_iterator InfoElements::find(const std::string& name) const
    {
        return std::find_if(elements.begin(), elements.end(), [&name](const InfoElements::value_type& keyValue) {
            return keyValue.first == name;
        });
    }

    InfoElements::iterator InfoElements::find(const std::string& name)
    {
        return std::find_if(elements.begin(), elements.end(), [&name](const InfoElements::value_type& keyValue) {
            return keyValue.first == name;
        });
    }
}
