//
//  refract/VisitorUtils.cc
//  librefract
//
//  Created by Vilibald Wanča on 09/11/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#include "Element.h"
#include "Visitors.h"
#include "VisitorUtils.h"

namespace refract
{

    ArrayElement* DefaultAttribute(const IElement& e)
    {
        IElement::MemberElementCollection::const_iterator i = e.attributes.find("default");

        if (i == e.attributes.end()) {
            return NULL;
        }

        ArrayElement* vals = TypeQueryVisitor::as<ArrayElement>((*i)->value.second);

        if (!vals || vals->empty()) {
            return NULL;
        }

        return vals;
    }

    IElement* GetFirstSample(const IElement& e)
    {
        IElement::MemberElementCollection::const_iterator i = e.attributes.find("samples");

        if (i == e.attributes.end()) {
            return NULL;
        }

        ArrayElement* vals = TypeQueryVisitor::as<ArrayElement>((*i)->value.second);

        if (!vals || vals->empty()) {
            return NULL;
        }

        return vals->value[0];
    }

    StringElement* GetDescription(const IElement& e)
    {
        IElement::MemberElementCollection::const_iterator i = e.meta.find("description");

        if (i == e.meta.end()) {
            return NULL;
        }

        return TypeQueryVisitor::as<StringElement>((*i)->value.second);
    }

}
