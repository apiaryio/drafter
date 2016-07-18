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
        struct CheckElement {
            bool checkElement(const IElement* e) const {
                std::string type;

                if (e) {
                    type = e->element();
                }

                return !isReserved(type) || type == "ref" ;
            }
        };

        template <typename T, typename V = typename T::ValueType>
        struct IsExpandable : public CheckElement {
            bool operator()(const T* e) const {

                if (checkElement(e)) {
                    return true;
                }

                return false;
            }
        };

        template <typename T>
        struct IsExpandable<T, SelectElement::ValueType> : public CheckElement {
            bool operator()(const T* e) const {

                if (checkElement(e)) {
                    return true;
                }

                for (std::vector<OptionElement*>::const_iterator i = e->value.begin() ; i != e->value.end() ; ++i ) {
                    IsExpandableVisitor v;
                    VisitBy(*(*i), v);

                    if (v.get()) {
                        return true;
                    }
                }

                return false;
            }
        };

        template <typename T>
        struct IsExpandable<T, MemberElement::ValueType> : public CheckElement {
            bool operator()(const T* e) const {

                if (checkElement(e)) {
                    return true;
                }

                if (e->value.first) {
                    IsExpandableVisitor v;
                    VisitBy(*e->value.first, v);
                    if (v.get()) {
                        return true;
                    }
                }

                if (e->value.second) {
                    IsExpandableVisitor v;
                    VisitBy(*e->value.second, v);
                    if (v.get()) {
                        return true;
                    }
                }

                return false;
            }
        };

        template <typename T>
        struct IsExpandable<T, RefractElements> : public CheckElement {
            bool operator()(const T* e) const {

                if (checkElement(e)) {
                    return true;
                }

                for (std::vector<IElement*>::const_iterator i = e->value.begin() ; i != e->value.end() ; ++i ) {
                    IsExpandableVisitor v;
                    VisitBy(*(*i), v);

                    if (v.get()) {
                        return true;
                    }
                }

                return false;
            }
        };
    } // anonymous namespace

    IsExpandableVisitor::IsExpandableVisitor() : result(false) {}

    template<typename T>
    void IsExpandableVisitor::operator()(const T& e) {
        result = IsExpandable<T>()(&e);
    }

    template<>
    void IsExpandableVisitor::operator()(const IElement& e) {
        VisitBy(e, *this);
    }

    // Explicit instantioning of templates to avoid Linker Error
    template void IsExpandableVisitor::operator()<NullElement>(const NullElement&);
    template void IsExpandableVisitor::operator()<StringElement>(const StringElement&);
    template void IsExpandableVisitor::operator()<NumberElement>(const NumberElement&);
    template void IsExpandableVisitor::operator()<BooleanElement>(const BooleanElement&);
    template void IsExpandableVisitor::operator()<ArrayElement>(const ArrayElement&);
    template void IsExpandableVisitor::operator()<EnumElement>(const EnumElement&);
    template void IsExpandableVisitor::operator()<MemberElement>(const MemberElement&);
    template void IsExpandableVisitor::operator()<ObjectElement>(const ObjectElement&);
    template void IsExpandableVisitor::operator()<ExtendElement>(const ExtendElement&);
    template void IsExpandableVisitor::operator()<OptionElement>(const OptionElement&);
    template void IsExpandableVisitor::operator()<SelectElement>(const SelectElement&);

    bool IsExpandableVisitor::get() const {
        return result;
    }

}; // namespace refract
