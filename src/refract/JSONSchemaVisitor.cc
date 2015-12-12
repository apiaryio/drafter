//
//  refract/JSONSchemaVisitor.cc
//  librefract
//
//  Created by Vilibald Wanča on 09/11/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "VisitorUtils.h"
#include "sosJSON.h"
#include <sstream>
#include <iostream>
#include <map>

namespace refract
{

    JSONSchemaVisitor::JSONSchemaVisitor(bool fixit /*= false*/) : fixed(fixit)
    {
        pObj = new ObjectElement;
        pObj->renderType(IElement::rCompact);
    }

    JSONSchemaVisitor::~JSONSchemaVisitor()
    {
        if (NULL != pObj) {
            delete pObj;
        }
    }

    void JSONSchemaVisitor::setFixed(bool fixit)
    {
        fixed = fixit;
    }

    template<>
    void JSONSchemaVisitor::setPrimitiveType(const BooleanElement& e)
    {
        setSchemaType("boolean");
    }

    template<>
    void JSONSchemaVisitor::setPrimitiveType(const StringElement& e)
    {
        setSchemaType("string");
    }

    template<>
    void JSONSchemaVisitor::setPrimitiveType(const NumberElement& e)
    {
        setSchemaType("number");
    }

    void JSONSchemaVisitor::setSchemaType(const std::string& type)
    {
        StringElement *s = new StringElement;

        s->renderType(IElement::rCompact);
        s->set(type);
        addMember("type", s);
    }

    void JSONSchemaVisitor::addMember(const std::string& key, IElement *val)
    {
        MemberElement *m = new MemberElement;

        m->renderType(IElement::rCompact);
        m->set(key, val);
        pObj->push_back(m);
    }

    bool JSONSchemaVisitor::allItemsEmpty(const ArrayElement::ValueType* val)
    {
        for (ArrayElement::ValueType::const_iterator i = val->begin();
             i != val->end();
             ++i) {

            if (!(*i)->empty()) {
                return false;
            }
        }

        return true;
    }

    template<typename T>
    void JSONSchemaVisitor::primitiveType(const T& e)
    {
        const typename T::ValueType *v = GetValue<T>(e);

        if (v) {
            setPrimitiveType(e);

            if (fixed && !e.empty()) {
                ArrayElement *a = new ArrayElement;
                a->push_back(IElement::Create(*v));
                addMember("enum", a);
            }
        }
    }

    void JSONSchemaVisitor::visit(const IElement& e)
    {
        e.content(*this);
    }

    void JSONSchemaVisitor::visit(const MemberElement& e)
    {
        JSONSchemaVisitor renderer;

        if (e.value.second) {
           if (IsTypeAttribute(e, "fixed") || fixed) {
                renderer.setFixed(true);
            }
           renderer.visit(*e.value.second);
        }

        if (StringElement* str = TypeQueryVisitor::as<StringElement>(e.value.first)) {
            IElement *desc = GetDescription(e);

            if (desc) {
                IElement *d = desc->clone();
                d->renderType(IElement::rCompact);
                renderer.addMember("description", d);
            }

            addMember(str->value, renderer.getOwnership());
        }
        else {
            throw std::logic_error("A property's key in the object is not of type string");
        }
    }

    void JSONSchemaVisitor::visit(const ObjectElement& e)
    {
        const ObjectElement::ValueType* val = GetValue<ObjectElement>(e);
        setSchemaType("object");

        if (!val) {
            return;
        }

        ObjectElement *o = new ObjectElement;
        ArrayElement::ValueType reqVals;

        o->renderType(IElement::rCompact);

        for (std::vector<refract::IElement*>::const_iterator it = val->begin();
             it != val->end();
             ++it) {

            if (*it) {
                if (IsTypeAttribute(*(*it), "required") ||
                    IsTypeAttribute(*(*it), "fixed") ||
                    (fixed && !IsTypeAttribute(*(*it), "optional"))) {

                    MemberElement *mr = TypeQueryVisitor::as<MemberElement>(*it);
                    StringElement *n = TypeQueryVisitor::as<StringElement>(mr->value.first);
                    reqVals.push_back(IElement::Create(n->value));
                }

                JSONSchemaVisitor renderer(fixed);
                renderer.visit(*(*it));

                ObjectElement *o1 = TypeQueryVisitor::as<ObjectElement>(renderer.get());
                // the o1->value is never emtpy as the object returned
                // by JSONSchema visitor always has a member, at least a type
                // key for primitive types
                MemberElement *m1 = TypeQueryVisitor::as<MemberElement>(o1->value[0]->clone());

                m1->renderType(IElement::rCompact);
                o->push_back(m1);
            }
        }

        addMember("properties", o);

        if (!reqVals.empty()) {
            ArrayElement *a = new ArrayElement;
            a->renderType(IElement::rCompact);
            a->set(reqVals);
            addMember("required", a);
        }

        if (fixed) {
            addMember("additionalProperties", IElement::Create(false));
        }
    }

    void JSONSchemaVisitor::anyOf(std::map<std::string, std::vector<IElement*> >& types, std::vector<std::string>& typesOrder)
    {
        ArrayElement *a = new ArrayElement;
        a->renderType(IElement::rCompact);

        for (std::vector<std::string>::const_iterator i = typesOrder.begin();
             i != typesOrder.end();
             ++i) {

            const std::vector<IElement*>& items = types[*i];
            JSONSchemaVisitor v;
            IElement *elm = items.front();

            v.visit(*elm);

            if (elm->element() == "enum"){
                v.addMember("enum", elm->clone());
            }
            else if (NULL == TypeQueryVisitor::as<ObjectElement>(elm)) {
                ArrayElement::ValueType enmVals;
                for (std::vector<IElement*>::const_iterator i = items.begin();
                    i != items.end();
                    ++i) {

                    if (!(*i)->empty()) {
                        IElement *e = (*i)->clone();
                        e->renderType(IElement::rCompact);
                        enmVals.push_back(e);
                    }
                }

                if (!enmVals.empty()) {
                    ArrayElement *enm = new ArrayElement;
                    enm->set(enmVals);
                    v.addMember("enum", enm);
                }
            }

            a->push_back(v.getOwnership());
        }
        addMember("anyOf", a);
    }

    void JSONSchemaVisitor::enumElement(const ArrayElement& e, const ArrayElement::ValueType *val)
    {
        std::map<std::string, std::vector<IElement*> > types;
        std::vector<std::string> typesOrder;

        for (ArrayElement::ValueType::const_iterator it = val->begin();
             it != val->end();
             ++it) {

            if (*it) {
                std::vector<IElement*>& items = types[(*it)->element()];

                if (items.empty()) {
                    typesOrder.push_back((*it)->element());
                }

                items.push_back(*it);
            }
        }

        if (types.size() > 1) {
            anyOf(types, typesOrder);
        }
        else {
            if (!e.empty() || DefaultAttribute(e) != NULL) {
                ArrayElement *a = new ArrayElement;
                a->renderType(IElement::rCompact);

                for (ArrayElement::ValueType::const_iterator it = val->begin();
                     it != val->end();
                     ++it) {

                    IElement *e = (*it)->clone();
                    e->renderType(IElement::rCompact);
                    a->push_back(e);
                }

                setSchemaType(types.begin()->first);
                addMember("enum", a);
            }
        }
    }


    void JSONSchemaVisitor::visit(const ArrayElement& e) {
        const ArrayElement::ValueType* val = GetValue<ArrayElement>(e);

        if (!val || val->empty()) {
            return;
        }

        if (e.element() == "enum") {
            enumElement(e, val);
        }
        else {
            JSONSchemaVisitor renderer;
            setSchemaType("array");

            if (fixed) {
                ArrayElement::ValueType av;
                bool allEmpty = allItemsEmpty(val);

                for (ArrayElement::ValueType::const_iterator it = val->begin();
                     it != val->end();
                     ++it) {

                    if (*it) {
                        // if all items are just type items then we
                        // want them in the schema, otherwise skip
                        // empty ones
                        if (allEmpty || !(*it)->empty()) {
                            JSONSchemaVisitor v(true);

                            v.visit(*(*it));
                            av.push_back(v.getOwnership());
                        }
                    }
                }

                if (!av.empty()) {
                    ArrayElement *a = new ArrayElement;
                    a->set(av);
                    addMember("items", a);
                }
            }

            if (IsTypeAttribute(e, "required")) {
                for (ArrayElement::ValueType::const_iterator it = val->begin();
                     it != val->end();
                     ++it) {

                    if (*it && !(*it)->empty()) {
                        renderer.visit(*(*it));
                    }
                }

                addMember("items", renderer.getOwnership());
            }
        }

        IElement *def = DefaultAttribute(e);

        if (def) {
            IElement *d = def->clone();
            d->renderType(IElement::rCompact);

            if (e.element() == "enum") {
                addMember("default", d);
            }
            else {
                ArrayElement *a = new ArrayElement;
                a->push_back(d);
                addMember("default", a);
            }
        }
    }

    void JSONSchemaVisitor::visit(const NullElement& e)
    {
        addMember("type", new NullElement);
    }

    void JSONSchemaVisitor::visit(const StringElement& e)
    {
        primitiveType(e);
    }

    void JSONSchemaVisitor::visit(const NumberElement& e)
    {
        primitiveType(e);
    }

    void JSONSchemaVisitor::visit(const BooleanElement& e)
    {
        primitiveType(e);
    }

    IElement* JSONSchemaVisitor::get()
    {
        return pObj;
    }

    IElement* JSONSchemaVisitor::getOwnership()
    {
        IElement* ret = pObj;
        pObj = NULL;
        return ret;
    }

    std::string JSONSchemaVisitor::getSchema(const IElement& e)
    {
        StringElement *str = new StringElement;

        str->renderType(IElement::rCompact);
        str->set("http://json-schema.org/draft-04/schema#");
        addMember("$schema", str);
        setSchemaType("object");

        visit(e);

        sos::SerializeJSON s;
        std::stringstream ss;
        SerializeCompactVisitor sv;

        pObj->content(sv);
        s.process(sv.value(), ss);

        return ss.str();
    }
}
