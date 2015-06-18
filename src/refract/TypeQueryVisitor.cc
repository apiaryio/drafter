//
//  refract/TypeQueryVisitor.cc
//  librefract
//
//  Created by Jiri Kratochvil on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#include "Element.h"
#include "Visitors.h"

namespace refract
{
    TypeQueryVisitor::TypeQueryVisitor() : typeInfo(Unknown)
    {
    }

    void TypeQueryVisitor::visit(const IElement& e)
    {
        TypeQueryVisitor query;
        e.content(query);
        typeInfo = query.get();
    }

    template<typename E>
    E* TypeQueryVisitor::as(IElement* e) const 
    {
        if(!e) {
            return NULL;
        }

        TypeQueryVisitor tq;
        e->content(tq);

        E type;
        TypeQueryVisitor eq;
        type.content(eq);

        if(eq.typeInfo != tq.typeInfo) {
            return NULL;
        }

        return static_cast<E*>(e);
    }

    // Explicit instancioning of templates to avoi Linker Error
    template NullElement* TypeQueryVisitor::as<NullElement>(IElement*) const;
    template StringElement* TypeQueryVisitor::as<StringElement>(IElement*) const;
    template NumberElement* TypeQueryVisitor::as<NumberElement>(IElement*) const;
    template BooleanElement* TypeQueryVisitor::as<BooleanElement>(IElement*) const;
    template ArrayElement* TypeQueryVisitor::as<ArrayElement>(IElement*) const;
    template MemberElement* TypeQueryVisitor::as<MemberElement>(IElement*) const;
    template ObjectElement* TypeQueryVisitor::as<ObjectElement>(IElement*) const;

    template<typename E>
    const E* TypeQueryVisitor::as(const IElement* e) const 
    {
        TypeQueryVisitor tq;
        e->content(tq);

        E type;
        TypeQueryVisitor eq;
        type.content(eq);

        if(eq.typeInfo != tq.typeInfo) {
            return NULL;
        }

        return static_cast<E*>(&e);
    }

    // Explicit instancioning of templates to avoid Linker Error
    template const NullElement* TypeQueryVisitor::as<const NullElement>(IElement*) const;
    template const StringElement* TypeQueryVisitor::as<const StringElement>(IElement*) const;
    template const NumberElement* TypeQueryVisitor::as<const NumberElement>(IElement*) const;
    template const BooleanElement* TypeQueryVisitor::as<const BooleanElement>(IElement*) const;
    template const ArrayElement* TypeQueryVisitor::as<const ArrayElement>(IElement*) const;
    template const MemberElement* TypeQueryVisitor::as<const MemberElement>(IElement*) const;
    template const ObjectElement* TypeQueryVisitor::as<const ObjectElement>(IElement*) const;


    void TypeQueryVisitor::visit(const NullElement& e)
    {
        typeInfo = Null;
    }

    void TypeQueryVisitor::visit(const StringElement& e)
    {
        typeInfo = String;
    }

    void TypeQueryVisitor::visit(const NumberElement& e)
    {
        typeInfo = Number;
    }

    void TypeQueryVisitor::visit(const BooleanElement& e)
    {
        typeInfo = Boolean;
    }

    void TypeQueryVisitor::visit(const ArrayElement& e)
    {
        typeInfo = Array;
    }

    void TypeQueryVisitor::visit(const MemberElement& e)
    {
        typeInfo = Member;
    }

    void TypeQueryVisitor::visit(const ObjectElement& e)
    {
        typeInfo = Object;
    }

    TypeQueryVisitor::ElementType TypeQueryVisitor::get() const {
        return typeInfo;
    }

}; // namespace refract
