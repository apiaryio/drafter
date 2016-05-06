//
//  refract/TypeQueryVisitor.cc
//  librefract
//
//  Created by Jiri Kratochvil on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#include "Element.h"
#include "TypeQueryVisitor.h"


#define VISIT_IMPL( ELEMENT ) void TypeQueryVisitor::operator()(const ELEMENT ## Element& e) { typeInfo = ELEMENT; }

namespace refract
{
    TypeQueryVisitor::TypeQueryVisitor() : typeInfo(Unknown)
    {
    }

    void TypeQueryVisitor::operator()(const IElement& e)
    {
        TypeQueryVisitor query;
        VisitBy(e, query);
        typeInfo = query.get();
    }


    VISIT_IMPL(Null)
    VISIT_IMPL(String)
    VISIT_IMPL(Number)
    VISIT_IMPL(Boolean)
    VISIT_IMPL(Array)
    VISIT_IMPL(Enum)
    VISIT_IMPL(Member)
    VISIT_IMPL(Object)
    VISIT_IMPL(Extend)
    VISIT_IMPL(Option)
    VISIT_IMPL(Select)

    TypeQueryVisitor::ElementType TypeQueryVisitor::get() const {
        return typeInfo;
    }

}; // namespace refract

#undef VISIT_IMPL
