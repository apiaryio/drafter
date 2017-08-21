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

    namespace
    {

        const ArrayElement* GetEnumerations(const EnumElement& e) {

            IElement::MemberElementCollection::const_iterator i = e.attributes.find("enumerations");

            if (i == e.attributes.end()) {
                return NULL;
            }

            return TypeQueryVisitor::as<ArrayElement>((*i)->value.second);
        }

        IElement* GetEnumValue(const ExtendElement& extend)
        {
            if (extend.empty()) {
                return NULL;
            }

            for (ExtendElement::ValueType::const_reverse_iterator it = extend.value.rbegin(); it != extend.value.rend();
                 ++it) {

                const EnumElement* element = TypeQueryVisitor::as<EnumElement>(*it);

                if (!element) {
                    continue;
                }

                const EnumElement::ValueType* item = GetValue<EnumElement>(*element);

                if (item) {
                    return *item;
                }
            }

            return NULL;
        }

        const IElement* GetEnumValue(const EnumElement& element) {
            if (const EnumElement* s = GetSample(element)) {
                return GetEnumValue(*s);
            }

            if (const EnumElement* d = GetDefault(element)) {
                return GetEnumValue(*d);
            }

            if (!element.empty()) {
                return element.value;
            }

            if (const ArrayElement* e = GetEnumerations(element)) {
                if (e && !e->empty()) {
                    for (const auto& item : e->value) {
                        if (!item) {
                            continue;
                        }

                        // We need hadle Enum individualy because of attr["enumerations"]
                        if (EnumElement* val = TypeQueryVisitor::as<EnumElement>(item)) {
                            const IElement* ret = GetEnumValue(*val);
                            if (ret) {
                                return ret;
                            }
                        }

                        if (!item->empty()) {
                            return  item;
                        }
                    }
                }
            }

            if (element.empty() && IsTypeAttribute(element, "nullable")) {
                return NULL;
            }

            return element.value;
        }

        template <typename T>
        void FetchMembers(const T& element, typename T::ValueType& members)
        {
            const typename T::ValueType* val = GetValue<T>(element);

            if (!val) {
                return;
            }

            for (typename T::ValueType::const_iterator it = val->begin(); it != val->end(); ++it) {

                if (!(*it) || (*it)->empty()) {
                    continue;
                }

                if (RefElement* ref = TypeQueryVisitor::as<RefElement>(*it)) {
                    HandleRefWhenFetchingMembers<T>(ref, members, FetchMembers<T>);
                    continue;
                } else if (SelectElement* select = TypeQueryVisitor::as<SelectElement>(*it)) {
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

    RenderJSONVisitor::RenderJSONVisitor() : result(NULL), enumValue(NULL)
    {
    }

    RenderJSONVisitor::~RenderJSONVisitor()
    {
        if (result) {
            delete result;
        }

        if (enumValue) {
            delete enumValue;
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

    void RenderJSONVisitor::operator()(const ObjectElement& e)
    {
        ObjectElement::ValueType members;
        FetchMembers(e, members);
        ObjectElement* o = new ObjectElement;
        o->set(members);
        result = o;
    }

    void RenderJSONVisitor::operator()(const EnumElement& e)
    {

        if (!enumValue) { // there is no enumValue injected from ExtendElement,try to pick value directly

            const IElement* val = GetEnumValue(e);
            if (val && !val->empty()) {
                enumValue = val->clone();
            }
            else {
                enumValue = new StringElement;
            }
        }
        

        RenderJSONVisitor renderer;
        VisitBy(*enumValue, renderer);
        result = renderer.getOwnership();

        delete enumValue;
        enumValue = NULL;
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
            return NULL;
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

        if (TypeQueryVisitor::as<EnumElement>(merged)) {
            renderer.enumValue = GetEnumValue(e);
            if (renderer.enumValue) {
                renderer.enumValue = renderer.enumValue->clone();
            }
        }

        Visit(renderer, *merged);
        result = renderer.getOwnership();

        delete merged;
    }

    IElement* RenderJSONVisitor::getOwnership()
    {
        IElement* ret = result;
        result = NULL;
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
