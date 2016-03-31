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

    struct IsFullRender {
        typedef const IElement* first_argument_type;
        typedef const bool second_argument_type;
        typedef const bool result_type;

        bool operator()(const IElement* element, const bool exportSourceMap) const {

            if (exportSourceMap) {
                IElement::MemberElementCollection::const_iterator it = element->attributes.find("sourceMap");
                // there is sourceMap in attributes
                if (it != element->attributes.end()) {
                    return true;
                }
            }

            return element && (element->renderType() == IElement::rFull || element->renderType() == IElement::rCompactContent);
        }
    };

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

    namespace {

        void SerializeValues(sos::Array& array, const RefractElements& values, bool exportSourceMap)
        {

            for (RefractElements::const_iterator it = values.begin(); it != values.end(); ++it) {
                if (IsFullRender()((*it), exportSourceMap)) {
                    SerializeVisitor s(exportSourceMap);
                    s.visit(*(*it));
                    array.push(s.get());
                }
                else {
                    SerializeCompactVisitor s(exportSourceMap);
                    (*it)->content(s);
                    array.push(s.value());
                }
            }
        }
    }

    void SerializeCompactVisitor::visit(const EnumElement& e)
    {
        sos::Array array;
        SerializeValues(array, e.value, exportSourceMap);
        value_ = array;
    }

    void SerializeCompactVisitor::visit(const ArrayElement& e)
    {
        sos::Array array;
        SerializeValues(array, e.value, exportSourceMap);
        value_ = array;
    }

    void SerializeCompactVisitor::visit(const MemberElement& e)
    {
        if (e.value.first) {
            SerializeCompactVisitor s(exportSourceMap);
            e.value.first->content(s);
            key_ = s.value().str;
        }

        if (e.value.second) {
            if (!IsFullRender()(e.value.second, exportSourceMap)) {
                e.value.second->content(*this);
            }
            else { // value has request to be serialized in Expanded form
                SerializeVisitor s(exportSourceMap);
                s.visit(*e.value.second);
                value_ = s.get();
            }
        }
    }

    void SerializeCompactVisitor::visit(const ObjectElement& e)
    {

        typedef ObjectElement::ValueType::const_iterator iterator;
        iterator it = find_if(e.value.begin(), e.value.end(), std::bind2nd(IsFullRender(), exportSourceMap));

        // if there is ANY element required to be serialized in Full
        // we must use array to serialize
        if (it != e.value.end()) {
            sos::Array arr;

            for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
                SerializeVisitor s(exportSourceMap);
                s.visit(*(*it));
                arr.push(s.get());
            }

            value_ = arr;
        }
        else {
            sos::Object obj;

            for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
                SerializeCompactVisitor sv(exportSourceMap);
                (*it)->content(sv);
                obj.set(sv.key(), sv.value());
            }

            value_ = obj;
        }
    }

    void SerializeCompactVisitor::visit(const ExtendElement& e)
    {
        throw NotImplemented("ExtendElement serialization Not Implemented");
    }

}; // namespace refract
