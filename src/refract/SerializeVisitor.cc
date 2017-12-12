//
//  refract/SerializeVisitor.cc
//  librefract
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#include "Element.h"

#include "SerializeCompactVisitor.h"
#include "SerializeVisitor.h"
#include "TypeQueryVisitor.h"

namespace refract
{

    namespace
    {
        sos::Object SerializeElementCollection(const InfoElements& collection, bool generateSourceMap)
        {
            sos::Object result;

            for (const auto& m : collection) {

                if (!generateSourceMap) {
                    if (m.first == "sourceMap") {
                        continue;
                    }
                }

                SosSerializeVisitor s(generateSourceMap);
                Visit(s, *m.second);
                result.set(m.first, s.get());
            }

            return result;
        }

        sos::Object ElementToObject(const IElement* e, bool generateSourceMap)
        {
            SosSerializeVisitor s(generateSourceMap);
            Visit(s, *e);
            return s.get();
        }

        template <typename T>
        sos::Array SerializeValueList(const T& e, bool generateSourceMap)
        {
            sos::Array array;

            for (const auto& v : e.get()) {
                array.push(ElementToObject(v.get(), generateSourceMap));
            }

            return array;
        }

    } // end of anonymous namespace

    void SosSerializeVisitor::operator()(const IElement& e)
    {
        result.set("element", sos::String(e.element()));
        bool sourceMap = generateSourceMap;

        sos::Object meta = SerializeElementCollection(e.meta(), sourceMap);
        if (!meta.empty()) {
            result.set("meta", meta);
        }

        if (e.element() == "annotation") {
            sourceMap = true;
        }

        sos::Object attr = SerializeElementCollection(e.attributes(), sourceMap);
        if (!attr.empty()) {
            result.set("attributes", attr);
        }

        if (e.empty())
            return;

        VisitBy(e, *this);
        result.set("content", partial);
    }

    void SosSerializeVisitor::SetSerializerValue(SosSerializeVisitor& s, sos::Base& value)
    {
        if (!s.key.empty()) {
            s.result.set(s.key, value);
            s.key.clear();
        } else {
            s.partial = value;
        }
    }

    void SosSerializeVisitor::operator()(const HolderElement& e)
    {
        sos::Object object = ElementToObject(e.get().data(), generateSourceMap);

        SetSerializerValue(*this, object);
    }

    void SosSerializeVisitor::operator()(const NullElement& e)
    {
        sos::Base value = sos::Null();
        SetSerializerValue(*this, value);
    }

    void SosSerializeVisitor::operator()(const StringElement& e)
    {
        sos::Base value = sos::Null();

        if (!e.empty()) {
            value = sos::String(e.get());
        }

        SetSerializerValue(*this, value);
    }

    void SosSerializeVisitor::operator()(const NumberElement& e)
    {
        sos::Base value = sos::Null();

        if (!e.empty()) {
            value = sos::Number(e.get());
        }

        SetSerializerValue(*this, value);
    }

    void SosSerializeVisitor::operator()(const BooleanElement& e)
    {
        sos::Base value = sos::Null();

        if (!e.empty()) {
            value = sos::Boolean(e.get());
        }

        SetSerializerValue(*this, value);
    }

    void SosSerializeVisitor::operator()(const MemberElement& e)
    {
        sos::Object object;

        if (const auto key = e.get().key()) {
            object.set("key", ElementToObject(key, generateSourceMap));
        }

        if (const auto value = e.get().value()) {
            object.set("value", ElementToObject(value, generateSourceMap));
        }

        SetSerializerValue(*this, object);
    }

    void SosSerializeVisitor::operator()(const ArrayElement& e)
    {
        sos::Array array = SerializeValueList(e, generateSourceMap);
        SetSerializerValue(*this, array);
    }

    void SosSerializeVisitor::operator()(const EnumElement& e)
    {
        sos::Object object = ElementToObject(e.get().value(), generateSourceMap);
        SetSerializerValue(*this, object);
    }

    void SosSerializeVisitor::operator()(const ObjectElement& e)
    {
        sos::Array array = SerializeValueList(e, generateSourceMap);
        SetSerializerValue(*this, array);
    }

    void SosSerializeVisitor::operator()(const RefElement& e)
    {
        sos::Base value = sos::Null();

        if (!e.empty()) {
            value = sos::String(e.get().symbol());
        }

        SetSerializerValue(*this, value);
    }

    void SosSerializeVisitor::operator()(const ExtendElement& e)
    {
        sos::Array array = SerializeValueList(e, generateSourceMap);
        SetSerializerValue(*this, array);
    }

    void SosSerializeVisitor::operator()(const OptionElement& e)
    {
        sos::Array array = SerializeValueList(e, generateSourceMap);
        SetSerializerValue(*this, array);
    }

    void SosSerializeVisitor::operator()(const SelectElement& e)
    {
        sos::Array array = SerializeValueList(e, generateSourceMap);
        SetSerializerValue(*this, array);
    }

}; // namespace refract
