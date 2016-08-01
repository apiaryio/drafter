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
#include "Visitor.h"

#include "TypeQueryVisitor.h"
#include "ComparableVisitor.h"

// this will be removed, refract should not contain reference to other libraries
#include "SourceAnnotation.h"

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
    bool IsVariableProperty(const T& e) {
        IElement::MemberElementCollection::const_iterator i = e.attributes.find("variable");

        if (i == e.attributes.end()) {
            return false;
        }

        BooleanElement* b =  TypeQueryVisitor::as<BooleanElement>((*i)->value.second);
        return b ? b->value : false;
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

    struct GetEnumValue {
        const EnumElement& element;

        GetEnumValue(const EnumElement& e) : element(e) {}

        operator const EnumElement::ValueType*() {
            if (const EnumElement* s = GetSample(element)) {
                return &s->value;
            }

            if (const EnumElement* d = GetDefault(element)) {
                return &d->value;
            }

            if (!element.empty()) {
                return &element.value;
            }

            if (element.empty() && IsTypeAttribute(element, "nullable")) {
                return NULL;
            }

            return &element.value;
        }
    };

    // will be moved into different header (as part of drafter instead of refract)
    template <typename T>
    void CheckMixinParent(refract::IElement* element)
    {
        const T* resolved = TypeQueryVisitor::as<T>(element);

        if (!resolved) {
            throw snowcrash::Error("mixin base type should be the same as parent base type. objects should contain object mixins. arrays should contain array mixins", snowcrash::MSONError);
        }
    }

    // will be moved into different header (as part of drafter instead of refract)
    template<typename T, typename Functor>
    void HandleRefWhenFetchingMembers(const refract::IElement* e, typename T::ValueType& members, const Functor& functor)
    {
        IElement::MemberElementCollection::const_iterator found = e->attributes.find("resolved");

        if (found == e->attributes.end()) {
            return;
        }

        const ExtendElement* extended = TypeQueryVisitor::as<ExtendElement>((*found)->value.second);

        if (!extended) {
            CheckMixinParent<T>((*found)->value.second);
            // We can safely cast it because we are already checking the type in the above line.
            functor(static_cast<const T&>(*(*found)->value.second), members);
            return;
        }

        for (RefractElements::const_iterator it = extended->value.begin();
             it != extended->value.end();
             ++it) {

            CheckMixinParent<T>(*it);
            // We can safely cast it because we are already checking the type in the above line.
            functor(static_cast<const T&>(**it), members);
        }
    }

    template <typename T>
    MemberElement* FindMemberByKey(const T& e,
                                   const std::string& name)
    {
        for (typename T::ValueType::const_iterator it = e.value.begin()
                 ; it != e.value.end()
                 ; ++it ) {

            ComparableVisitor cmp(name, ComparableVisitor::key);
            VisitBy(*(*it), cmp);

            if (cmp.get()) { // key was recognized - it is save to cast to MemberElement
                return static_cast<MemberElement*>(*it);
            }
        }

        return NULL;
    }

    template <typename T>
    T* FindCollectionMemberValue(const IElement::MemberElementCollection& collection, const std::string& key) {
        IElement::MemberElementCollection::const_iterator i = collection.find(key);
        if (i == collection.end()) {
            return NULL;
        }

        return TypeQueryVisitor::as<T>((*i)->value.second);
    }

    std::string GetKeyAsString(const MemberElement& e);

    StringElement* GetDescription(const IElement& e);

    void SetRenderFlag(RefractElements& elements, const IElement::renderFlags flag);

}

#endif /* REFRACT_VISITORUTILS_H */
