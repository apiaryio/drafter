//
//  refract/IsExpandableVisitor.cc
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#include "Element.h"
#include "IsExpandableVisitor.h"

namespace refract
{

    namespace
    {
        bool checkElement(const IElement* e)
        {
            std::string type;

            if (e) {
                type = e->element();
            }

            return !isReserved(type.c_str());
        }

        template <typename T, typename V = typename T::ValueType, bool IsIterable = dsd::is_iterable<V>::value>
        struct IsExpandable {
            bool operator()(const T* e) const
            {

                if (checkElement(e)) {
                    return true;
                }

                return false;
            }
        };

        template <typename T>
        struct IsExpandable<T, RefElement::ValueType, false> {
            bool operator()(const T* e) const
            {

                return true;
            }
        };

        template <typename T>
        struct IsExpandable<T, SelectElement::ValueType, true> {
            bool operator()(const T* e) const
            {

                if (checkElement(e)) {
                    return true;
                }

                if (!e->empty())
                    for (const auto& option : e->get()) {
                        IsExpandableVisitor v;
                        VisitBy(*option, v);

                        if (v.get()) {
                            return true;
                        }
                    }

                return false;
            }
        };

        template <typename T>
        struct IsExpandable<T, MemberElement::ValueType, false> {
            bool operator()(const T* e) const
            {

                if (checkElement(e)) {
                    return true;
                }

                if (e->empty())
                    return false;

                const auto& content = e->get();

                if (const IElement* key = content.key()) {
                    IsExpandableVisitor v;
                    VisitBy(*key, v);
                    if (v.get()) {
                        return true;
                    }
                }

                if (const IElement* value = content.value()) {
                    IsExpandableVisitor v;
                    VisitBy(*value, v);
                    if (v.get()) {
                        return true;
                    }
                }

                return false;
            }
        };

        template <typename T, typename V>
        struct IsExpandable<T, V, true> {
            bool operator()(const T* e) const
            {

                if (checkElement(e)) {
                    return true;
                }

                if (!e->empty())
                    for (const auto& entry : e->get()) {
                        IsExpandableVisitor v;
                        VisitBy(*entry, v);

                        if (v.get()) {
                            return true;
                        }
                    }

                return false;
            }
        };

        template <>
        struct IsExpandable<EnumElement, EnumElement::ValueType, false> {
            bool operator()(const EnumElement* e) const
            {
                if (checkElement(e))
                    return true;

                if (!e->empty()) {
                    IsExpandableVisitor v;
                    VisitBy(*e->get().value(), v);
                    if (v.get())
                        return true;
                }

                const auto it = e->attributes().find("enumerations");
                if (it != e->attributes().end()) {
                    IsExpandableVisitor v;
                    VisitBy(*it->second, v);
                    if (v.get())
                        return true;
                }

                return false;
            }
        };
    } // anonymous namespace

    IsExpandableVisitor::IsExpandableVisitor() : result(false) {}

    template <typename T>
    void IsExpandableVisitor::operator()(const T& e)
    {
        result = IsExpandable<T>()(&e);
    }

    template <>
    void IsExpandableVisitor::operator()(const IElement& e)
    {
        VisitBy(e, *this);
    }

    // Explicit instantioning of templates to avoid Linker Error
    template void IsExpandableVisitor::operator()<NullElement>(const NullElement&);
    template void IsExpandableVisitor::operator()<StringElement>(const StringElement&);
    template void IsExpandableVisitor::operator()<NumberElement>(const NumberElement&);
    template void IsExpandableVisitor::operator()<BooleanElement>(const BooleanElement&);
    template void IsExpandableVisitor::operator()<HolderElement>(const HolderElement&);
    template void IsExpandableVisitor::operator()<ArrayElement>(const ArrayElement&);
    template void IsExpandableVisitor::operator()<EnumElement>(const EnumElement&);
    template void IsExpandableVisitor::operator()<MemberElement>(const MemberElement&);
    template void IsExpandableVisitor::operator()<ObjectElement>(const ObjectElement&);
    template void IsExpandableVisitor::operator()<RefElement>(const RefElement&);
    template void IsExpandableVisitor::operator()<ExtendElement>(const ExtendElement&);
    template void IsExpandableVisitor::operator()<OptionElement>(const OptionElement&);
    template void IsExpandableVisitor::operator()<SelectElement>(const SelectElement&);

    bool IsExpandableVisitor::get() const
    {
        return result;
    }

}; // namespace refract
