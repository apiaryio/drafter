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
#include "../VisitorUtils.h"

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

Object::Object() : elements_() {}

Object::Object(const Object& other) : elements_()
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

Object::Object(Object&& rhs) : Object()
{
    swap(*this, rhs);
}

Object& Object::operator=(Object rhs)
{
    swap(*this, rhs);
    return *this;
}

void Object::push_back(std::unique_ptr<IElement> el)
{
    assert(el);

    if (!empty())
        if (auto mbr = TypeQueryVisitor::as<const MemberElement>(el.get()))
            if (!mbr->empty() && mbr->get().key())
                if (auto str = TypeQueryVisitor::as<const StringElement>(mbr->get().key()))
                    if (!str->empty()) {
                        auto it = find(str->get().get());
                        if (it != end())
                            erase(it);
                    }

    elements_.emplace_back(std::move(el));
}

Object::iterator Object::insert(Object::iterator it, std::unique_ptr<IElement> el)
{
    assert(it >= begin());
    assert(it <= end());
    assert(el);

    return elements_.insert(it, std::move(el));
}

Object::iterator Object::erase(Object::iterator b, Object::iterator e)
{
    return elements_.erase(b, e);
}

Object::iterator Object::find(const std::string& name)
{
    return std::find_if(begin(), end(), [&name](const std::unique_ptr<IElement>& entry) {
        if (auto mbr = TypeQueryVisitor::as<const MemberElement>(entry.get())) {
            if (mbr->empty())
                return false;
            if (auto key = TypeQueryVisitor::as<const StringElement>(mbr->get().key()))
                return !key->empty() && (key->get().get() == name);
        }
        return false;
    });
}

Object::iterator Object::addMember(std::string name, std::unique_ptr<IElement> value)
{
    auto it = find(name);
    if (it != end())
        erase(it);
    return insert(end(), make_element<MemberElement>(name, std::move(value)));
}

bool dsd::operator==(const Object& lhs, const Object& rhs) noexcept
{
    return lhs.size() == rhs.size()
        && std::equal( //
               lhs.begin(),
               lhs.end(),
               rhs.begin(),
               [](const Object::value_type& l, const Object::value_type& r) {
                   assert(l);
                   assert(r);
                   return *l == *r;
               });
}

bool dsd::operator!=(const Object& lhs, const Object& rhs) noexcept
{
    return !(lhs == rhs);
}
