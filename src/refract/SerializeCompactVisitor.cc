//
//  refract/SerializeCompactVisitor.cc
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#include "Element.h"
#include "Visitors.h"

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
            if ((*it)->renderType() == IElement::rFull) {
                SerializeVisitor s;
                s.visit(*(*it));
                array.push(s.get());
            }
            else {
                SerializeCompactVisitor s;
                (*it)->content(s);
                array.push(s.value());
            }
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
            if (e.value.second->renderType() != IElement::rFull) {
                e.value.second->content(*this);
            }
            else { // value has request to be serialized in Expanded form
                SerializeVisitor s;
                s.visit(*e.value.second);
                value_ = s.get();
            }
        }
    }

    static bool IsFullRender(const IElement* element) {
        return element->renderType() == IElement::rFull;
    }

    void SerializeCompactVisitor::visit(const ObjectElement& e)
    {

        typedef ObjectElement::ValueType::const_iterator iterator;
        iterator it = find_if(e.value.begin(), e.value.end(), IsFullRender);

        // if there is ANY element required to be serialized in Full
        // we must use array to serialize
        if (it != e.value.end()) {
            sos::Array arr;

            for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
                SerializeVisitor s;
                s.visit(*(*it));
                arr.push(s.get());
            }

            value_ = arr;
        }
        else {
            sos::Object obj;

            for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
                SerializeCompactVisitor sv;
                (*it)->content(sv);
                obj.set(sv.key(), sv.value());
            }

            value_ = obj;
        }
    }

}; // namespace refract
