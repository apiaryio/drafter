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
#include "SerializeCompactVisitor.h"

namespace refract
{

    namespace {

        IElement* getEnumValue(const ExtendElement& extend)
        {
            if (extend.empty()) {
                return NULL;
            }

            for (ObjectElement::ValueType::const_reverse_iterator it = extend.value.rbegin();
                 it != extend.value.rend();
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

        template <typename T>
        void FetchMembers(const T& element, typename T::ValueType& members)
        {
            const typename T::ValueType* val = GetValue<T>(element);

            if (!val) {
                return;
            }

            for (typename T::ValueType::const_iterator it = val->begin();
                 it != val->end();
                 ++it) {

                if (!(*it) || (*it)->empty()) {
                    continue;
                }

                if ((*it)->element() == "ref") {
                    IElement::MemberElementCollection::const_iterator found = (*it)->attributes.find("resolved");

                    if (found == (*it)->attributes.end()) {
                        continue;
                    }

                    const T* resolved = TypeQueryVisitor::as<T>((*found)->value.second);

                    if (!resolved) {
                        throw refract::LogicError("Mixin must refer to same type as parent");
                    }

                    FetchMembers(*resolved, members);
                    continue;
                }

                RenderJSONVisitor renderer;
                renderer.visit(*(*it));
                IElement* e = renderer.getOwnership();

                if (!e) {
                    continue;
                }

                e->renderType(IElement::rCompact);
                members.push_back(e);

            }
        }

    }

    RenderJSONVisitor::RenderJSONVisitor() : result(NULL), enumValue(NULL) {}

    RenderJSONVisitor::~RenderJSONVisitor() { 
        if (result) {
            delete result;
        }

        if (enumValue) {
            delete enumValue;
        }
    }

    void RenderJSONVisitor::visit(const IElement& e) {
        e.content(*this);
    }

    void RenderJSONVisitor::visit(const MemberElement& e) {

        RenderJSONVisitor renderer;

        if (e.value.second) {
            if (IsTypeAttribute(e, "nullable") && e.value.second->empty()) {
                renderer.result = new NullElement;
            } 
            else if (IsTypeAttribute(e, "optional") && e.value.second->empty()) {
                return;
            } 
            else {
                renderer.visit(*e.value.second);
                if (!renderer.result) {
                    return;
                }
            }
        }

        if (StringElement* str = TypeQueryVisitor::as<StringElement>(e.value.first)) {
            MemberElement *m = new MemberElement;
            IElement* v= renderer.result ? renderer.getOwnership() : new StringElement;
            m->set(str->value, v);
            result = m;
            result->renderType(IElement::rCompact);
        }
        else {
            throw std::logic_error("A property's key in the object is not of type string");
        }
    }


    void RenderJSONVisitor::visit(const ObjectElement& e) {

        ObjectElement::ValueType members;
        FetchMembers(e, members);
        ObjectElement* o = new ObjectElement;
        o->set(members);
        o->renderType(IElement::rCompact);
        result = o;
    }

    void RenderJSONVisitor::visit(const ArrayElement& e) {

        // FIXME: introduce EnumElement
        if (e.element() == "enum") {

            if (!enumValue) { // there is no enumValue injected from ExtendElement,try to pick value directly

                const ArrayElement::ValueType* val = GetValue<ArrayElement>(e);
                if (val && !val->empty()) {
                    enumValue = val->front()->clone();
                } 
                else {
                    enumValue = new StringElement;
                }
            }

            RenderJSONVisitor renderer;
            enumValue->content(renderer);
            result = renderer.getOwnership();

            delete enumValue;
            enumValue = NULL;

            return;
        }

        ArrayElement::ValueType members;
        FetchMembers(e, members);
        ArrayElement* a = new ArrayElement;
        a->set(members);
        a->renderType(IElement::rCompact);
        result = a;
    }

    void RenderJSONVisitor::visit(const NullElement& e) {
        result = new NullElement;
    }

    template <typename T>
    IElement* getResult(const T& e)
    {
        const typename T::ValueType* v = GetValue<T>(e);

        if (!v) {
            return NULL;
        }

        T* result = IElement::Create(*v);
        result->renderType(IElement::rCompact);

        return result;
    }

    void RenderJSONVisitor::visit(const StringElement& e) {
        result = getResult(e);
    }

    void RenderJSONVisitor::visit(const NumberElement& e) {
        result = getResult(e);
    }

    void RenderJSONVisitor::visit(const BooleanElement& e) {
        result = getResult(e);
    }

    void RenderJSONVisitor::visit(const ExtendElement& e) {

        RenderJSONVisitor renderer;
        IElement* resolved = e.merge();

        if (!resolved) {
            return;
        }
        resolved->renderType(IElement::rCompact);

        if (resolved->element() == "enum") {
            renderer.enumValue = getEnumValue(e);
            if (renderer.enumValue) {
                renderer.enumValue = renderer.enumValue->clone();
            }
        }

        renderer.visit(*resolved);
        result = renderer.getOwnership();

        delete resolved;
    }

   IElement* RenderJSONVisitor::getOwnership() {
       IElement* ret = result;
       result = NULL;
       return ret;
   }

    std::string RenderJSONVisitor::getString() const {
        std::string out;

        if (result) {
            sos::SerializeJSON serializer;
            std::stringstream ss;

            SerializeCompactVisitor s;
            result->content(s);
            serializer.process(s.value(), ss);

            return ss.str();
        }

        return out;
    }
}
