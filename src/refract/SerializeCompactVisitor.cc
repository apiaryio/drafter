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

        bool operator()(const IElement* element, const bool generateSourceMap) const {

            if (generateSourceMap) {
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

        template <typename Values>
        void SerializeValues(sos::Array& array, const Values& values, bool generateSourceMap)
        {

            for (typename Values::const_iterator it = values.begin(); it != values.end(); ++it) {
                if (IsFullRender()((*it), generateSourceMap)) {
                    SerializeVisitor s(generateSourceMap);
                    s.visit(*(*it));
                    array.push(s.get());
                }
                else {
                    SerializeCompactVisitor s(generateSourceMap);
                    (*it)->content(s);
                    array.push(s.value());
                }
            }
        }
    }

    void SerializeCompactVisitor::visit(const EnumElement& e)
    {
        sos::Array array;
        SerializeValues(array, e.value, generateSourceMap);
        value_ = array;
    }

    void SerializeCompactVisitor::visit(const ArrayElement& e)
    {
        sos::Array array;
        SerializeValues(array, e.value, generateSourceMap);
        value_ = array;
    }

    void SerializeCompactVisitor::visit(const MemberElement& e)
    {
        if (e.value.first) {
            SerializeCompactVisitor s(generateSourceMap);
            e.value.first->content(s);
            key_ = s.value().str;
        }

        if (e.value.second) {
            if (!IsFullRender()(e.value.second, generateSourceMap)) {
                e.value.second->content(*this);
            }
            else { // value has request to be serialized in Expanded form
                SerializeVisitor s(generateSourceMap);
                s.visit(*e.value.second);
                value_ = s.get();
            }
        }
    }

    void SerializeCompactVisitor::visit(const ObjectElement& e)
    {

        typedef ObjectElement::ValueType::const_iterator iterator;
        iterator it = find_if(e.value.begin(), e.value.end(), std::bind2nd(IsFullRender(), generateSourceMap));

        // if there is ANY element required to be serialized in Full
        // we must use array to serialize
        if (it != e.value.end()) {
            sos::Array arr;

            for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
                SerializeVisitor s(generateSourceMap);
                s.visit(*(*it));
                arr.push(s.get());
            }

            value_ = arr;
        }
        else {
            sos::Object obj;

            for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
                SerializeCompactVisitor sv(generateSourceMap);
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

    void SerializeCompactVisitor::visit(const OptionElement& e)
    {
        sos::Array array;
        SerializeValues(array, e.value, generateSourceMap);
        value_ = array;
    }

    void SerializeCompactVisitor::visit(const SelectElement& e)
    {
        sos::Array array;
        SerializeValues(array, e.value, generateSourceMap);
        value_ = array;
    }


}; // namespace refract
