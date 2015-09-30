//
//  refract/RenderJSONVisitor.cc
//  librefract
//
//  Created by Pavan Kumar Sunkara on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "Element.h"
#include "Visitors.h"
#include "sosJSON.h"
#include <sstream>

namespace refract
{

    RenderJSONVisitor::RenderJSONVisitor(const sos::Base::Type& type_)
    : type(type_), isExtend(false) {}

    RenderJSONVisitor::RenderJSONVisitor()
    : type(sos::Base::UndefinedType), isExtend(false) {}

    void RenderJSONVisitor::assign(sos::Base value) {
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

    void RenderJSONVisitor::assign(std::string key, sos::Base value) {
        if (!key.empty() && type == sos::Base::ObjectType) {
            pObj.set(key, value);
        }
    }

    void RenderJSONVisitor::extend(sos::Base value) {
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

    void RenderJSONVisitor::visit(const IElement& e) {
        e.content(*this);
    }

    template<typename T>
    bool IsTypeAttribute(const T& e, std::string typeAttribute) {
        IElement::MemberElementCollection::const_iterator ta = e.attributes.find("typeAttributes");
        
        if (ta == e.attributes.end()) {
            return false;
        }

        ArrayElement* attrs = TypeQueryVisitor::as<ArrayElement>((*ta)->value.second);

        if (!attrs) {
            return false;
        }

        for (ArrayElement::ValueType::const_iterator it = attrs->value.begin() ; it != attrs->value.end() ; ++it ) {
            StringElement* attr = TypeQueryVisitor::as<StringElement>(*it);
            if (!attr) {
                continue;
            }
            if (attr->value == typeAttribute) {
                return true;
            }
        }

        return false;
    }

    void RenderJSONVisitor::visit(const MemberElement& e) {
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

    template<typename T>
    const T* getDefault(const T& e) {
        IElement::MemberElementCollection::const_iterator i = e.attributes.find("default");

        if (i == e.attributes.end()) {
            return NULL;
        }

        return TypeQueryVisitor::as<T>((*i)->value.second);
    }

    template<typename T>
    const T* getSample(const T& e) {
        IElement::MemberElementCollection::const_iterator i = e.attributes.find("samples");

        if (i == e.attributes.end()) {
            return NULL;
        }

        ArrayElement* a = TypeQueryVisitor::as<ArrayElement>((*i)->value.second);

        if (!a || a->value.empty()) {
            return NULL;
        }

        return TypeQueryVisitor::as<T>(*(a->value.begin()));
    }

    template<typename T, typename R = typename T::ValueType>
    struct getValue {
        const T& element;

        getValue(const T& e) : element(e) {}

        operator const R*() {
            // FIXME: if value is propageted as first
            // following example will be rendered w/ empty members
            // ```
            // - o
            //     - m1
            //     - m2
            //         - sample
            //             - m1: a
            //             - m2: b
            // ```
            // because `o` has members `m1` and  `m2` , but members has no sed value
            if (!element.empty()) {
                return &element.value;
            }

            if (const T* s = getSample(element)) {
                return &s->value;
            }

            if (const T* d = getDefault(element)) {
                return &d->value;
            }

            if (element.empty() && IsTypeAttribute(element, "nullable")) {
                return NULL;
            }

            return &element.value;
        }
    };

    void RenderJSONVisitor::visit(const ObjectElement& e) {
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

        const ObjectElement::ValueType* val = getValue<ObjectElement>(e);

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

    void RenderJSONVisitor::visit(const ArrayElement& e) {
        RenderJSONVisitor renderer(sos::Base::ArrayType);

        const ArrayElement::ValueType* val = getValue<ArrayElement>(e);

        if (!val) {
            return;
        }

        if (e.element() == "extend") {
            renderer.isExtend = true;
        }

        for (ArrayElement::ValueType::const_iterator it = val->begin();
             it != val->end();
             ++it) {

            if (*it) {
                renderer.visit(*(*it));
            }
        }

        assign(renderer.get());
    }

    void RenderJSONVisitor::visit(const NullElement& e) {}

    void RenderJSONVisitor::visit(const StringElement& e) {
        const StringElement::ValueType* v = getValue<StringElement>(e);

        if (v) {
            assign(sos::String(*v));
        }
    }

    void RenderJSONVisitor::visit(const NumberElement& e) {
        const NumberElement::ValueType* v = getValue<NumberElement>(e);

        if (v) {
            assign(sos::Number(*v));
        }
    }

    void RenderJSONVisitor::visit(const BooleanElement& e) {
        const BooleanElement::ValueType* v = getValue<BooleanElement>(e);

        if (v) {
            assign(sos::Boolean(*v));
        }
    }

    sos::Base RenderJSONVisitor::get() const {
        if (type == sos::Base::ArrayType) {
            return pArr;
        }
        else if (type == sos::Base::ObjectType) {
            return pObj;
        }

        return result;
    }

    std::string RenderJSONVisitor::getString() const {
        sos::SerializeJSON serializer;
        std::stringstream ss;

        serializer.process(get(), ss);
        return ss.str();
    }
}
