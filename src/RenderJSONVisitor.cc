//
//  RenderJSONVisitor.cc
//  libdrafter
//
//  Created by Pavan Kumar Sunkara on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "RenderJSONVisitor.h"
#include "sosJSON.h"
#include "stream.h"

using namespace refract;

namespace drafter
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
    }

    void RenderJSONVisitor::assign(std::string key, sos::Base value) {
        if (!key.empty() && type == sos::Base::ObjectType) {
            pObj.set(key, value);
        }
    }

    void RenderJSONVisitor::visit(const IElement& e) {
        if (e.element() == "object") {
            this->visit(static_cast<const ObjectElement&>(e));
        }
        else if (e.element() == "array") {
            this->visit(static_cast<const ArrayElement&>(e));
        }
        else if (e.element() == "member") {
            this->visit(static_cast<const MemberElement&>(e));
        }
        else if (e.element() == "string") {
            this->visit(static_cast<const StringElement&>(e));
        }
        else if (e.element() == "number") {
            this->visit(static_cast<const NumberElement&>(e));
        }
        else if (e.element() == "boolean") {
            this->visit(static_cast<const BooleanElement&>(e));
        }
        else if (e.element() == "extend") {
            this->extend(static_cast<const ObjectElement&>(e));
        }
    }

    void RenderJSONVisitor::visit(const MemberElement& e) {
        RenderJSONVisitor renderer;
        renderer.visit(*e.value.second);
        assign(static_cast<const StringElement&>(*e.value.first).value, renderer.get());
    }

    void RenderJSONVisitor::visit(const ObjectElement& e) {
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

    void RenderJSONVisitor::extend(const ObjectElement& e) {
        // FIXME: Allow extend to work with arrays

        RenderJSONVisitor renderer(sos::Base::ObjectType);
        std::vector<refract::IElement*>::const_iterator it;

        for (it = e.value.begin(); it != e.value.end(); ++it) {
            renderer.visit(*(*it));
        }

        assign(renderer.get());
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
