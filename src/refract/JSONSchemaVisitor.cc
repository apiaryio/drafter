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

    JSONSchemaVisitor::JSONSchemaVisitor() {
        pObj = new ObjectElement;
        pObj->renderType(IElement::rCompact);
        fixed = false;
    }

    void JSONSchemaVisitor::setFixed(bool fixit) {
        fixed = fixit;
    }

    void JSONSchemaVisitor::setSchemaType(const std::string& type) {
        StringElement *s = new StringElement;
        s->renderType(IElement::rCompact);
        s->set(type);
        addMember("type", s);
    }

    void JSONSchemaVisitor::addMember(const std::string& key, IElement *val) {
        MemberElement *m = new MemberElement;
        m->renderType(IElement::rCompact);
        m->set(key, val);
        pObj->push_back(m);
    }

    bool JSONSchemaVisitor::allItemsEmpty(const ArrayElement::ValueType* val) {
        for (ArrayElement::ValueType::const_iterator i = val->begin();
             i != val->end();
             ++i) {
            if (!(*i)->empty()) return false;
        }
        return true;
    }

    void JSONSchemaVisitor::visit(const IElement& e) {
        e.content(*this);
    }

    void JSONSchemaVisitor::visit(const MemberElement& e) {
        JSONSchemaVisitor renderer;

        if (e.value.second) {
            if (IsTypeAttribute(e, "nullable") && e.value.second->empty()) {
                renderer.visit(*e.value.second);
            } else if (IsTypeAttribute(e, "fixed") || fixed) {
                renderer.setFixed(true);
                renderer.visit(*e.value.second);
            } else {
                renderer.visit(*e.value.second);
            }
        }

        if (StringElement* str = TypeQueryVisitor::as<StringElement>(e.value.first)) {
            IElement *desc = GetDescription(e);
            if (desc) {
                IElement *d = desc->clone();
                d->renderType(IElement::rCompact);
                renderer.addMember("description", d);
            }
            addMember(str->value, renderer.get());
        } else {
            throw std::logic_error("A property's key in the object is not of type string");
        }
    }

    void JSONSchemaVisitor::visit(const ObjectElement& e) {
        const ObjectElement::ValueType* val = GetValue<ObjectElement>(e);
        setSchemaType("object");

        if (!val) {
            return;
        }
        ObjectElement *o = new ObjectElement;
        ArrayElement *required = new ArrayElement;

        o->renderType(IElement::rCompact);
        required->renderType(IElement::rCompact);

        for (std::vector<refract::IElement*>::const_iterator it = val->begin();
             it != val->end();
             ++it) {
            if (*it) {
                if (IsTypeAttribute(*(*it), "required") ||
                    IsTypeAttribute(*(*it), "fixed") ||
                    (fixed && !IsTypeAttribute(*(*it), "optional"))) {
                    MemberElement *mr = TypeQueryVisitor::as<MemberElement>(*it);
                    StringElement *n = TypeQueryVisitor::as<StringElement>(mr->value.first);
                    StringElement *s = new StringElement;
                    s->set(n->value);
                    required->push_back(s);
                }

                JSONSchemaVisitor renderer;
                renderer.setFixed(fixed);
                renderer.visit(*(*it));
                ObjectElement *o1 = TypeQueryVisitor::as<ObjectElement>(renderer.get());
                MemberElement *m1 = TypeQueryVisitor::as<MemberElement>(o1->value[0]);
                o1->renderType(IElement::rCompact);
                m1->renderType(IElement::rCompact);
                o->push_back(m1);
            }
        }

        addMember("properties", o);

        if (!required->value.empty()) addMember("required", required);

        if (fixed) {
            BooleanElement *b = new BooleanElement;
            b->set(false);
            addMember("additionalProperties", b);
        }
    }

    void JSONSchemaVisitor::anyOf(std::map<std::string, std::vector<IElement*> >& types, std::vector<std::string>& typesOrder) {
        ArrayElement *a = new ArrayElement;
        a->renderType(IElement::rCompact);

        for (std::vector<std::string>::const_iterator i = typesOrder.begin();
             i != typesOrder.end();
             ++i) {
            const std::vector<IElement*>& items = types[*i];
            JSONSchemaVisitor v;
            ArrayElement *enm = new ArrayElement;
            IElement *elm = items.front();

            v.visit(*elm);
            if (elm->element() == "enum"){
                v.addMember("enum", elm);
            } else if (elm->element() != "object") {
                for(std::vector<IElement*>::const_iterator i = items.begin();
                    i != items.end();
                    ++i) {
                    IElement *e = NULL;
                    if (!(*i)->empty()) {
                        e = (*i)->clone();
                        e->renderType(IElement::rCompact);
                        enm->push_back(e);
                    }
                }
                if (!enm->empty()) v.addMember("enum", enm);
            }
            a->push_back(v.get());
        }
        addMember("anyOf", a);
    }

    void JSONSchemaVisitor::enumElement(const ArrayElement& e, const ArrayElement::ValueType *val) {

        std::map<std::string, std::vector<IElement*> > types;
        std::vector<std::string> typesOrder;
        for (ArrayElement::ValueType::const_iterator it = val->begin();
             it != val->end();
             ++it) {
            if (*it) {
                std::vector<IElement*>& items = types[(*it)->element()];
                if (items.empty()) typesOrder.push_back((*it)->element());
                items.push_back(*it);
            }
        }

        if (types.size() > 1) {
            anyOf(types, typesOrder);
        } else {
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
        } else {
            JSONSchemaVisitor renderer;
            setSchemaType("array");

            if (fixed) {
                ArrayElement *a = new ArrayElement;
                bool allEmpty = allItemsEmpty(val);

                for (ArrayElement::ValueType::const_iterator it = val->begin();
                     it != val->end();
                     ++it) {
                    if (*it) {
                        // if all items are just type items then we
                        // want them in the schema, otherwise skip
                        // empty ones
                        if (allEmpty) {
                            JSONSchemaVisitor renderer;
                            renderer.setFixed(true);
                            renderer.visit(*(*it));
                            a->push_back(renderer.get());
                        } else if (!(*it)->empty()) {
                            JSONSchemaVisitor renderer;
                            renderer.setFixed(true);
                            renderer.visit(*(*it));
                            a->push_back(renderer.get());
                        }
                    }
                }
                if (!a->empty()) addMember("items", a);
            }

            if (IsTypeAttribute(e, "required")) {
                for (ArrayElement::ValueType::const_iterator it = val->begin();
                     it != val->end();
                     ++it) {

                    if (*it && !(*it)->empty()) {
                        renderer.visit(*(*it));
                    }
                }
                addMember("items", renderer.get());
            }
        }

        IElement *def = DefaultAttribute(e);
        if (def) {
            IElement *d = def->clone();
            d->renderType(IElement::rCompact);
            if (e.element() == "enum") {
                addMember("default", d);
            } else {
                ArrayElement *a = new ArrayElement;
                a->push_back(d);
                addMember("default", a);
            }
        }
    }

    void JSONSchemaVisitor::visit(const NullElement& e) {
        NullElement nil;
        addMember("type", &nil);
    }

    void JSONSchemaVisitor::visit(const StringElement& e) {
        const StringElement::ValueType* v = GetValue<StringElement>(e);
        if (v) {
            setSchemaType("string");
            if (fixed && !e.empty()) {
                ArrayElement *a = new ArrayElement;
                StringElement *s = new StringElement;
                s->set(*v);
                a->push_back(s);
                addMember("enum", a);
            }
        }
    }

    void JSONSchemaVisitor::visit(const NumberElement& e) {
        const NumberElement::ValueType* v = GetValue<NumberElement>(e);
        if (v) {
            setSchemaType("number");
            if (fixed && !e.empty()) {
                ArrayElement *a = new ArrayElement;
                NumberElement *n = new NumberElement;
                n->set(*v);
                a->push_back(n);
                addMember("enum", a);
            }
        }

    }

    void JSONSchemaVisitor::visit(const BooleanElement& e) {
        const BooleanElement::ValueType* v = GetValue<BooleanElement>(e);
        if (v) {
            setSchemaType("boolean");
            if (fixed && !e.empty()) {
                ArrayElement *a = new ArrayElement;
                BooleanElement *b = new BooleanElement;
                b->set(*v);
                a->push_back(b);
                addMember("enum", a);
            }
        }
    }

    IElement* JSONSchemaVisitor::get() {
        return pObj;
    }

    std::string JSONSchemaVisitor::getSchema(const IElement& e) {

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
