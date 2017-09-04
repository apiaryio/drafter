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

#include "RenderJSONVisitor.h"

namespace refract
{

    namespace {

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

                if (RefElement* ref = TypeQueryVisitor::as<RefElement>(*it)) {
                    HandleRefWhenFetchingMembers<T>(ref, members, FetchMembers<T>);
                    continue;
                }
                else if (SelectElement* select = TypeQueryVisitor::as<SelectElement>(*it)) {
                    if (select->value.empty() || !(*select->value.begin())) {
                        continue;
                    }

                    FetchMembers(*(*select->value.begin()), members);
                    continue;
                }

                RenderJSONVisitor renderer;
                Visit(renderer, *(*it));
                IElement* e = renderer.getOwnership();

                if (!e) {
                    continue;
                }

                members.push_back(e);
            }
        }

    }

    RenderJSONVisitor::RenderJSONVisitor() : result(nullptr) {}

    RenderJSONVisitor::~RenderJSONVisitor() {
        if (result) {
            delete result;
        }
    }

    void RenderJSONVisitor::operator()(const IElement& e)
    {
        VisitBy(e, *this);
    }

    void RenderJSONVisitor::operator()(const MemberElement& e)
    {

        std::string key = GetKeyAsString(e);

        if (key.empty()) {
            return;
        }

        RenderJSONVisitor renderer;

        if (e.value.second) {
            if (EnumElement* enm = TypeQueryVisitor::as<EnumElement>(e.value.second)) {
                // We nned nadle Enum individualy because of attr["enumerations"]
                Visit(renderer, *enm);
            }
            else if (IsTypeAttribute(e, "nullable") && e.value.second->empty()) {
                renderer.result = new NullElement;
            } else if (IsTypeAttribute(e, "optional") && e.value.second->empty()) {
                return;
            } else {
                Visit(renderer, *e.value.second);
            }
        }

        if (!renderer.result) {
            return;
        }

        result = new MemberElement(key, renderer.result ? renderer.getOwnership() : new StringElement);
    }

    void RenderJSONVisitor::operator()(const ObjectElement& e) {
        ObjectElement::ValueType members;
        FetchMembers(e, members);
        ObjectElement* o = new ObjectElement;
        o->set(members);
        result = o;
    }

    void RenderJSONVisitor::operator()(const EnumElement& e) {

        const IElement* val = GetValue<EnumElement>(e);
        if (val && !val->empty()) {
            val = val->clone();
        }
        else {
            val = new StringElement;
        }

        RenderJSONVisitor renderer;
        VisitBy(*val, renderer);
        result = renderer.getOwnership();

        delete val;
    }

    void RenderJSONVisitor::operator()(const ArrayElement& e)
    {
        ArrayElement::ValueType members;
        FetchMembers(e, members);
        ArrayElement* a = new ArrayElement;
        a->set(members);
        result = a;
    }

    void RenderJSONVisitor::operator()(const NullElement& e)
    {
        result = new NullElement;
    }

    template <typename T>
    IElement* getResult(const T& e)
    {
        const typename T::ValueType* v = GetValue<T>(e);

        if (!v) {
            return nullptr;
        }

        T* result = IElement::Create(*v);

        return result;
    }

    void RenderJSONVisitor::operator()(const StringElement& e)
    {
        result = getResult(e);
    }

    void RenderJSONVisitor::operator()(const NumberElement& e)
    {
        result = getResult(e);
    }

    void RenderJSONVisitor::operator()(const BooleanElement& e)
    {
        result = getResult(e);
    }

    void RenderJSONVisitor::operator()(const ExtendElement& e)
    {

        RenderJSONVisitor renderer;
        IElement* merged = e.merge();

        if (!merged) {
            return;
        }

        Visit(renderer, *merged);
        result = renderer.getOwnership();

        delete merged;
    }

   IElement* RenderJSONVisitor::getOwnership() {
       IElement* ret = result;
       result = nullptr;
       return ret;
   }

    std::string RenderJSONVisitor::getString() const
    {
        std::string out;

        if (result) {
            sos::SerializeJSON serializer;
            std::stringstream ss;

            // FIXME: remove SosSerializeCompactVisitor dependency
            SosSerializeCompactVisitor s;
            VisitBy(*result, s);
            serializer.process(s.value(), ss);

            return ss.str();
        }

        return out;
    }
}
