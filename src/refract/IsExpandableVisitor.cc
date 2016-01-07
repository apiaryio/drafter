//
//  refract/IsExpandableVisitor.cc
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#include "Element.h"
#include "Visitors.h"

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
        struct IsExpandable<T, MemberElement::ValueType> : public CheckElement {
            bool operator()(const T* e) const {

                if (checkElement(e)) {
                    return true;
                }

                if (e->value.first) {
                    IsExpandableVisitor v;
                    e->value.first->content(v);
                    if (v.get()) {
                        return true;
                    }
                }

                if (e->value.second) {
                    IsExpandableVisitor v;
                    e->value.second->content(v);
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
                    (*i)->content(v);

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
    void IsExpandableVisitor::visit(const T& e) {
        result = IsExpandable<T>()(&e);
    }

    // Explicit instantioning of templates to avoid Linker Error
    template void IsExpandableVisitor::visit<NullElement>(const NullElement&);
    template void IsExpandableVisitor::visit<StringElement>(const StringElement&);
    template void IsExpandableVisitor::visit<NumberElement>(const NumberElement&);
    template void IsExpandableVisitor::visit<BooleanElement>(const BooleanElement&);
    template void IsExpandableVisitor::visit<ArrayElement>(const ArrayElement&);
    template void IsExpandableVisitor::visit<EnumElement>(const EnumElement&);
    template void IsExpandableVisitor::visit<MemberElement>(const MemberElement&);
    template void IsExpandableVisitor::visit<ObjectElement>(const ObjectElement&);
    template void IsExpandableVisitor::visit<ExtendElement>(const ExtendElement&);

    bool IsExpandableVisitor::get() const {
        return result;
    }

}; // namespace refract
