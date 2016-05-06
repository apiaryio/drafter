//
//  refract/SerializeCompactVisitor.cc
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#include "Element.h"

#include "SerializeVisitor.h"
#include "SerializeCompactVisitor.h"

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

    void SosSerializeCompactVisitor::operator()(const IElement& e)
    {
        throw NotImplemented("NI: IElement Compact Serialization");
    }

    void SosSerializeCompactVisitor::operator()(const NullElement& e)
    {
        value_ = sos::Null();
    }

    void SosSerializeCompactVisitor::operator()(const StringElement& e)
    {
        value_ = sos::String(e.value);
    }

    void SosSerializeCompactVisitor::operator()(const NumberElement& e)
    {
        value_ = sos::Number(e.value);
    }

    void SosSerializeCompactVisitor::operator()(const BooleanElement& e)
    {
        value_ = sos::Boolean(e.value);
    }

    namespace {

        template <typename Values>
        void SerializeValues(sos::Array& array, const Values& values, bool generateSourceMap)
        {

            for (typename Values::const_iterator it = values.begin(); it != values.end(); ++it) {
                if (IsFullRender()((*it), generateSourceMap)) {
                    SosSerializeVisitor s(generateSourceMap);
                    Visit(s, *(*it));
                    array.push(s.get());
                }
                else {
                    SosSerializeCompactVisitor s(generateSourceMap);
                    VisitBy(*(*it), s);
                    array.push(s.value());
                }
            }
        }
    }

    void SosSerializeCompactVisitor::operator()(const EnumElement& e)
    {
        sos::Array array;
        SerializeValues(array, e.value, generateSourceMap);
        value_ = array;
    }

    void SosSerializeCompactVisitor::operator()(const ArrayElement& e)
    {
        sos::Array array;
        SerializeValues(array, e.value, generateSourceMap);
        value_ = array;
    }

    void SosSerializeCompactVisitor::operator()(const MemberElement& e)
    {
        if (e.value.first) {
            SosSerializeCompactVisitor s(generateSourceMap);
            VisitBy(*e.value.first, s);
            key_ = s.value().str;
        }

        if (e.value.second) {
            if (!IsFullRender()(e.value.second, generateSourceMap)) {
                VisitBy(*e.value.second, *this);
            }
            else { // value has request to be serialized in Expanded form
                SosSerializeVisitor s(generateSourceMap);
                Visit(s, *e.value.second);
                value_ = s.get();
            }
        }
    }

    void SosSerializeCompactVisitor::operator()(const ObjectElement& e)
    {

        typedef ObjectElement::ValueType::const_iterator iterator;
        iterator it = find_if(e.value.begin(), e.value.end(), std::bind2nd(IsFullRender(), generateSourceMap));

        // if there is ANY element required to be serialized in Full
        // we must use array to serialize
        if (it != e.value.end()) {
            sos::Array arr;

            for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
                SosSerializeVisitor s(generateSourceMap);
                Visit(s, *(*it));
                arr.push(s.get());
            }

            value_ = arr;
        }
        else {
            sos::Object obj;

            for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
                SosSerializeCompactVisitor sv(generateSourceMap);
                VisitBy(*(*it), sv);
                obj.set(sv.key(), sv.value());
            }

            value_ = obj;
        }
    }

    void SosSerializeCompactVisitor::operator()(const ExtendElement& e)
    {
        throw NotImplemented("ExtendElement serialization Not Implemented");
    }

    void SosSerializeCompactVisitor::operator()(const OptionElement& e)
    {
        sos::Array array;
        SerializeValues(array, e.value, generateSourceMap);
        value_ = array;
    }

    void SosSerializeCompactVisitor::operator()(const SelectElement& e)
    {
        sos::Array array;
        SerializeValues(array, e.value, generateSourceMap);
        value_ = array;
    }


}; // namespace refract
