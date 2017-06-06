//
//  refract/SerializeCompactVisitor.cc
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#include "Element.h"

#include "SerializeCompactVisitor.h"

namespace refract
{

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
                SosSerializeCompactVisitor s(generateSourceMap);
                VisitBy(*(*it), s);
                array.push(s.value());
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
            VisitBy(*e.value.second, *this);
        }
    }

    void SosSerializeCompactVisitor::operator()(const ObjectElement& e)
    {
        typedef ObjectElement::ValueType::const_iterator iterator;

        sos::Object obj;

        for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
            SosSerializeCompactVisitor sv(generateSourceMap);
            VisitBy(*(*it), sv);
            obj.set(sv.key(), sv.value());
        }

        value_ = obj;
    }

    void SosSerializeCompactVisitor::operator()(const RefElement& e)
    {
        throw NotImplemented("NI: RefElement Compact Serialization");
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
