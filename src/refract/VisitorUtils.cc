//
//  refract/VisitorUtils.cc
//  librefract
//
//  Created by Vilibald Wanča on 09/11/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#include "Element.h"
#include "VisitorUtils.h"

namespace refract
{

    StringElement* GetDescription(const IElement& e)
    {
        IElement::MemberElementCollection::const_iterator i = e.meta.find("description");

        if (i == e.meta.end()) {
            return NULL;
        }

        return TypeQueryVisitor::as<StringElement>((*i)->value.second);
    }

    void SetRenderFlag(RefractElements& elements, const IElement::renderFlags flag) {
        std::for_each(elements.begin(), elements.end(),
                 std::bind2nd(std::mem_fun((void (refract::IElement::*)(const refract::IElement::renderFlags))&refract::IElement::renderType), flag));
    }

    std::string GetKeyAsString(const MemberElement& e)
    {

        IElement* element = e.value.first;

        if (StringElement* str = TypeQueryVisitor::as<StringElement>(element)) {
            return str->value;
        }

        if (ExtendElement* ext = TypeQueryVisitor::as<ExtendElement>(element)) {
            IElement* merged = ext->merge();

            if (StringElement* str = TypeQueryVisitor::as<StringElement>(merged)) {

                std::string key = str->value;
                if (key.empty()) {
                    const std::string* k = GetValue<StringElement>(*str);
                    if (k) {
                        key = *k;
                    }
                }
                delete merged;

                return key;

            }

            delete merged;
        }

        return std::string();
    }

}
