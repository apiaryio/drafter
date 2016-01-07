//
//  refract/ExpandVisitor.cc
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "Element.h"
#include "Visitors.h"
#include "Registry.h"
#include <stack>

namespace refract
{

    namespace
    {

        bool Expandable(const IElement& e)
        {
            IsExpandableVisitor v;
            e.content(v);
            return v.get();
        }

        IElement* ExpandOrClone(const IElement* e, const Registry& registry)
        {
            IElement* result = NULL;
            if (!e) {
                return result;
            }

            ExpandVisitor expander(registry);
            e->content(expander);
            result = expander.get();

            if (!result) {
                result = e->clone();
            }

            return result;
        }

        void CopyMetaId(IElement& dst, const IElement& src)
        {
            IElement::MemberElementCollection::const_iterator name = src.meta.find("id");
            if (name != src.meta.end() && (*name)->value.second && !(*name)->value.second->empty()) {
                dst.meta["id"] = (*name)->value.second->clone();
            }
        }

        void MetaIdToRef(IElement& e)
        {
            IElement::MemberElementCollection::const_iterator name = e.meta.find("id");
            if (name != e.meta.end() && (*name)->value.second && !(*name)->value.second->empty()) {
                IElement* clone = (*name)->value.second->clone();
                e.meta["ref"] = clone;
                e.meta.erase("id");
            }
        }

        typedef std::vector<IElement*> RefractElements;

        template <typename T>
        struct ExpandValue {

            T operator()(const T& value, const Registry& registry) {
                return value;
            }
        };

        template <>
        struct ExpandValue<RefractElements> {

            RefractElements operator()(const RefractElements& value, const Registry& registry) {
                RefractElements members;

                for (RefractElements::const_iterator it = value.begin() ; it != value.end() ; ++it) {
                    members.push_back(ExpandOrClone(*it, registry));
                }

                return members;
            }
        };

        template<typename T>
        T* ExpandMembers(const T& e, const Registry& registry)
        {
            T* o = new T;
            o->attributes.clone(e.attributes);
            o->meta.clone(e.meta);

            if (!e.empty()) {
                o->set(ExpandValue<typename T::ValueType>()(e.value, registry));
            }

            return o;
        }

        ExtendElement* GetInheritanceTree(const std::string& name, const Registry& registry)
        {
            std::stack<IElement*> inheritance;
            std::string en = name;

            // FIXME: add check against recursive inheritance
            // walk recursive in registry and expand inheritance tree
            for (const IElement* parent = registry.find(en)
                ; parent && !isReserved(en)
                ; en = parent->element(), parent = registry.find(en)) {

                inheritance.push(parent->clone((IElement::cAll ^ IElement::cElement) | IElement::cNoMetaId));
                inheritance.top()->meta["ref"] = IElement::Create(en);
            }

            ExtendElement* e = new ExtendElement;

            while (!inheritance.empty()) {
                e->push_back(inheritance.top());
                inheritance.pop();
            }

            // FIXME: posible solution while referenced type is not found in regisry
            // \see test/fixtures/mson-resource-unresolved-reference.apib
            //
            //if (e->value.empty()) {
            //   e->meta["ref"] = IElement::Create(name);
            //}

            ExtendElement* result = ExpandMembers(*e, registry);
            delete e;

            return result;
        }

        template <typename T>
        IElement* ExpandNamedType(const T& e, const Registry& registry)
        {
            ExtendElement* extend = GetInheritanceTree(e.element(), registry);

            CopyMetaId(*extend, e);

            T* origin = ExpandMembers(e, registry);
            origin->meta.erase("id");

            extend->push_back(origin);

            return extend;
        }

        template <typename T>
        T* ExpandReference(const T& e, const Registry& registry)
        {
            T* ref = static_cast<T*>(e.clone());

            MemberElement *m = FindMemberByKey(e, "href");
            if (!m) {
                return NULL;
            }

            StringElement* href = TypeQueryVisitor::as<StringElement>(m->value.second);
            if (!href || href->value.empty()) {
                return ref;
            }

            if (IElement* referenced = registry.find(href->value)) {
                referenced = ExpandOrClone(referenced, registry);
                MetaIdToRef(*referenced);
                referenced->renderType(IElement::rFull);
                ref->attributes["resolved"] = referenced;
            }

            return ref;
        }

        template <typename T, typename V = typename T::ValueType>
        struct ExpandElement {
            IElement* result;

            ExpandElement(const T& e, const Registry& registry) : result(NULL) {
                if (!isReserved(e.element())) { // expand named type
                    result = ExpandNamedType(e, registry);
                }
            }

            operator IElement* () {
                return result;
            }
        };

        template <typename T>
        struct ExpandElement<T, RefractElements> {
            IElement* result;

            ExpandElement(const T& e, const Registry& registry) : result(NULL) {

                if (!Expandable(e)) {  // do we have some expandable members?
                    return;
                }

                std::string en = e.element();

                if (!isReserved(en)) { // expand named type
                    result = ExpandNamedType(e, registry);
                }
                else if (en == "ref") { // expand reference
                    result = ExpandReference(e, registry);
                }
                else { // walk throught members and expand them
                    result = ExpandMembers(e, registry);
                }
            }

            operator IElement* () {
                return result;
            }
        };

        template <typename T>
        inline IElement* Expand(const T& e, const Registry& registry) {
            return ExpandElement<T>(e, registry);
        }

    } // end of anonymous namespace

    ExpandVisitor::ExpandVisitor(const Registry& registry) : result(NULL), registry(registry) {};

    void ExpandVisitor::visit(const IElement& e) {
        e.content(*this);
    }

    void ExpandVisitor::visit(const MemberElement& e) {
        if (!Expandable(e)) {
            return;
        }

        MemberElement* expanded = static_cast<MemberElement*>(e.clone(IElement::cAll ^ IElement::cValue));
        expanded->set(ExpandOrClone(e.value.first, registry), ExpandOrClone(e.value.second, registry));

        result = expanded;
    }


    // do nothing, NullElements are not expandable
    void ExpandVisitor::visit(const NullElement& e) {}

    void ExpandVisitor::visit(const StringElement& e) {
        result = Expand(e, registry);
    }

    void ExpandVisitor::visit(const NumberElement& e) {
        result = Expand(e, registry);
    }

    void ExpandVisitor::visit(const BooleanElement& e) {
        result = Expand(e, registry);
    }

    void ExpandVisitor::visit(const ArrayElement& e) {
        result = Expand(e, registry);
    }

    void ExpandVisitor::visit(const EnumElement& e) {
        result = Expand(e, registry);
    }

    void ExpandVisitor::visit(const ObjectElement& e) {
        result = Expand(e, registry);
    }

    void ExpandVisitor::visit(const ExtendElement& e) {
        result = Expand(e, registry);
    }

    IElement* ExpandVisitor::get() const {
        return result;
    }

}; // namespace refract
