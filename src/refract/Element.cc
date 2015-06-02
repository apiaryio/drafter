//
//  refract/Element.cc
//  librefract
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#include "Element.h"
#include <cassert>

namespace refract
{

#if 0
    void SerializeVisitor::visit(const IElement& e)
    {
        result.set("element", sos::String(e.element()));

        typedef IElement::MemberElementCollection::const_iterator iterator;

        // FIXME : refactoring
        if (!e.meta.empty()) {
            sos::Object obj;
            for (iterator it = e.meta.begin(); it != e.meta.end(); ++it) {
                SerializeCompactVisitor s;
                s.visit(*(*it));
                obj.set(s.key(), s.value());
            }
            result.set("meta", obj);
        }

        if (!e.attributes.empty()) {
            sos::Object obj;
            for (iterator it = e.attributes.begin(); it != e.attributes.end(); ++it) {
                SerializeCompactVisitor s;
                s.visit(*(*it));
                obj.set(s.key(), s.value());
            }
            result.set("attributes", obj);
        }

        if (e.empty())
            return;

        if (e.compactContent()) {
            SerializeCompactVisitor s;
            e.content(s);
            result.set("content", s.value());
        } else {
            e.content(*this);
            result.set("content", partial);
        }
    }

    void SerializeVisitor::SetSerializerValue(SerializeVisitor& s, sos::Base& value)
    {
        if (!s.key.empty()) {
            s.result.set(s.key, value);
            s.key.clear();
        } else {
            s.partial = value;
        }
    }

    void SerializeVisitor::visit(const NullElement& e)
    {
        sos::Base value = sos::Null();
        SetSerializerValue(*this, value);
    }

    void SerializeVisitor::visit(const StringElement& e)
    {
        sos::Base value = sos::Null();
        if (!e.empty()) {
            value = sos::String(e.value);
        }
        SetSerializerValue(*this, value);
    }

    void SerializeVisitor::visit(const NumberElement& e)
    {
        sos::Base value = sos::Null();
        if (!e.empty()) {
            value = sos::Number(e.value);
        }
        SetSerializerValue(*this, value);
    }

    void SerializeVisitor::visit(const BooleanElement& e)
    {
        sos::Base value = sos::Null();
        if (!e.empty()) {
            value = sos::Boolean(e.value);
        }
        SetSerializerValue(*this, value);
    }

    void SerializeVisitor::visit(const ArrayElement& e)
    {
        sos::Array array;

        typedef ArrayElement::ValueType::const_iterator iterator;
        for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
            SerializeVisitor s;
            s.visit(*(*it));
            array.push(s.get());
        }
        SetSerializerValue(*this, array);
    }

    void SerializeVisitor::visit(const MemberElement& e)
    {
        sos::Object object;
        if(e.value.first) {
            SerializeVisitor s;
            s.visit(*e.value.first);
            object.set("key",s.get());
        }

        if(e.value.second) {
            SerializeVisitor s;
            s.visit(*e.value.second);
            object.set("value",s.get());
        }
        SetSerializerValue(*this, object);
    }

    void SerializeVisitor::visit(const ObjectElement& e)
    {
        sos::Array array;

        typedef ObjectElement::ValueType::const_iterator iterator;
        for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
            SerializeVisitor s;
            s.visit(static_cast<IElement&>(*(*it)));
            array.push(s.get());
        }
        SetSerializerValue(*this, array);
    }
#endif


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
