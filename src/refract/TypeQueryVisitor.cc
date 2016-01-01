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

    void TypeQueryVisitor::visit(const EnumElement& e)
    {
        typeInfo = Enum;
    }

    void TypeQueryVisitor::visit(const ExtendElement& e)
    {
        typeInfo = Extend;
    }

    TypeQueryVisitor::ElementType TypeQueryVisitor::get() const {
        return typeInfo;
    }

}; // namespace refract
