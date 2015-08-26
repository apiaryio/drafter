//
//  refract/ComparableVisitor.cc
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#include "Element.h"
#include "Visitors.h"

namespace refract
{

    ComparableVisitor::ComparableVisitor(const std::string& str, const int compare) : compare_to(str), result(false), compare(compare)
    {
    }

    void ComparableVisitor::visit(const MemberElement& e)
    {
        ComparableVisitor v(compare_to);
        if (compare == key && e.value.first) {
            e.value.first->content(v);
        }
        else if (e.value.second) {
            e.value.second->content(v);
        }
        result = v.get();
    }

    void ComparableVisitor::visit(const IElement& e)
    {
        throw LogicError("Fallback impl - behavioration for Base class IElement is not defined");
    }

    bool ComparableVisitor::get() const
    {
        return result;
    }

}; // namespace refract
