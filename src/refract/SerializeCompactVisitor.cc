//
//  refract/SerializeCompactVisitor.cc
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#include "Element.h"

namespace refract
{

    void SerializeCompactVisitor::visit(const IElement& e)
    {
        throw NotImplemented("NI: IElement Compact Serialization");
    }

    void SerializeCompactVisitor::visit(const NullElement& e)
    {
        value_ = sos::Null();
    }

    void SerializeCompactVisitor::visit(const StringElement& e)
    {
        value_ = sos::String(e.value);
    }

    void SerializeCompactVisitor::visit(const NumberElement& e)
    {
        value_ = sos::Number(e.value);
    }

    void SerializeCompactVisitor::visit(const BooleanElement& e)
    {
        value_ = sos::Boolean(e.value);
    }

    void SerializeCompactVisitor::visit(const ArrayElement& e)
    {
        sos::Array array;
        typedef ArrayElement::ValueType::const_iterator iterator;
        for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
            SerializeCompactVisitor s;
            (*it)->content(s);
            array.push(s.value());
        }
        value_ = array;
    }

    void SerializeCompactVisitor::visit(const MemberElement& e)
    {
        if (e.value.first) {
            SerializeCompactVisitor s;
            e.value.first->content(s);
            key_ = s.value().str;
        }

        if (e.value.second) {
            e.value.second->content(*this);
        }
    }

    void SerializeCompactVisitor::visit(const ObjectElement& e)
    {
        sos::Object obj;
        typedef ObjectElement::ValueType::const_iterator iterator;
        for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
            SerializeCompactVisitor sv;
            (*it)->content(sv);

            IElement::MemberElementCollection::const_iterator name = (*it)->meta.find("name");
            if (name == (*it)->meta.end() || (*name)->value.second == NULL) {
                throw LogicError("Object has not set name");
            }

            SerializeCompactVisitor sk;
            (*name)->content(sk);
            if (sk.value().type != sos::Base::StringType) {
                throw LogicError("Metadata element 'name' does not contain string type");
            }

            obj.set(sk.value().str, sv.value());
        }
        value_ = obj;
    }

}; // namespace refract
