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
    bool HasTypeAttribute(const T& e, std::string typeAttribute)
    {
        auto ta = e.attributes().find("typeAttributes");

        if (ta == e.attributes().end()) {
            return false;
        }

        auto attrs = TypeQueryVisitor::as<const ArrayElement>(ta->second.get());

        if (!attrs) {
            return false;
        }

        for (const auto& value : attrs->get()) {
            auto attr = TypeQueryVisitor::as<const StringElement>(value.get());
            if (!attr) {
                continue;
            }
            if (attr->get() == typeAttribute) {
                return true;
            }
        }

        return false;
    }

    template <typename T>
    bool IsVariableProperty(const T& e)
    {
        auto const var = e.attributes().find("variable");

        if (var == e.attributes().end()) {
            return false;
        }

        auto b = TypeQueryVisitor::as<const BooleanElement>(var->second.get());
        return b ? static_cast<bool>(b->get()) : false;
    }

    template <typename T>
    const T* GetDefault(const T& e)
    {
        auto const dflt = e.attributes().find("default");

        if (dflt == e.attributes().end()) {
            return NULL;
        }

        return TypeQueryVisitor::as<const T>(dflt->second.get());
    }

    template <typename T>
    const T* GetSample(const T& e)
    {
        auto const i = e.attributes().find("samples");

        if (i == e.attributes().end()) {
            return nullptr;
        }

        auto a = TypeQueryVisitor::as<ArrayElement>(i->second.get());

        if (!a || a->get().empty()) {
            return nullptr;
        }

        return TypeQueryVisitor::as<T>(a->get().begin()->get());
    }

    template <typename T, typename R = typename T::ValueType>
    struct GetValue {

        const T* operator()(const T& element) const
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
                return &element;
            }

            if (const T* s = GetSample(element)) {
                return s;
            }

            if (const T* d = GetDefault(element)) {
                return d;
            }

            if (element.empty() && HasTypeAttribute(element, "nullable")) {
                return nullptr;
            }

            return &element;
        }
    };

    template <typename ValueType>
    struct GetValue<refract::EnumElement, ValueType> {
        const IElement* operator()(const EnumElement& element) const
        {
            if (const EnumElement* s = GetSample(element)) {
                return operator()(*s);
            }

            if (const EnumElement* d = GetDefault(element)) {
                return operator()(*d);
            }

            if (!element.empty()) {
                const auto& value = element.get();
                if (value.value()) {
                    return value.value();
                }
            }

            if (const ArrayElement* e = GetEnumerations(element)) {
                if (e && !e->empty()) {
                    for (const auto& item : e->get()) {
                        if (!item) {
                            continue;
                        }

                        // We need to hadle Enum individualy because of attr["enumerations"]
                        if (const EnumElement* val = TypeQueryVisitor::as<const EnumElement>(item.get())) {
                            auto ret = operator()(*val);
                            if (ret) {
                                return ret;
                            }
                        }

                        if (!item->empty()) {
                            return item.get();
                        }
                    }
                }
            }

            return element.empty() ? //
                nullptr :
                element.get().value();
        }

        const ArrayElement* GetEnumerations(const EnumElement& e) const
        {
            auto i = e.attributes().find("enumerations");

            if (i == e.attributes().end()) {
                return nullptr;
            }

            return TypeQueryVisitor::as<const ArrayElement>(i->second.get());
        }
    };

    // will be moved into different header (as part of drafter instead of refract)
    template <typename T>
    void CheckMixinParent(const refract::IElement* element)
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
    template <typename T, typename Collection, typename Functor>
    void HandleRefWhenFetchingMembers(const refract::IElement& e, Collection& members, const Functor& functor)
    {
        auto found = e.attributes().find("resolved");

        if (found == e.attributes().end()) {
            return;
        }

        const IElement* foundValue = found->second.get();

        const ExtendElement* extended = TypeQueryVisitor::as<const ExtendElement>(foundValue);

        if (!extended) {

            CheckMixinParent<T>(foundValue);
            // We can safely cast it because we are already checking the type in the above line.
            functor(static_cast<const T&>(*foundValue), members);
            return;
        }

        for (const auto& ext : extended->get()) {

            CheckMixinParent<T>(ext.get());
            // We can safely cast it because we are already checking the type in the above line.
            functor(static_cast<const T&>(*ext), members);
        }
    }

    MemberElement* FindMemberByKey(const ObjectElement& e, const std::string& name);

    template <typename T>
    T* FindCollectionMemberValue(const InfoElements& collection, const std::string& key)
    {
        auto const i = collection.find(key);
        if (i == collection.end()) {
            return nullptr;
        }

        return TypeQueryVisitor::as<T>(i->second.get());
    }

    std::string GetKeyAsString(const MemberElement& e);

    const StringElement* GetDescription(const IElement& e);

    bool IsLiteral(const IElement& e);
}

#endif /* REFRACT_VISITORUTILS_H */
