//
//  refract/Element.cc
//  librefract
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#include "Element.h"
#include <cassert>

#include <set>
#include <string>

namespace refract
{
    refract::Registry DSRegistry;

    bool isReserved(const std::string& element) {
        static std::set<std::string> reserved;
        if(reserved.empty()) {
            reserved.insert("null");
            reserved.insert("boolean");
            reserved.insert("number");
            reserved.insert("string");

            reserved.insert("member");

            reserved.insert("array");
            reserved.insert("enum");
            reserved.insert("object");

            reserved.insert("ref");
            reserved.insert("select");
            reserved.insert("option");
            reserved.insert("extend");
        }

        return reserved.find(element) != reserved.end();
    }

    IElement::MemberElementCollection::const_iterator
    IElement::MemberElementCollection::find(const std::string& name) const
    {
        ComparableVisitor v(name);
        const_iterator it = begin();
        for (; it != end(); ++it) {
            (*it)->value.first->content(v);
            if (v)
                return it;
        }
        return it;
    }

    IElement* IElement::Create(const char* value)
    {
        return Create(std::string(value));
    };

    MemberElement& IElement::MemberElementCollection::operator[](const std::string& name)
    {
        const_iterator it = find(name);
        if (it != end()) {
            return *(*it);
        }

        StringElement* key = new StringElement;
        key->set(name);
        MemberElement* member = new MemberElement;
        member->value.first = key;
        push_back(member);
        return *member;
    }

    IElement::MemberElementCollection::~MemberElementCollection()
    {
        for (iterator it = begin(); it != end(); ++it) {
            delete (*it);
        }
        clear();
    }

    MemberElement& IElement::MemberElementCollection::operator[](const int index)
    {
        // IDEA : use static assert;
        throw LogicError("Do not use number index");
    }

}; // namespace refract
