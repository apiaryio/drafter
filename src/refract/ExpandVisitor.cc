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

#include <functional>

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
        struct ExpandValueImpl {

            template <typename Functor>
            T operator()(const T& value, Functor&) {
                return value;
            }
        };

        template <>
        struct ExpandValueImpl<RefractElements> {

            template <typename Functor>
            RefractElements operator()(const RefractElements& value, Functor& expand) {
                RefractElements members;

                for (RefractElements::const_iterator it = value.begin() ; it != value.end() ; ++it) {
                    members.push_back(expand(*it));
                }

                return members;
            }
        };

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

            return e;
        }
    } // anonymous namespace

    struct ExpandVisitor::Context {

        const Registry& registry;
        ExpandVisitor* visitor;

        Context(const Registry& registry, ExpandVisitor* visitor) : registry(registry), visitor(visitor) {}

        IElement* ExpandOrClone(const IElement* e)
        {
            IElement* result = NULL;
            if (!e) {
                return result;
            }

            e->content(*visitor);
            result = visitor->get();

            if (!result) {
                result = e->clone();
            }

            return result;
        }

        template <typename V>
        V ExpandValue(const V& v) {
            std::binder1st<std::mem_fun1_t<IElement*, ExpandVisitor::Context, const IElement*> > expandOrClone = std::bind1st(std::mem_fun(&ExpandVisitor::Context::ExpandOrClone), this);
            return ExpandValueImpl<V>()(v, expandOrClone);
        }

        template<typename T>
        T* ExpandMembers(const T& e)
        {
            T* o = new T;
            o->attributes.clone(e.attributes);
            o->meta.clone(e.meta);

            if (!e.empty()) {
                o->set(ExpandValue(e.value));
            }

            return o;
        }

        std::deque<std::string> members;

        template <typename T>
        IElement* ExpandNamedType(const T& e)
        {
            
            // Look for Circular Reference thro members
            if (std::find(members.begin(), members.end(), e.element()) != members.end()) {
                // To avoid unfinised recursion just clone
                const IElement* root = FindRootAncestor(e.element(), registry);
                IElement* result = root->clone(IElement::cMeta | IElement::cAttributes | IElement::cNoMetaId);
                result->meta["ref"] = IElement::Create(e.element());
                return result;
            }

            members.push_back(e.element());

            ExtendElement* tree = GetInheritanceTree(e.element(), registry);
            ExtendElement* extend = ExpandMembers(*tree);
            delete tree;

            CopyMetaId(*extend, e);

            members.pop_back();

            T* origin = ExpandMembers(e);
            origin->meta.erase("id");

            extend->push_back(origin);

            return extend;
        }

        template <typename T>
        T* ExpandReference(const T& e)
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
                referenced = ExpandOrClone(referenced);
                MetaIdToRef(*referenced);
                referenced->renderType(IElement::rFull);
                ref->attributes["resolved"] = referenced;
            }

            return ref;
        }

    };

    template <typename T, typename V = typename T::ValueType>
    struct ExpandElement {
        IElement* result;

        ExpandElement(const T& e, ExpandVisitor::Context* context) : result(NULL) {

            if (!isReserved(e.element())) { // expand named type
                result = context->ExpandNamedType(e);
            }
        }

        operator IElement* () {
            return result;
        }
    };

    template <typename T>
    struct ExpandElement<T, RefractElements> {
        IElement* result;

        ExpandElement(const T& e, ExpandVisitor::Context* context) : result(NULL) {

            if (!Expandable(e)) {  // do we have some expandable members?
                return;
            }

            std::string en = e.element();

            if (!isReserved(en)) { // expand named type
                result = context->ExpandNamedType(e);
            }
            else if (en == "ref") { // expand reference
                result = context->ExpandReference(e);
            }
            else { // walk throught members and expand them
                result = context->ExpandMembers(e);
            }
        }

        operator IElement* () {
            return result;
        }
    };

    template <typename T>
    struct ExpandElement<T, MemberElement::ValueType> {
        IElement* result;

        ExpandElement(const T& e, ExpandVisitor::Context* context) : result(NULL) {

            if (!Expandable(e)) {
                return;
            }

            MemberElement* expanded = static_cast<MemberElement*>(e.clone(IElement::cAll ^ IElement::cValue));
            expanded->set(context->ExpandOrClone(e.value.first), context->ExpandOrClone(e.value.second));

            result = expanded;
        }

        operator IElement* () {
            return result;
        }
    };

    template <typename T>
    inline IElement* Expand(const T& e, ExpandVisitor::Context* context) {
        return ExpandElement<T>(e, context);
    }



    ExpandVisitor::ExpandVisitor(const Registry& registry) : result(NULL), context(new Context(registry, this)) {};

    ExpandVisitor::~ExpandVisitor() {
        delete context;
    }

    void ExpandVisitor::visit(const IElement& e) {
        e.content(*this);
    }
    
    // do nothing, NullElements are not expandable
    void ExpandVisitor::visit(const NullElement& e) {}

    void ExpandVisitor::visit(const MemberElement& e) {
        result = Expand(e, context);
    }

    void ExpandVisitor::visit(const StringElement& e) {
        result = Expand(e, context);
    }

    void ExpandVisitor::visit(const NumberElement& e) {
        result = Expand(e, context);
    }

    void ExpandVisitor::visit(const BooleanElement& e) {
        result = Expand(e, context);
    }

    void ExpandVisitor::visit(const ArrayElement& e) {
        result = Expand(e, context);
    }

    void ExpandVisitor::visit(const EnumElement& e) {
        result = Expand(e, context);
    }

    void ExpandVisitor::visit(const ObjectElement& e) {
        result = Expand(e, context);
    }

    void ExpandVisitor::visit(const ExtendElement& e) {
        result = Expand(e, context);
    }

    IElement* ExpandVisitor::get() const {
        return result;
    }

}; // namespace refract
