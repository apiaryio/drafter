//
//  refract/ComparableVisitor.cc
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#include "Element.h"
#include "Exception.h"
#include "ComparableVisitor.h"

namespace refract
{

    ComparableVisitor::ComparableVisitor(const std::string& str, const int compare)
        : compare_to(str), result(false), compare(compare)
    {
    }

    void ComparableVisitor::operator()(const MemberElement& e)
    {
        ComparableVisitor cv(compare_to); // OPTIM: reuse current visitor
        Visitor visitor(cv);

        if (compare == key) {
            if (const IElement* k = e.get().key())
                k->content(visitor);
        } else if (const IElement* v = e.get().value()) {
            v->content(visitor);
        }

        result = cv.get();
    }

    void ComparableVisitor::operator()(const IElement& e)
    {
        throw LogicError("Fallback impl - behavioration for Base class IElement is not defined");
    }

    bool ComparableVisitor::get() const
    {
        return result;
    }

} // namespace refract
