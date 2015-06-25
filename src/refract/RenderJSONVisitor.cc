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
#include <iostream>

namespace refract
{

    RenderJSONVisitor::RenderJSONVisitor(const sos::Base::Type& type_)
    : type(type_) {}

    RenderJSONVisitor::RenderJSONVisitor()
    : type(sos::Base::UndefinedType) {}

    void RenderJSONVisitor::assign(sos::Base value) {
        if (type == sos::Base::ArrayType) {
            pArr.push(value);
        }
        else if (type == sos::Base::UndefinedType) {
            result = value;
        }
        else if (type == sos::Base::ObjectType) {

            for (sos::KeyValues::iterator it = value.object().begin();
                 it != value.object().end();
                 ++it) {

                pObj.set(it->first, it->second);
            }
        }
    }

    void RenderJSONVisitor::assign(std::string key, sos::Base value) {
        if (!key.empty() && type == sos::Base::ObjectType) {
            pObj.set(key, value);
        }
    }

    void RenderJSONVisitor::visit(const IElement& e) {
        e.content(*this);
    }

    void RenderJSONVisitor::visit(const MemberElement& e) {
        RenderJSONVisitor renderer;
        renderer.visit(*e.value.second);
        assign(static_cast<const StringElement&>(*e.value.first).value, renderer.get());
    }

    void RenderJSONVisitor::visit(const ObjectElement& e) {
        if (e.element() == "ref") {
            return;
        }

        RenderJSONVisitor renderer(sos::Base::ObjectType);
        std::vector<refract::IElement*>::const_iterator it;

        for (it = e.value.begin(); it != e.value.end(); ++it) {
            renderer.visit(*(*it));
        }

        assign(renderer.get());
    }

    void RenderJSONVisitor::visit(const ArrayElement& e) {
        RenderJSONVisitor renderer(sos::Base::ArrayType);
        std::vector<refract::IElement*>::const_iterator it;

        for (it = e.value.begin(); it != e.value.end(); ++it) {
            renderer.visit(*(*it));
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
