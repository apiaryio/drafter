//
//  refract/VisitorUtils.h
//  librefract
//
//  Created by Vilibald Wanča on 09/11/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_VISITORUTILS_H
#define REFRACT_VISITORUTILS_H

#include "Element.h"
#include "Visitors.h"

namespace refract
{
    template<typename T>
    bool IsTypeAttribute(const T& e, std::string typeAttribute) {
        IElement::MemberElementCollection::const_iterator ta = e.attributes.find("typeAttributes");

        if (ta == e.attributes.end()) {
            return false;
        }

        ArrayElement* attrs = TypeQueryVisitor::as<ArrayElement>((*ta)->value.second);

        if (!attrs) {
            return false;
        }

        for (ArrayElement::ValueType::const_iterator it = attrs->value.begin() ; it != attrs->value.end() ; ++it ) {
            StringElement* attr = TypeQueryVisitor::as<StringElement>(*it);
            if (!attr) {
                continue;
            }
            if (attr->value == typeAttribute) {
                return true;
            }
        }

        return false;
    }


    template<typename T>
    const T* GetDefault(const T& e) {
        IElement::MemberElementCollection::const_iterator i = e.attributes.find("default");

        if (i == e.attributes.end()) {
            return NULL;
        }

        return TypeQueryVisitor::as<T>((*i)->value.second);
    }

    template<typename T>
    const T* GetSample(const T& e) {
        IElement::MemberElementCollection::const_iterator i = e.attributes.find("samples");

        if (i == e.attributes.end()) {
            return NULL;
        }

        ArrayElement* a = TypeQueryVisitor::as<ArrayElement>((*i)->value.second);

        if (!a || a->value.empty()) {
            return NULL;
        }

        return TypeQueryVisitor::as<T>(*(a->value.begin()));
    }

    template<typename T, typename R = typename T::ValueType>
    struct GetValue {
        const T& element;

        GetValue(const T& e) : element(e) {}

        operator const R*() {
            // FIXME: if value is propageted as first
            // following example will be rendered w/ empty members
            // ```
            // - o
            //     - m1
            //     - m2
            //         - sample
            //             - m1: a
            //             - m2: b
            // ```
            // because `o` has members `m1` and  `m2` , but members has no sed value
            if (!element.empty()) {
                return &element.value;
            }

            if (const T* s = GetSample(element)) {
                return &s->value;
            }

            if (const T* d = GetDefault(element)) {
                return &d->value;
            }

            if (element.empty() && IsTypeAttribute(element, "nullable")) {
                return NULL;
            }

            return &element.value;
        }
    };


    template <typename T>
    MemberElement* FindMemberByKey(const T& e,
                                   const std::string& name)
    {
        for (typename T::ValueType::const_iterator it = e.value.begin()
                 ; it != e.value.end()
                 ; ++it ) {

            ComparableVisitor cmp(name, ComparableVisitor::key);
            (*it)->content(cmp);

            if (cmp.get()) { // key was recognized - it is save to cast to MemberElement
                return static_cast<MemberElement*>(*it);
            }
        }

        return NULL;
    }

    IElement* GetFirstSample(const IElement& e);

    StringElement* GetDescription(const IElement& e);

    void SetRenderFlag(RefractElements& elements, const IElement::renderFlags flag);

}

#endif /* REFRACT_VISITORUTILS_H */
