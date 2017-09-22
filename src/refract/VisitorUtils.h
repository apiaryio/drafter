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
    template <typename T>
    bool IsTypeAttribute(const T& e, std::string typeAttribute)
    {
        auto ta = e.attributes.find("typeAttributes");

        if (ta == e.attributes.end()) {
            return false;
        }

        ArrayElement* attrs = TypeQueryVisitor::as<ArrayElement>((*ta)->value.second);

        if (!attrs) {
            return false;
        }

        for (auto const& value: attrs->value) {
            StringElement* attr = TypeQueryVisitor::as<StringElement>(value);
            if (!attr) {
                continue;
            }
            if (attr->value == typeAttribute) {
                return true;
            }
        }

        return false;
    }

    template <typename T>
    bool IsVariableProperty(const T& e)
    {
        auto const var = e.attributes.find("variable");

        if (var == e.attributes.end()) {
            return false;
        }

        const BooleanElement* b = TypeQueryVisitor::as<BooleanElement>((*var)->value.second);
        return b ? b->value : false;
    }

    template <typename T>
    const T* GetDefault(const T& e)
    {
        auto const dflt = e.attributes.find("default");

        if (dflt == e.attributes.end()) {
            return NULL;
        }

        return TypeQueryVisitor::as<T>((*dflt)->value.second);
    }

    template <typename T>
    const T* GetSample(const T& e)
    {
        auto const i = e.attributes.find("samples");

        if (i == e.attributes.end()) {
            return NULL;
        }

        ArrayElement* a = TypeQueryVisitor::as<ArrayElement>((*i)->value.second);

        if (!a || a->value.empty()) {
            return NULL;
        }

        return TypeQueryVisitor::as<T>(*(a->value.begin()));
    }

    template <typename T, typename R = typename T::ValueType>
    struct GetValue {
        const T& element;

        GetValue(const T& e) : element(e)
        {
        }

        operator const R*()
        {
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

    template <>
    struct GetValue<refract::EnumElement, typename refract::EnumElement::ValueType> {
        using EnumElement = refract::EnumElement;
        const EnumElement& element;

        GetValue(const EnumElement& e) : element(e)
        {
        }

        operator const IElement*()
        {
            return GetEnumValue(element);
        }

        IElement* GetEnumValue(const EnumElement& element)
        {
            if (const EnumElement* s = GetSample(element)) {
                return GetEnumValue(*s);
            }

            if (const EnumElement* d = GetDefault(element)) {
                return GetEnumValue(*d);
            }

            if (element.value) {
                return element.value;
            }

            if (const ArrayElement* e = GetEnumerations(element)) {
                if (e && !e->empty()) {
                    for (const auto& item : e->value) {
                        if (!item) {
                            continue;
                        }

                        // We need to hadle Enum individualy because of attr["enumerations"]
                        if (EnumElement* val = TypeQueryVisitor::as<EnumElement>(item)) {
                            IElement* ret = GetEnumValue(*val);
                            if (ret) {
                                return ret;
                            }
                        }

                        if (!item->empty()) {
                            return item;
                        }
                    }
                }
            }

            return element.value;
        }

        const ArrayElement* GetEnumerations(const EnumElement& e)
        {

            auto i = e.attributes.find("enumerations");

            if (i == e.attributes.end()) {
                return nullptr;
            }

            return TypeQueryVisitor::as<ArrayElement>((*i)->value.second);
        }
    };

    // will be moved into different header (as part of drafter instead of refract)
    template <typename T>
    void CheckMixinParent(refract::IElement* element)
    {
        const T* resolved = TypeQueryVisitor::as<T>(element);

        if (!resolved) {
            throw snowcrash::Error(
                "mixin base type should be the same as parent base type. objects should contain object mixins. arrays "
                "should contain array mixins",
                snowcrash::MSONError);
        }
    }

    // will be moved into different header (as part of drafter instead of refract)
    template <typename T, typename Functor>
    void HandleRefWhenFetchingMembers(
        const refract::IElement* e, typename T::ValueType& members, const Functor& functor)
    {
        auto const found = e->attributes.find("resolved");

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

        for (RefractElements::const_iterator it = extended->value.begin(); it != extended->value.end(); ++it) {

            CheckMixinParent<T>(*it);
            // We can safely cast it because we are already checking the type in the above line.
            functor(static_cast<const T&>(**it), members);
        }
    }

    template <typename T>
    MemberElement* FindMemberByKey(const T& e, const std::string& name)
    {
        for (auto const& value: e.value) {

            ComparableVisitor cmp(name, ComparableVisitor::key);
            VisitBy(*value, cmp);

            if (cmp.get()) { // key was recognized - it is safe to cast to MemberElement
                return static_cast<MemberElement*>(value);
            }
        }

        return NULL;
    }

    template <typename T>
    T* FindCollectionMemberValue(const IElement::MemberElementCollection& collection, const std::string& key)
    {
        auto const i = collection.find(key);
        if (i == collection.end()) {
            return NULL;
        }

        return TypeQueryVisitor::as<T>((*i)->value.second);
    }

    std::string GetKeyAsString(const MemberElement& e);

    StringElement* GetDescription(const IElement& e);
}

#endif /* REFRACT_VISITORUTILS_H */
