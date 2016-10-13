//
//  refract/JSONSchemaVisitor.cc
//  librefract
//
//  Created by Vilibald Wanča on 09/11/15.
//  Copyright (c) 2015, 2016 Apiary Inc. All rights reserved.
//

#include "VisitorUtils.h"
#include "sosJSON.h"
#include <sstream>
#include <iostream>
#include <map>

#include "RenderJSONVisitor.h"
#include "JSONSchemaVisitor.h"
#include "SerializeCompactVisitor.h"

namespace refract
{

    template <typename T>
    void CloneMembers(T* a, const RefractElements* val)
    {
        for (RefractElements::const_iterator it = val->begin();
             it != val->end();
             ++it) {

            if ((*it)->empty()) {
                continue;
            }

            RenderJSONVisitor v;
            Visit(v, *(*it));
            IElement *e = v.getOwnership();
            e->renderType(IElement::rCompact);
            a->push_back(e);
        }
    }


    template <typename T>
    void IncludeMembers(const T& element, typename T::ValueType& members)
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
                HandleRefWhenFetchingMembers<T>(*it, members, IncludeMembers<T>);
                continue;
            }

            members.push_back(*it);
        }
    }

    JSONSchemaVisitor::JSONSchemaVisitor(ObjectElement *pDefinitions /*= NULL*/,
                                         bool _fixed /*= false*/,
                                         bool _fixedType /*= false*/)
        : pDefs(pDefinitions), fixed(_fixed), fixedType(_fixedType)
    {
        pObj = new ObjectElement;
        pObj->renderType(IElement::rCompact);

        if (!pDefs) {
            pDefs = new ObjectElement;
            pDefs->renderType(IElement::rCompact);
        }
    }

    JSONSchemaVisitor::~JSONSchemaVisitor()
    {
        if (NULL != pObj) {
            delete pObj;
        }
    }


    void JSONSchemaVisitor::setFixed(bool _fixed)
    {
        fixed = _fixed;
    }

    void JSONSchemaVisitor::setFixedType(bool _fixedType)
    {
        fixedType = _fixedType;
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
        addMember("type",
                  new StringElement(
                      type,
                      IElement::rCompact));
    }

    void JSONSchemaVisitor::addSchemaType(const std::string& type)
    {
        // FIXME: this will not work corretly if "type" attribute will already
        // have more members
        // need to check if type is it is Array and for already pushed types
        MemberElement *m = FindMemberByKey(*pObj, "type");

        if (m && m->value.second) {
            IElement *t = m->value.second;
            ArrayElement *a = new ArrayElement;
            a->push_back(t);
            a->push_back(IElement::Create(type));
            m->value.second = a;
        } else {
            setSchemaType(type);
        }
    }

    void JSONSchemaVisitor::addMember(const std::string& key, IElement *val)
    {
        pObj->push_back(new MemberElement(
                            key,
                            val,
                            IElement::rCompact));
    }

    bool JSONSchemaVisitor::allItemsEmpty(const ArrayElement::ValueType* val)
    {
        return std::find_if(val->begin(),val->end(), std::not1(std::mem_fun(&refract::IElement::empty))) == val->end();
    }

    template<typename T>
    void JSONSchemaVisitor::primitiveType(const T& e)
    {
        const typename T::ValueType *value = GetValue<T>(e);

        if (value) {
            setPrimitiveType(e);

            if (fixed) {
                ArrayElement *a = new ArrayElement;
                a->push_back(IElement::Create(*value));
                addMember("enum", a);
            }
        }
    }

    void JSONSchemaVisitor::operator()(const IElement& e)
    {
        VisitBy(e, *this);
    }

    void JSONSchemaVisitor::operator()(const MemberElement& e)
    {
        JSONSchemaVisitor renderer(pDefs);

        if (e.value.second) {
            if (IsTypeAttribute(e, "fixed") || fixed) {
                renderer.setFixed(true);
            }

            renderer.setFixedType(IsTypeAttribute(e, "fixedType"));
            Visit(renderer, *e.value.second);
        }

        StringElement* str = TypeQueryVisitor::as<StringElement>(e.value.first);
        ExtendElement* ext = TypeQueryVisitor::as<ExtendElement>(e.value.first);

        if (ext) {
            IElement* merged = ext->merge();
            str = TypeQueryVisitor::as<StringElement>(merged);
            if (!str) {
                delete merged;
            }
        }

        if (str) {
            IElement *desc = GetDescription(e);

            if (desc) {
                IElement *d = desc->clone();
                d->renderType(IElement::rCompact);
                renderer.addMember("description", d);
            }

            if (IsTypeAttribute(e, "nullable")) {
                renderer.addSchemaType("null");
            }
            addMember(str->value, renderer.getOwnership());
        }
        else {
            throw LogicError("A property's key in the object is not of type string");
        }

        if (ext && str) {
            delete str; // if Extend -> remove resolved (merged) element
        }
    }

    ObjectElement* JSONSchemaVisitor::definitionFromVariableProperty(JSONSchemaVisitor& renderer)
    {
        ObjectElement* definition = new ObjectElement();

        definition->push_back(new MemberElement(
                                "type",
                                new StringElement("object", IElement::rCompact),
                                IElement::rCompact));
        definition->push_back(new MemberElement(
                                "patternProperties",
                                new ObjectElement(
                                    new MemberElement("", renderer.getOwnership(), IElement::rCompact),
                                    IElement::rCompact),
                                IElement::rCompact));

        return definition;
    }

    ArrayElement* JSONSchemaVisitor::arrayFromProps(std::vector<MemberElement*>& props)
    {
        ArrayElement *a = new ArrayElement;
        a->renderType(IElement::rCompact);

        for (std::vector<MemberElement *>::const_iterator i = props.begin();
             i != props.end();
             ++i) {

            StringElement *str = TypeQueryVisitor::as<StringElement>((*i)->value.first);

            if (str) {
                bool fixedType = IsTypeAttribute(*(*i), "fixedType");
                JSONSchemaVisitor renderer(pDefs, fixed, fixedType);
                Visit(renderer, *(*i)->value.second);

                pDefs->push_back(new MemberElement(
                                    str->value,
                                    definitionFromVariableProperty(renderer),
                                    IElement::rCompact));

                a->push_back(new ObjectElement(
                                 new MemberElement(
                                     "$ref",
                                     new StringElement(
                                         "#/definitions/" + str->value,
                                         IElement::rCompact),
                                     IElement::rCompact),
                                 IElement::rCompact));
            }
        }

        return a;
    }

    void JSONSchemaVisitor::addVariableProps(std::vector<MemberElement*>& props,
                                             ObjectElement *o)
    {
        if (o->empty() && props.size() == 1) {
            StringElement *str = TypeQueryVisitor::as<StringElement>(props[0]->value.first);

            if (str) {
                bool fixedType = IsTypeAttribute(*props.front(), "fixedType");
                JSONSchemaVisitor renderer(pDefs, fixed, fixedType);
                Visit(renderer, *props.front()->value.second);

                pDefs->push_back(new MemberElement(
                                    str->value,
                                    definitionFromVariableProperty(renderer),
                                    IElement::rCompact));

                addMember("$ref", new StringElement("#/definitions/" + str->value, IElement::rCompact));
            }
        }
        else {
            ArrayElement *a = arrayFromProps(props);

            if (!o->empty()) {
                a->push_back(new ObjectElement(
                                 new MemberElement(
                                     "properties",
                                     o,
                                     IElement::rCompact),
                                 IElement::rCompact));
            }

            addMember("allOf", a);
        }
    }


    void JSONSchemaVisitor::operator()(const ObjectElement& e)
    {
        ObjectElement::ValueType val;
        IncludeMembers(e, val);

        ObjectElement *o = new ObjectElement;
        ArrayElement::ValueType reqVals;
        std::vector<MemberElement*> varProps;
        RefractElements oneOfMembers;

        o->renderType(IElement::rCompact);

        if (IsTypeAttribute(e, "fixed")) {
            fixed = true;
        }

        if (IsTypeAttribute(e, "fixedType")) {
            fixedType = true;
        }

        for (std::vector<refract::IElement*>::const_iterator it = val.begin();
             it != val.end();
             ++it) {

            if (!*it) {
                continue;
            }

            TypeQueryVisitor type;
            Visit(type, *(*it));

            switch (type.get()) {
                case TypeQueryVisitor::Member: {
                    MemberElement *mr = static_cast<MemberElement*>(*it);
                    if (IsTypeAttribute(*(*it), "required") ||
                        IsTypeAttribute(*(*it), "fixed") ||
                        ((fixed || fixedType) && !IsTypeAttribute(*(*it), "optional"))) {

                        StringElement *str = TypeQueryVisitor::as<StringElement>(mr->value.first);
                        if (str) {
                            reqVals.push_back(IElement::Create(str->value));
                        }
                    }

                    if (IsVariableProperty(*mr->value.first)) {
                        varProps.push_back(mr);
                    }
                    else {
                        JSONSchemaVisitor renderer(pDefs, fixed);
                        Visit(renderer, *(*it));
                        ObjectElement *o1 = TypeQueryVisitor::as<ObjectElement>(renderer.get());
                        if (!o1->value.empty()) {
                            MemberElement *m1 = TypeQueryVisitor::as<MemberElement>(o1->value[0]->clone());
                            if (m1) {
                                m1->renderType(IElement::rCompact);
                                o->push_back(m1);
                            }

                        }
                    }
                }
                break;

                case TypeQueryVisitor::Select:
                {
                    SelectElement* sel = static_cast<SelectElement*>(*it);

                    // FIXME: there is no valid solution for multiple "SelectElement" in one object.

                    for (SelectElement::ValueType::const_iterator it = sel->value.begin() ; it != sel->value.end() ; ++it) {
                        JSONSchemaVisitor v(pDefs);
                        VisitBy(*(*it), v);
                        oneOfMembers.push_back(v.getOwnership());
                    }
                }
                break;

                default:
                    throw LogicError("Invalid member type of object in MSON definition");
            }
        }

        if (!varProps.empty()) {
            addVariableProps(varProps, o);
            if (o->value.empty()) {
                delete o;
            }
        }
        else {
            setSchemaType("object");
            addMember("properties", o);
        }

        if (!reqVals.empty()) {
            addMember("required", new ArrayElement(reqVals, IElement::rCompact));
        }

        if (!oneOfMembers.empty()) {
            addMember("oneOf", new ArrayElement(oneOfMembers, IElement::rCompact));
        }

        if (fixed || fixedType) {
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

            IElement *elm = items.front();
            JSONSchemaVisitor v(pDefs);
            Visit(v, *elm);

            if (TypeQueryVisitor::as<EnumElement>(elm)){
                v.addMember("enum", elm->clone());
            }
            else if (!TypeQueryVisitor::as<ObjectElement>(elm)) {
                ArrayElement::ValueType enmVals;
                CloneMembers(&enmVals, &items);

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

    void JSONSchemaVisitor::operator()(const ArrayElement& e) {
        const ArrayElement::ValueType* val = GetValue<ArrayElement>(e);

        if (!val || val->empty()) {
            return;
        }

        JSONSchemaVisitor renderer(pDefs);
        setSchemaType("array");

        if (IsTypeAttribute(e, "fixed")) {
            fixed = true;
        }

        if (IsTypeAttribute(e, "fixedType")) {
            fixedType = true;
        }

        if (fixed || fixedType) {
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
                        JSONSchemaVisitor v(pDefs, fixed);
                        Visit(v, *(*it));
                        av.push_back(v.getOwnership());
                    }
                }
            }

            if (!av.empty()) {
                if (av.size() == 1) {
                    addMember("items",av[0]);

                } else {
                    ArrayElement *a = new ArrayElement;
                    a->set(av);
                    addMember("items", a);
                }
            }
        }

        const ArrayElement *def = GetDefault(e);

        if (def && !def->empty()) {
            ArrayElement *d = static_cast<ArrayElement*>(def->clone());
            d->renderType(IElement::rCompact);
            SetRenderFlag(d->value, IElement::rCompact);
            addMember("default", d);
        }
    }

    void JSONSchemaVisitor::operator()(const EnumElement& e) {
        const EnumElement::ValueType* val = GetValue<EnumElement>(e);

        if (!val || val->empty()) {
            return;
        }

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
            const EnumElement* def = GetDefault(e);
            if (!e.empty() || (def && !def->empty())) {
                ArrayElement *a = new ArrayElement;
                a->renderType(IElement::rCompact);
                CloneMembers(a, val);
                setSchemaType(types.begin()->first);
                addMember("enum", a);
            }
        }

        const EnumElement *def = GetDefault(e);

        if (def && !def->empty() && !def->value.empty()) {
            IElement *d = def->value.front()->clone();
            d->renderType(IElement::rCompact);
            addMember("default", d);
        }
    }

    void JSONSchemaVisitor::operator()(const NullElement& e)
    {
        addMember("type", new NullElement);
    }

    void JSONSchemaVisitor::operator()(const StringElement& e)
    {
        primitiveType(e);
    }

    void JSONSchemaVisitor::operator()(const NumberElement& e)
    {
        primitiveType(e);
    }

    void JSONSchemaVisitor::operator()(const BooleanElement& e)
    {
        primitiveType(e);
    }

    void JSONSchemaVisitor::operator()(const ExtendElement& e)
    {
        IElement* merged = e.merge();
        if (!merged) {
            return;
        }

        Visit(*this, *merged);
        delete merged;
    }

    void JSONSchemaVisitor::operator()(const OptionElement& e)
    {
        ObjectElement* props = pObj;
        RefractElements members;
        RefractElements oneOfMembers;
        IncludeMembers(e, members);

        for (OptionElement::ValueType::const_iterator it = members.begin() ; it != members.end() ; ++it) {
            if (SelectElement* sel = TypeQueryVisitor::as<SelectElement>(*it)) {
                for (SelectElement::ValueType::const_iterator it = sel->value.begin() ; it != sel->value.end() ; ++it) {
                    JSONSchemaVisitor v(pDefs);
                    VisitBy(*(*it), v);
                    oneOfMembers.push_back(v.getOwnership());
                }
            }
            else {
                Visit(*this, *(*it));
            }
        }

        pObj = new ObjectElement;
        pObj->renderType(IElement::rCompact);

        addMember("properties", props);
        if (!oneOfMembers.empty()) {
            addMember("oneOf", new ArrayElement(oneOfMembers, IElement::rCompact));
        }
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
        addMember("$schema",
                  new StringElement(
                      "http://json-schema.org/draft-04/schema#",
                      IElement::rCompact));
        setSchemaType("object");

        Visit(*this, e);

        if (!pDefs->empty()) {
            addMember("definitions", pDefs);
        } else {
            delete pDefs;
        }

        sos::SerializeJSON s;
        std::stringstream ss;
        // FIXME: remove SosSerializeCompactVisitor dependency
        SosSerializeCompactVisitor sv(false);
        VisitBy(*pObj, sv);

        s.process(sv.value(), ss);

        return ss.str();
    }
}
