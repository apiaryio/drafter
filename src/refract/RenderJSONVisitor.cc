//
//  refract/RenderJSONVisitor.cc
//  librefract
//
//  Created by Pavan Kumar Sunkara on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "VisitorUtils.h"
#include "sosJSON.h"
#include <sstream>

namespace refract
{

    RenderJSONVisitor::RenderJSONVisitor(const sos::Base::Type& type_)
    : type(type_), isExtend(false) {}

    RenderJSONVisitor::RenderJSONVisitor()
    : type(sos::Base::UndefinedType), isExtend(false) {}

    void RenderJSONVisitor::assign(sos::Base value)
    {
        if (isExtend) {
            extend(value);
        }
        else if (type == sos::Base::ArrayType) {
            pArr.push(value);
        }
        else if (type == sos::Base::UndefinedType) {
            result = value;
        }
    }

    void RenderJSONVisitor::assign(std::string key, sos::Base value)
    {
        if (!key.empty() && type == sos::Base::ObjectType) {
            pObj.set(key, value);
        }
    }

    void RenderJSONVisitor::extend(sos::Base value)
    {
        if (type == sos::Base::ObjectType) {

            for (sos::KeyValues::iterator it = value.object().begin();
                 it != value.object().end();
                 ++it) {

                pObj.set(it->first, it->second);
            }
        }
        else if (type == sos::Base::ArrayType) {

            for (sos::Bases::iterator it = value.array().begin();
                 it != value.array().end();
                 ++it) {

                pArr.push(*it);
            }
        }
    }

    void RenderJSONVisitor::visit(const IElement& e)
    {
        e.content(*this);
    }

    void RenderJSONVisitor::visit(const MemberElement& e)
    {
        RenderJSONVisitor renderer;

        if (e.value.second) {
            if (IsTypeAttribute(e, "nullable") && e.value.second->empty()) {
                renderer.result = sos::Null();
            } else if (IsTypeAttribute(e, "optional") && e.value.second->empty()) {
                return;
            } else {
                renderer.visit(*e.value.second);
            }
        }

        if (StringElement* str = TypeQueryVisitor::as<StringElement>(e.value.first)) {
            assign(str->value, renderer.get());
        }
        else {
            throw std::logic_error("A property's key in the object is not of type string");
        }
    }

    void RenderJSONVisitor::visit(const ObjectElement& e)
    {
        // If the element is a mixin reference
        if (e.element() == "ref") {
            IElement::MemberElementCollection::const_iterator resolved = e.attributes.find("resolved");

            if (resolved == e.attributes.end()) {
                return;
            }

            RenderJSONVisitor renderer;
            if ((*resolved)->value.second) {
                renderer.visit(*(*resolved)->value.second);
            }

            // Imitate an extend object
            isExtend = true;
            assign(renderer.get());
            isExtend = false;

            return;
        }

        RenderJSONVisitor renderer(sos::Base::ObjectType);

        const ObjectElement::ValueType* val = GetValue<ObjectElement>(e);

        if (!val) {
            return;
        }

        if (e.element() == "extend") {
            renderer.isExtend = true;
        }

        for (std::vector<refract::IElement*>::const_iterator it = val->begin();
             it != val->end();
             ++it) {

            if (*it) {
                renderer.visit(*(*it));
            }
        }

        assign(renderer.get());
    }

    namespace {

        void FetchArray(const ArrayElement::ValueType* val, RenderJSONVisitor& renderer)
        {
            for (ArrayElement::ValueType::const_iterator it = val->begin();
                 it != val->end();
                 ++it) {

                if (*it && !(*it)->empty()) {
                    renderer.visit(*(*it));
                }
            }
        }

        IElement* getEnumValue(const ArrayElement::ValueType* extend)
        {
            if (!extend || extend->empty()) {
                return NULL;
            }

            for (ArrayElement::ValueType::const_reverse_iterator it = extend->rbegin();
                 it != extend->rend();
                 ++it) {

                const ArrayElement* element = TypeQueryVisitor::as<ArrayElement>(*it);

                if (!element) {
                    continue;
                }

                const ArrayElement::ValueType* items = GetValue<ArrayElement>(*element);

                if (!items->empty()) {
                    return *items->begin();
                }
            }

            return NULL;
        }

        bool isEnum(const ArrayElement::ValueType* val)
        {
            for (ArrayElement::ValueType::const_iterator it = val->begin();
                 it != val->end();
                 ++it) {

                if ((*it)->element() == "enum") {
                    return true;
                }
            }
            return false;
        }
    }

    void RenderJSONVisitor::visit(const ArrayElement& e)
    {
        RenderJSONVisitor renderer(sos::Base::ArrayType);

        const ArrayElement::ValueType* val = GetValue<ArrayElement>(e);

        if (!val) {
            return;
        }

        if (e.element() == "extend") {
            if (isEnum(val)) {
                IElement* value = getEnumValue(val);

                if (!value) {
                    assign(sos::String());
                    return;
                }

                RenderJSONVisitor renderer;
                value->content(renderer);
                assign(renderer.get());
                return;
            }

            renderer.isExtend = true;
        }

        FetchArray(val, renderer);
        assign(renderer.get());
    }

    void RenderJSONVisitor::visit(const NullElement& e) {}

    void RenderJSONVisitor::visit(const StringElement& e)
    {
        const StringElement::ValueType* v = GetValue<StringElement>(e);

        if (v) {
            assign(sos::String(*v));
        }
    }

    void RenderJSONVisitor::visit(const NumberElement& e)
    {
        const NumberElement::ValueType* v = GetValue<NumberElement>(e);

        if (v) {
            assign(sos::Number(*v));
        }
    }

    void RenderJSONVisitor::visit(const BooleanElement& e)
    {
        const BooleanElement::ValueType* v = GetValue<BooleanElement>(e);

        if (v) {
            assign(sos::Boolean(*v));
        }
    }

    sos::Base RenderJSONVisitor::get() const
    {
        if (type == sos::Base::ArrayType) {
            return pArr;
        }
        else if (type == sos::Base::ObjectType) {
            return pObj;
        }

        return result;
    }

    std::string RenderJSONVisitor::getString() const
    {
        sos::SerializeJSON serializer;
        std::stringstream ss;

        serializer.process(get(), ss);
        return ss.str();
    }
}
