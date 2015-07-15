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

    void RenderJSONVisitor::visit(const MemberElement& e) {
        RenderJSONVisitor renderer;
        if (e.value.second) {
            renderer.visit(*e.value.second);
        }

        if (StringElement* str = TypeQueryVisitor::as<StringElement>(e.value.first)) {
            assign(str->value, renderer.get());
        }
        else {
            throw std::logic_error("A property's key in the object is not of type string");
        }
    }

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
        std::vector<refract::IElement*>::const_iterator it;

        if (e.element() == "extend") {
            renderer.isExtend = true;
        }

        for (it = e.value.begin(); it != e.value.end(); ++it) {
            if (*it) {
                renderer.visit(*(*it));
            }
        }

        assign(renderer.get());
    }

    void RenderJSONVisitor::visit(const ArrayElement& e) {
        RenderJSONVisitor renderer(sos::Base::ArrayType);
        std::vector<refract::IElement*>::const_iterator it;

        if (e.element() == "extend") {
            renderer.isExtend = true;
        }

        for (it = e.value.begin(); it != e.value.end(); ++it) {
            if (*it) {
                renderer.visit(*(*it));
            }
        }

        assign(renderer.get());
    }

    void RenderJSONVisitor::visit(const NullElement& e) {}

    void RenderJSONVisitor::visit(const StringElement& e) {
        assign(sos::String(e.value));
    }

    void RenderJSONVisitor::visit(const NumberElement& e) {
        assign(sos::Number(e.value));
    }

    void RenderJSONVisitor::visit(const BooleanElement& e) {
        assign(sos::Boolean(e.value));
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
