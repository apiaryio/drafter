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

        template<typename T>
        T* FindNamedType(const Registry& registry, const std::string& name)
        {
            std::string en = name;
            T* e = new T;

            // FIXME: add check against recursive inheritance

            // walk recursive in registry an expand inheritance tree
            for (const IElement* parent = registry.find(en)
                ; parent && !isReserved(en)
                ; en = parent->element(), parent = registry.find(en) ) {

                IElement* clone = parent->clone((IElement::cAll ^ IElement::cElement) | IElement::cNoMetaId);
                clone->meta["ref"] = IElement::Create(en);
                e->push_back(clone);
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
        T* ExpandMembers(const T& e, const Registry& registry, const bool cloneId = true)
        {
            std::vector<IElement*> members;
#if _MIXIN_EXPANSION_
            bool hasRef = false;
#endif

            for (typename T::ValueType::const_iterator it = e.value.begin() ; it != e.value.end() ; ++it) {
#if _MIXIN_EXPANSION_
                if ((*it)->element() == "ref") {
                   hasRef = true; 
                }
#endif

                members.push_back(ExpandOrClone(*it, registry));
            }

            T* o = new T;
            o->attributes.clone(e.attributes);
            o->meta.clone(e.meta);
            if (!cloneId) {
                o->meta.erase("id");
            }

            if (!members.empty()) {
                o->set(members);
            }

#if _MIXIN_EXPANSION_
            if (hasRef) { // create addition object envelope over parent object
                T* extend = new T;
                extend->element("extend");
                extend->push_back(o);
                o = extend;
            }
#endif

            return o;
        }

        template<typename T>
        T* CreateExtend(const IElement& e, const Registry& registry) {
            typedef T ElementType;

            ElementType* extend = FindNamedType<T>(registry, e.element());
            extend->element("extend");

            CopyMetaId(*extend, e);

            ElementType* origin = ExpandMembers(static_cast<const ElementType&>(e), registry, false);

            extend->push_back(origin);

            return extend;
        }

        IElement* ExpandNamedType(const ObjectElement& e, const Registry& registry) 
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

#if _MIXIN_EXPANSION_
        IElement* ExpandReference(const ObjectElement& e, const Registry& registry)
        {
            TypeQueryVisitor tq;
            StringElement* href = tq.as<StringElement>(FindMemberByKey(e, "href"));

            if (href) {
                IElement* expanded = FindNamedType(registry, href->value);

                if (expanded->empty()) { // if referenced element not found return clone of reference
                    delete expanded;
                    expanded = e.clone(); 
                }

                return expanded;
            }

            // FIXME: report error
            // - Ref Element does not contain "href" key, 
            // - value of `href` is not `StringElement`
            
            // can no be implemeted until error reporting system
            // for now - ignore silently
            return NULL;
        }
#endif


    } // end of anonymous namespac

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
            result = ExpandNamedType(e, registry);
        }
#if _MIXIN_EXPANSION_
        else if (en == "ref") { // expand reference
            result = ExpandReference(e, registry);
        } 
#endif
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
