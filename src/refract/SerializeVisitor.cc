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
        sos::Object SerializeElementCollection(const IElement::MemberElementCollection& collection, bool generateSourceMap)
        {
            typedef IElement::MemberElementCollection::const_iterator iterator;

            sos::Object result;

            for (iterator it = collection.begin(); it != collection.end(); ++it) {

                if (!generateSourceMap) {
                    StringElement* str = TypeQueryVisitor::as<StringElement>((*it)->value.first);
                    if (str && str->value == "sourceMap"){
                        continue;
                    }
                }

                SosSerializeCompactVisitor s(generateSourceMap);
                Visit(s, *(*it));
                result.set(s.key(), s.value());
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
        sos::Array SerializeValueList(const T& e, bool generateSourceMap) {
            sos::Array array;
            typedef typename T::ValueType::const_iterator iterator;

            for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
                array.push(ElementToObject(*it, generateSourceMap));
            }

            return array;
        }

        IElement::renderFlags SelectSerializationType(const IElement& e, bool generateSourceMap) {

            if (generateSourceMap) {
                IElement::MemberElementCollection::const_iterator it = e.attributes.find("sourceMap");
                // there is sourceMap in attributes
                if (it != e.attributes.end()) {
                    return IElement::rFull;
                }
            }

            if (e.renderType() == IElement::rCompact || e.renderType() == IElement::rCompactContent) {
                return IElement::rCompact;
            }

            return IElement::rFull;
        }

    } // end of anonymous namespace

    void SosSerializeVisitor::operator()(const IElement& e)
    {
        result.set("element", sos::String(e.element()));
        bool sourceMap = generateSourceMap;

        sos::Object meta = SerializeElementCollection(e.meta, sourceMap);
        if (!meta.empty()) {
            result.set("meta", meta);
        }

        if (e.element() == "annotation") {
            sourceMap = true;
        }
        sos::Object attr = SerializeElementCollection(e.attributes, sourceMap);
        if (!attr.empty()) {
            result.set("attributes", attr);
        }

        if (e.empty())
            return;

        const IElement::renderFlags render = SelectSerializationType(e, generateSourceMap);

        if (render == IElement::rCompact) {
            SosSerializeCompactVisitor s(generateSourceMap);
            VisitBy(e, s);
            result.set("content", s.value());
        } else {
            VisitBy(e, *this);
            result.set("content", partial);
        }
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

    void SosSerializeVisitor::operator()(const NullElement& e)
    {
        sos::Base value = sos::Null();
        SetSerializerValue(*this, value);
    }

    void SosSerializeVisitor::operator()(const StringElement& e)
    {
        sos::Base value = sos::Null();

        if (!e.empty()) {
            value = sos::String(e.value);
        }

        SetSerializerValue(*this, value);
    }

    void SosSerializeVisitor::operator()(const NumberElement& e)
    {
        sos::Base value = sos::Null();

        if (!e.empty()) {
            value = sos::Number(e.value);
        }

        SetSerializerValue(*this, value);
    }

    void SosSerializeVisitor::operator()(const BooleanElement& e)
    {
        sos::Base value = sos::Null();

        if (!e.empty()) {
            value = sos::Boolean(e.value);
        }

        SetSerializerValue(*this, value);
    }


    void SosSerializeVisitor::operator()(const MemberElement& e)
    {
        sos::Object object;

        if (e.value.first) {
            object.set("key", ElementToObject(e.value.first, generateSourceMap));
        }

        if (e.value.second) {
            object.set("value", ElementToObject(e.value.second, generateSourceMap));
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
        sos::Array array = SerializeValueList(e, generateSourceMap);
        SetSerializerValue(*this, array);
    }

    void SosSerializeVisitor::operator()(const ObjectElement& e)
    {
        sos::Array array = SerializeValueList(e, generateSourceMap);
        SetSerializerValue(*this, array);
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
