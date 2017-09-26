//
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
#include <set>

#include "RenderJSONVisitor.h"
#include "JSONSchemaVisitor.h"
#include "SerializeCompactVisitor.h"


#include <assert.h>

namespace refract
{

    template <typename T>
    void CloneMembers(T* a, const RefractElements* val)
    {
        for (const auto& value : *val) {

            if ((value)->empty()) {
                continue;
            }

            RenderJSONVisitor v;
            Visit(v, *value);
            IElement* e = v.getOwnership();
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

        for (auto const& value : *val) {

            if (!value || value->empty()) {
                continue;
            }

            if (value->element() == "ref") {
                HandleRefWhenFetchingMembers<T>(value, members, IncludeMembers<T>);
                continue;
            }

            members.push_back(value);
        }
    }

    JSONSchemaVisitor::JSONSchemaVisitor(
        ObjectElement* pDefinitions /*= nullptr*/, bool _fixed /*= false*/, bool _fixedType /*= false*/)
        : pDefs(pDefinitions), fixed(_fixed), fixedType(_fixedType)
    {
        pObj = new ObjectElement;

        if (!pDefs) {
            pDefs = new ObjectElement;
        }
    }

    JSONSchemaVisitor::~JSONSchemaVisitor()
    {
        if (nullptr != pObj) {
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

    template <>
    void JSONSchemaVisitor::setPrimitiveType(const BooleanElement& e)
    {
        setSchemaType("boolean");
    }

    template <>
    void JSONSchemaVisitor::setPrimitiveType(const StringElement& e)
    {
        setSchemaType("string");
    }

    template <>
    void JSONSchemaVisitor::setPrimitiveType(const NumberElement& e)
    {
        setSchemaType("number");
    }

    void JSONSchemaVisitor::setSchemaType(const std::string& type)
    {
        addMember("type", new StringElement(type));
    }

    void JSONSchemaVisitor::addSchemaType(const std::string& type)
    {
        // FIXME: this will not work corretly if "type" attribute will already
        // have more members. Need to check if type is it is Array and for
        // already pushed types
        MemberElement* m = FindMemberByKey(*pObj, "type");

        if (m && m->value.second) {
            IElement* t = m->value.second;
            ArrayElement* a = new ArrayElement;
            a->push_back(t);
            a->push_back(IElement::Create(type));
            m->value.second = a;
        } else {
            setSchemaType(type);
        }
    }

    void JSONSchemaVisitor::addNullToEnum()
    {
        MemberElement* m = FindMemberByKey(*pObj, "enum");

        if (m && m->value.second) {
            ArrayElement* a = TypeQueryVisitor::as<ArrayElement>(m->value.second);
            a->push_back(new NullElement);
        }
    }

    void JSONSchemaVisitor::addMember(const std::string& key, IElement* val)
    {
        pObj->push_back(new MemberElement(key, val));
    }

    bool JSONSchemaVisitor::allItemsEmpty(const ArrayElement::ValueType* val)
    {
        return std::find_if(val->begin(), val->end(), std::not1(std::mem_fun(&refract::IElement::empty))) == val->end();
    }

    template <typename T>
    void JSONSchemaVisitor::primitiveType(const T& e)
    {
        const typename T::ValueType* value = GetValue<T>(e);

        if (value) {
            setPrimitiveType(e);

            if (fixed) {
                ArrayElement* a = new ArrayElement;
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
            IElement* desc = GetDescription(e);

            if (desc) {
                IElement* d = desc->clone();
                renderer.addMember("description", d);
            }

            if (IsTypeAttribute(e, "nullable")) {
                renderer.addSchemaType("null");
                renderer.addNullToEnum();
            }

            // Check for primitive types
            StringElement* strSecond = TypeQueryVisitor::as<StringElement>(e.value.second);
            NumberElement* numSecond = TypeQueryVisitor::as<NumberElement>(e.value.second);
            BooleanElement* boolSecond = TypeQueryVisitor::as<BooleanElement>(e.value.second);

            if (e.value.second && (strSecond || numSecond || boolSecond)) {
                auto defaultIt = e.value.second->attributes.find("default");

                if (defaultIt != e.value.second->attributes.end()) {
                    renderer.addMember("default", (*defaultIt)->clone());
                }
            }

            addMember(str->value, renderer.getOwnership());
        } else {
            throw LogicError("A property's key in the object is not of type string");
        }

        if (ext && str) {
            delete str; // if Extend -> remove resolved (merged) element
        }
    }

    ObjectElement* JSONSchemaVisitor::definitionFromVariableProperty(JSONSchemaVisitor& renderer)
    {
        ObjectElement* definition = new ObjectElement();

        definition->push_back(new MemberElement("type", new StringElement("object")));

        definition->push_back(
            new MemberElement("patternProperties", new ObjectElement(new MemberElement("", renderer.getOwnership()))));

        return definition;
    }

    ArrayElement* JSONSchemaVisitor::arrayFromProps(std::vector<MemberElement*>& props)
    {
        ArrayElement* a = new ArrayElement;

        for (auto const& prop : props) {

            StringElement* str = TypeQueryVisitor::as<StringElement>(prop->value.first);

            if (str) {
                bool fixedType = IsTypeAttribute(*prop, "fixedType");
                JSONSchemaVisitor renderer(pDefs, fixed, fixedType);
                Visit(renderer, *prop->value.second);

                pDefs->push_back(new MemberElement(str->value, definitionFromVariableProperty(renderer)));

                a->push_back(
                    new ObjectElement(new MemberElement("$ref", new StringElement("#/definitions/" + str->value))));
            }
        }

        return a;
    }

    void JSONSchemaVisitor::addVariableProps(std::vector<MemberElement*>& props, ObjectElement* o)
    {
        if (o->empty() && props.size() == 1) {
            StringElement* str = TypeQueryVisitor::as<StringElement>(props[0]->value.first);

            if (str) {
                bool fixedType = IsTypeAttribute(*props.front(), "fixedType");
                JSONSchemaVisitor renderer(pDefs, fixed, fixedType);
                Visit(renderer, *props.front()->value.second);

                pDefs->push_back(new MemberElement(str->value, definitionFromVariableProperty(renderer)));

                addMember("$ref", new StringElement("#/definitions/" + str->value));
            }
        } else {
            ArrayElement* a = arrayFromProps(props);

            if (!o->empty()) {
                a->push_back(new ObjectElement(new MemberElement("properties", o)));
            }

            addMember("allOf", a);
        }
    }

    void JSONSchemaVisitor::operator()(const ObjectElement& e)
    {
        ObjectElement::ValueType val;
        IncludeMembers(e, val);

        ObjectElement* o = new ObjectElement;
        ArrayElement::ValueType reqVals;
        std::vector<MemberElement*> varProps;
        RefractElements oneOfMembers;

        if (IsTypeAttribute(e, "fixed")) {
            fixed = true;
        }

        if (IsTypeAttribute(e, "fixedType")) {
            fixedType = true;
        }

        processMembers(val, reqVals, varProps, oneOfMembers, o);

        if (!varProps.empty()) {
            addVariableProps(varProps, o);
            if (o->value.empty()) {
                delete o;
            }
        } else {
            setSchemaType("object");
            addMember("properties", o);
        }

        if (!reqVals.empty()) {
            addMember("required", new ArrayElement(reqVals));
        }

        if (!oneOfMembers.empty()) {
            addMember("oneOf", new ArrayElement(oneOfMembers));
        }

        if (fixed || fixedType) {
            addMember("additionalProperties", IElement::Create(false));
        }
    }

    void JSONSchemaVisitor::anyOf(
        std::map<std::string, std::vector<IElement*> >& types, std::vector<std::string>& typesOrder)
    {
        ArrayElement* a = new ArrayElement;

        for (auto const& item : typesOrder) {

            const std::vector<IElement*>& items = types[item];

            IElement* elm = items.front();
            JSONSchemaVisitor v(pDefs);
            Visit(v, *elm);

            if (TypeQueryVisitor::as<EnumElement>(elm)) {
                v.addMember("enum", elm->clone());
            } else if (!TypeQueryVisitor::as<ObjectElement>(elm)) {
                ArrayElement::ValueType enmVals;
                CloneMembers(&enmVals, &items);

                if (!enmVals.empty()) {
                    ArrayElement* enm = new ArrayElement;
                    enm->set(enmVals);
                    v.addMember("enum", enm);
                }
            }

            a->push_back(v.getOwnership());
        }
        addMember("anyOf", a);
    }

    void JSONSchemaVisitor::operator()(const ArrayElement& e)
    {
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

            for (auto const& value : *val) {

                assert(value);
                if (!value) {
                    continue;
                }

                // if all items are just type items then we
                // want them in the schema, otherwise skip
                // empty ones
                if (allEmpty || !value->empty()) {
                    JSONSchemaVisitor v(pDefs, fixed);
                    Visit(v, *value);
                    av.push_back(v.getOwnership());
                }
            }

            if (!av.empty()) {
                if (av.size() == 1) {
                    addMember("items", av[0]);

                } else {
                    ArrayElement* a = new ArrayElement;
                    a->set(av);
                    addMember("items", a);
                }
            }
        }

        const ArrayElement* def = GetDefault(e);

        if (def && !def->empty()) {
            ArrayElement* d = static_cast<ArrayElement*>(def->clone());
            addMember("default", d);
        }
    }

    void JSONSchemaVisitor::operator()(const EnumElement& e)
    {

        RefractElements elms;

        const auto& it = e.attributes.find("enumerations");
        if (it != e.attributes.end()) {
            if (ArrayElement* enums = TypeQueryVisitor::as<ArrayElement>((*it)->value.second)) {
                elms.insert(elms.end(), enums->value.begin(), enums->value.end());
            }
        }

        if (e.value) {
            elms.push_back(e.value);
        }

        if (elms.empty()) {
            return;
        }

        std::map<std::string, std::vector<IElement*> > types;
        std::vector<std::string> typesOrder;

        for (const auto& enumeration : elms) {

            if (enumeration) {
                std::vector<IElement*>& items = types[enumeration->element()];

                if (items.empty()) {
                    typesOrder.push_back(enumeration->element());
                }

                items.push_back(enumeration);
            }
        }

        if (types.size() > 1) {
            anyOf(types, typesOrder);
        } else {
            const EnumElement* def = GetDefault(e);
            if (!elms.empty() || (def && !def->empty())) {
                ArrayElement* a = new ArrayElement;
                CloneMembers(a, &elms);
                setSchemaType(types.begin()->first);
                addMember("enum", a);
            }
        }

        const EnumElement* def = GetDefault(e);

        // this works because "default" is everytime set by value
        // if value will be moved into "enumerations" it need aditional check
        if (def && !def->empty() && !def->value->empty()) {
            IElement* d = def->value->clone();
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
        ArrayElement::ValueType reqVals;
        std::vector<MemberElement*> varProps; // TODO: Add variable properties processing
        RefractElements oneOfMembers;
        IncludeMembers(e, members);

        processMembers(members, reqVals, varProps, oneOfMembers, props);

        pObj = new ObjectElement;

        addMember("properties", props);

        if (!reqVals.empty()) {
            addMember("required", new ArrayElement(reqVals));
        }

        if (!oneOfMembers.empty()) {
            addMember("oneOf", new ArrayElement(oneOfMembers));
        }
    }

    IElement* JSONSchemaVisitor::get()
    {
        return pObj;
    }

    IElement* JSONSchemaVisitor::getOwnership()
    {
        IElement* ret = pObj;
        pObj = nullptr;
        return ret;
    }

    std::string JSONSchemaVisitor::getSchema(const IElement& e)
    {
        addMember("$schema", new StringElement("http://json-schema.org/draft-04/schema#"));
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

    void JSONSchemaVisitor::processMembers(const std::vector<refract::IElement*>& members,
        ArrayElement::ValueType& reqVals,
        std::vector<MemberElement*>& varProps,
        RefractElements& oneOfMembers,
        ObjectElement* o)
    {
        std::set<std::string> required;

        for (const auto& member: members) {
            assert(member);
            if (!member) {
                continue;
            }

            TypeQueryVisitor type;
            Visit(type, *member);


            switch (type.get()) {
                case TypeQueryVisitor::Member: {
                    MemberElement* mr = static_cast<MemberElement*>(member);

                    if (IsTypeAttribute(*member, "required") || IsTypeAttribute(*member, "fixed")
                        || ((fixed || fixedType) && !IsTypeAttribute(*member, "optional"))) {

                        StringElement* str = TypeQueryVisitor::as<StringElement>(mr->value.first);

                        if (str) {
                            required.insert(str->value);
                        }
                    }

                    if (IsVariableProperty(*mr->value.first)) {
                        varProps.push_back(mr);
                    } else {
                        JSONSchemaVisitor renderer(pDefs, fixed);
                        Visit(renderer, *member);
                        ObjectElement* o1 = TypeQueryVisitor::as<ObjectElement>(renderer.get());

                        if (!o1->value.empty()) {
                            MemberElement* m1 = TypeQueryVisitor::as<MemberElement>(o1->value[0]->clone());

                            if (m1) {
                                o->push_back(m1);
                            }
                        }
                    }
                } break;

                case TypeQueryVisitor::Select: {
                    SelectElement* sel = static_cast<SelectElement*>(member);

                    // FIXME: there is no valid solution for multiple "SelectElement" in one object.

                    for (auto const& select : sel->value) {
                        JSONSchemaVisitor v(pDefs);
                        VisitBy(*select, v);
                        oneOfMembers.push_back(v.getOwnership());
                    }
                } break;

                default:
                    throw LogicError("Invalid member type of object in MSON definition");
            }
        }

        std::for_each(required.begin(), required.end(),
                [&reqVals](const std::string value) {
                    reqVals.push_back(IElement::Create(value));
                });
    }
}
