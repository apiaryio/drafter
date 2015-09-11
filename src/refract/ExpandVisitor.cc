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

        IElement* FindMemberByKey(const ObjectElement& e, const std::string& name)
        {
            for (ObjectElement::ValueType::const_iterator it = e.value.begin()
                ; it != e.value.end()
                ; ++it ) {

                ComparableVisitor cmp(name, ComparableVisitor::key);
                (*it)->content(cmp);

                if (cmp.get()) { // key was recognized - it is save to cast to MemberElement
                    MemberElement* m = static_cast<MemberElement*>(*it);
                    return m->value.second;
                }
            }

            return NULL;
        }

        typedef enum {
            Inherited,
            Referenced,
        } NamedTypeExpansion;

        template<typename T>
        T* ExpandNamedType(const Registry& registry, const std::string& name, NamedTypeExpansion expansion)
        {
            std::string en = name;
            T* e = NULL;

            if (expansion == Inherited) {
                e = new T;
            }

            // FIXME: add check against recursive inheritance

            // walk recursive in registry an expand inheritance tree
            for (const IElement* parent = registry.find(en)
                ; parent && !isReserved(en)
                ; en = parent->element(), parent = registry.find(en) ) {

                IElement* clone = parent->clone((IElement::cAll ^ IElement::cElement) | IElement::cNoMetaId);
                clone->meta["ref"] = IElement::Create(en);

                if (e) {
                    e->push_back(clone);
                }
                else {
                    e = static_cast<T*>(clone);
                }
            }

            // FIXME: posible solution while referenced type is not found in regisry
            // \see test/fixtures/mson-resource-unresolved-reference.apib
            //
            //if (o->value.empty()) {
            //   o->meta["ref"] = IElement::Create(name);
            //}

            return e;
        }

        template<typename T>
        T* ExpandMembers(const T& e, const Registry& registry)
        {
            std::vector<IElement*> members;

            for (typename T::ValueType::const_iterator it = e.value.begin() ; it != e.value.end() ; ++it) {
                members.push_back(ExpandOrClone(*it, registry));
            }

            T* o = new T;
            o->attributes.clone(e.attributes);
            o->meta.clone(e.meta);

            if (!members.empty()) {
                o->set(members);
            }

            return o;
        }

        template<typename T>
        T* CreateExtend(const IElement& e, const Registry& registry) {
            typedef T ElementType;

            ElementType* extend = ExpandNamedType<T>(registry, e.element(), Inherited);
            extend->element("extend");

            CopyMetaId(*extend, e);

            ElementType* origin = ExpandMembers(static_cast<const ElementType&>(e), registry);
            origin->meta.erase("id");
            extend->push_back(origin);

            return extend;
        }

        IElement* ExpandInheritance(const ObjectElement& e, const Registry& registry)
        {
            IElement* base = registry.find(e.element());
            TypeQueryVisitor t;

            if (base) {
                base->content(t);
            }

            if (t.get() == TypeQueryVisitor::Array) {
                return CreateExtend<ArrayElement>(e, registry);
            }
            else {
                return CreateExtend<ObjectElement>(e, registry);
            }

        }

        IElement* ExpandReference(const ObjectElement& e, const Registry& registry)
        {

            ObjectElement* ref = ExpandMembers(e, registry);
            ref->element("ref");
            ref->renderType(e.renderType());

            StringElement* href = TypeQueryVisitor::as<StringElement>(FindMemberByKey(e, "href"));

            if (!href || href->value.empty()) {
                return ref;
            }

            IElement* referenced = registry.find(href->value);

            if (!referenced) {
                return ref;
            }

            TypeQueryVisitor t;
            referenced->content(t);

            IElement* resolved = NULL;

            if (t.get() == TypeQueryVisitor::Array) {
                resolved = ExpandNamedType<ArrayElement>(registry, href->value, Referenced);
            }
            else {
                resolved = ExpandNamedType<ObjectElement>(registry, href->value, Referenced);
            }

            if (!resolved) {
                return ref;
            }

            resolved->renderType(IElement::rFull);
            ref->attributes["resolved"] = resolved;

            return ref;

        }

    } // end of anonymous namespace

    ExpandVisitor::ExpandVisitor(const Registry& registry) : result(NULL), registry(registry) {};

    void ExpandVisitor::visit(const IElement& e) {

        if (!Expandable(e)) {
            return;
        }

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

    void ExpandVisitor::visit(const ObjectElement& e) {

        if (!Expandable(e)) {  // do we have some expandable members?
            return;
        }

        std::string en = e.element();

        if (!isReserved(en)) { // expand named type
            result = ExpandInheritance(e, registry);
        }
        else if (en == "ref") { // expand reference
            result = ExpandReference(e, registry);
        }
        else { // walk throught members and expand them
            result = ExpandMembers(e, registry);
        }

    }

    // do nothing, primitive elements are not expandable
    void ExpandVisitor::visit(const NullElement& e) {}
    void ExpandVisitor::visit(const StringElement& e) {}
    void ExpandVisitor::visit(const NumberElement& e) {}
    void ExpandVisitor::visit(const BooleanElement& e) {}

    void ExpandVisitor::visit(const ArrayElement& e) {

        if (!Expandable(e)) {  // do we have some expandable members?
            return;
        }

        result = ExpandMembers(e, registry);
    }

    IElement* ExpandVisitor::get() const {
        return result;
    }

}; // namespace refract
