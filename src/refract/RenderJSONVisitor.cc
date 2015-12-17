//
//  refract/RenderJSONVisitor.cc
//  librefract
//
//  Created by Pavan Kumar Sunkara on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "VisitorUtils.h"
#include "sosJSON.h"
#include <sstream>
#include "SerializeCompactVisitor.h"

namespace refract
{

    namespace {

        typedef std::vector<IElement*> RefractElements;

        class ElementMerger {

            IElement* result;
            TypeQueryVisitor::ElementType base;


            /**
             * Merge strategy for Primitive types - just replace by latest value
             */
            template <typename T, typename V = typename T::ValueType> 
            struct ValueMerge {
                V& value;
                ValueMerge(T& element) : value(element.value) {}

                void operator()(const T& merge) { 
                    value = merge.value;
                }
            };

            /**
             * Merge stategy for objects/array
             * - if member 
             *   - without existing key -> append
             *   - with existing key - replace old value
             * - if not member
             *   - if empty value -> ignore (type holder for array)
             *   - else append
             */
            template <typename T>
            struct ValueMerge<T, RefractElements> {
                typename T::ValueType& value;

                ValueMerge(T& element) : value(element.value) {}

                void operator()(const T& merge) {
                    typedef std::map<std::string, MemberElement*> MapKeyToMember;
                    MapKeyToMember keysBase;

                    for (RefractElements::iterator it = value.begin() ; it != value.end() ; ++it) {
                        if (MemberElement* member = TypeQueryVisitor::as<MemberElement>(*it)) {

                            if (StringElement* key = TypeQueryVisitor::as<StringElement>(member->value.first)) {
                                keysBase[key->value] = member;
                            }
                        }
                    }

                    for (RefractElements::const_iterator it = merge.value.begin() ; it != merge.value.end() ; ++it) {
                        if (MemberElement* member = TypeQueryVisitor::as<MemberElement>(*it)) {
                            if (StringElement* key = TypeQueryVisitor::as<StringElement>(member->value.first)) {
                                MapKeyToMember::iterator iKey = keysBase.find(key->value);

                                if (iKey != keysBase.end()) { // key is already presented, replace value
                                    delete iKey->second->value.second;
                                    iKey->second->value.second = member->value.second->clone();
                                } 
                                else { // unknown key, append value
                                    MemberElement* clone = static_cast<MemberElement*>(member->clone());
                                    value.push_back(clone);
                                    keysBase[key->value] = clone;
                                }
                            }
                        }
                        else if(!(*it)->empty()) { // merge member is not MemberElement, append value
                            value.push_back((*it)->clone());
                        }
                    }
                }
            };

            template <typename T>
            struct InfoMerge {
                IElement::MemberElementCollection& info;
                InfoMerge(IElement::MemberElementCollection& info) : info(info) {}

                void operator()(const IElement::MemberElementCollection& append) {
                    for (IElement::MemberElementCollection::const_iterator it = append.begin() ; it != append.end() ; ++it) {
                        if (StringElement* key = TypeQueryVisitor::as<StringElement>((*it)->value.first)) {
                            IElement::MemberElementCollection::iterator item = info.find(key->value);

                            if (item != info.end()) {
                                delete (*item)->value.second;
                                (*item)->value.second = (*it)->value.second->clone();
                            }
                        }
                    }
                }
            };

            /**
             * precondition - target && append element MUST BE of same type
             * we use static_cast<> without checking type t is responsibility if caller
             */
            template <typename T>
            static void doMerge(IElement* target, const IElement* append) {
                typedef T ElementType;

                InfoMerge<T>(target->meta)(append->meta);
                InfoMerge<T>(target->attributes)(append->attributes);

                ValueMerge<T>(static_cast<ElementType&>(*target))
                             (static_cast<const ElementType&>(*append));
            }

        public:

            ElementMerger() : result(NULL), base(TypeQueryVisitor::Unknown) {}

            void operator()(const IElement* e) {
                if (!e) {
                    return;
                }

                if (!result) {
                    result = e->clone();

                    TypeQueryVisitor type;
                    type.visit(*result);
                    base = type.get();
                    return;
                }

                TypeQueryVisitor type;
                e->content(type);

                if(type.get() != base) {
                    throw refract::LogicError("Can not merge different types of elements");
                }

                switch(base) {
                    case TypeQueryVisitor::Null:
                        return;

                    case TypeQueryVisitor::String:
                        doMerge<StringElement>(result, e);
                        return;

                    case TypeQueryVisitor::Number:
                        doMerge<NumberElement>(result, e);
                        return;

                    case TypeQueryVisitor::Boolean:
                        doMerge<BooleanElement>(result, e);
                        return;

                    case TypeQueryVisitor::Array:
                        doMerge<ArrayElement>(result, e);
                        return;

                    case TypeQueryVisitor::Object:
                        doMerge<ObjectElement>(result, e);
                        return;

                    case TypeQueryVisitor::Member:
                        doMerge<MemberElement>(result, e);
                        return;
                }
            }

            operator IElement* () const { 
                return result; 
            }

        };

        IElement* getEnumValue(const ObjectElement& extend)
        {
            if (extend.empty()) {
                return NULL;
            }

            for (ObjectElement::ValueType::const_reverse_iterator it = extend.value.rbegin();
                 it != extend.value.rend();
                 ++it) {

                const ArrayElement* element = TypeQueryVisitor::as<ArrayElement>(*it);

                if (!element) {
                    continue;
                }

                const ArrayElement::ValueType* items = GetValue<ArrayElement>(*element);

                if (!items->empty()) {
                    return *items->begin();
                }
            }

            return NULL;
        }

        IElement* ResolveExtendElement(const ObjectElement& e) 
        {

            if (e.element() != "extend") {
                return NULL;
            }

            if (e.value.empty()) {
                return NULL;
            }

            //if (!*e.value.rbegin()) {
            //    return NULL;
            //}

            //return (*e.value.begin())->clone();

            //Mgr merger;
            //for (ObjectElement::ValueType::const_reverse_iterator it = e.value.rbegin() ; it != e.value.rend() ; ++it) {
            //    merger(*it);
            //}
            //return merger;

            return std::for_each(e.value.begin(), e.value.end(), ElementMerger());

        }

        template <typename T>
        void FetchMembers(const T& element, typename T::ValueType& members)
        {
            const typename T::ValueType* val = GetValue<T>(element);

            if (!val) {
                return;
            }

            for (typename T::ValueType::const_iterator it = val->begin();
                 it != val->end();
                 ++it) {

                if (!(*it) || (*it)->empty()) {
                    continue;
                }

                if ((*it)->element() == "ref") {
                    IElement::MemberElementCollection::const_iterator found = (*it)->attributes.find("resolved");

                    if (found == (*it)->attributes.end()) {
                        continue;
                    }

                    const T* resolved = TypeQueryVisitor::as<T>((*found)->value.second);

                    if (!resolved) {
                        throw refract::LogicError("Mixin must refer to same type as parent");
                    }

                    FetchMembers(*resolved, members);
                    continue;
                }

                RenderJSONVisitor renderer;
                renderer.visit(*(*it));
                IElement* e = renderer.getOwnership();

                if (!e) {
                    continue;
                }

                e->renderType(IElement::rCompact);
                members.push_back(e);

            }
        }

    }

    RenderJSONVisitor::RenderJSONVisitor() : result(NULL), enumValue(NULL) {}

    RenderJSONVisitor::~RenderJSONVisitor() { 
        if (result) {
            delete result;
        }

        if (enumValue) {
            delete enumValue;
        }
    }

    void RenderJSONVisitor::visit(const IElement& e) {
        e.content(*this);
    }

    void RenderJSONVisitor::visit(const MemberElement& e) {

        RenderJSONVisitor renderer;

        if (e.value.second) {
            if (IsTypeAttribute(e, "nullable") && e.value.second->empty()) {
                renderer.result = new NullElement;
            } 
            else if (IsTypeAttribute(e, "optional") && e.value.second->empty()) {
                return;
            } 
            else {
                renderer.visit(*e.value.second);
                if (!renderer.result) {
                    return;
                }
            }
        }

        if (StringElement* str = TypeQueryVisitor::as<StringElement>(e.value.first)) {
            MemberElement *m = new MemberElement;
            IElement* v= renderer.result ? renderer.getOwnership() : new StringElement;
            m->set(str->value, v);
            result = m;
            result->renderType(IElement::rCompact);
        }
        else {
            throw std::logic_error("A property's key in the object is not of type string");
        }
    }


    void RenderJSONVisitor::visit(const ObjectElement& e) {

        // FIXME: introduce ExtendElement type
        if (e.element() == "extend") {
            RenderJSONVisitor renderer;
            IElement* resolved = ResolveExtendElement(e);

            if (!resolved) {
                return;
            }
            resolved->renderType(IElement::rCompact);

            if (resolved->element() == "enum") {
                renderer.enumValue = getEnumValue(e);
                if (renderer.enumValue) {
                    renderer.enumValue = renderer.enumValue->clone();
                }
            }

            renderer.visit(*resolved);
            result = renderer.getOwnership();

            delete resolved;
            return;
        }

        ObjectElement::ValueType members;
        FetchMembers(e, members);
        ObjectElement* o = new ObjectElement;
        o->set(members);
        o->renderType(IElement::rCompact);
        result = o;

    }

    void RenderJSONVisitor::visit(const ArrayElement& e) {

        // FIXME: introduce EnumElement
        if (e.element() == "enum") {

            if (!enumValue) { // there is no enumValue injected from ExtendElement,try to pick value directly

                const ArrayElement::ValueType* val = GetValue<ArrayElement>(e);
                if (val && !val->empty()) {
                    enumValue = val->front()->clone();
                } 
                else {
                    enumValue = new StringElement;
                }
            }

            RenderJSONVisitor renderer;
            enumValue->content(renderer);
            result = renderer.getOwnership();

            delete enumValue;
            enumValue = NULL;

            return;
        }

        ArrayElement::ValueType members;
        FetchMembers(e, members);
        ArrayElement* a = new ArrayElement;
        a->set(members);
        a->renderType(IElement::rCompact);
        result = a;
    }

    void RenderJSONVisitor::visit(const NullElement& e) {
        result = new NullElement;
    }

    template <typename T>
    IElement* getResult(const T& e)
    {
        const typename T::ValueType* v = GetValue<T>(e);

        if (!v) {
            return NULL;
        }

        T* result = IElement::Create(*v);
        result->renderType(IElement::rCompact);

        return result;
    }

    void RenderJSONVisitor::visit(const StringElement& e) {
        result = getResult(e);
    }

    void RenderJSONVisitor::visit(const NumberElement& e) {
        result = getResult(e);
    }

    void RenderJSONVisitor::visit(const BooleanElement& e) {
        result = getResult(e);
    }

   IElement* RenderJSONVisitor::getOwnership() {
       IElement* ret = result;
       result = NULL;
       return ret;
   }

    std::string RenderJSONVisitor::getString() const {
        std::string out;

        if (result) {
            sos::SerializeJSON serializer;
            std::stringstream ss;

            SerializeCompactVisitor s;
            result->content(s);
            serializer.process(s.value(), ss);

            return ss.str();
        }

        return out;
    }
}
