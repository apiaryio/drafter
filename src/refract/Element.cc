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

#include "Visitors.h"

namespace refract
{

    bool isReserved(const std::string& element) {
        static std::set<std::string> reserved;
        if (reserved.empty()) {
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

    IElement::MemberElementCollection::const_iterator IElement::MemberElementCollection::find(const std::string& name) const
    {
        ComparableVisitor v(name);
        const_iterator it;

        for (it = begin(); it != end(); ++it) {
            (*it)->value.first->content(v);

            if (v.get()) {
                return it;
            }
        }

        return it;
    }

    IElement::MemberElementCollection::iterator IElement::MemberElementCollection::find(const std::string& name)
    {
        ComparableVisitor v(name);
        iterator it;

        for (it = begin(); it != end(); ++it) {
            (*it)->value.first->content(v);

            if (v.get()) {
                return it;
            }
        }

        return it;
    }

    StringElement* IElement::Create(const char* value)
    {
        return Create(std::string(value));
    };

    MemberElement& IElement::MemberElementCollection::operator[](const std::string& name)
    {
        const_iterator it = find(name);
        if (it != end()) {
            return *(*it);
        }

        // key not found - create new one and return reference
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

    void IElement::MemberElementCollection::clone(const IElement::MemberElementCollection& other)
    {
        for (const_iterator it = other.begin() ; it != other.end() ; ++it) {
            push_back(static_cast<value_type>((*it)->clone()));
        }
    }

    void IElement::MemberElementCollection::erase(const std::string& key)
    {
        iterator it = find(key);

        if (it != end()) {
            if (*it) {
                delete (*it);
            }

            std::vector<MemberElement*>::erase(it);
        }
    }

}; // namespace refract
