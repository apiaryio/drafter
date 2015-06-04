//
//  refract/Element.h
//  librefract
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef _REFRACT_ELEMENT_H_
#define _REFRACT_ELEMENT_H_

#include <string>
#include <vector>

#include <stdexcept>

#include "Typelist.h"
#include "VisitableBy.h"


#include "Exception.h"
#include "Visitor.h"
#include "SerializeVisitor.h"
#include "SerializeCompactVisitor.h"
#include "ComparableVisitor.h"

#include <iostream>

namespace refract
{

    // Forward declarations of Elements
    struct StringElement;
    struct NullElement;
    struct NumberElement;
    struct BooleanElement;
    struct ArrayElement;
    struct ObjectElement;
    struct MemberElement;

    struct ExpandVisitor;

    template <typename T> struct ElementTypeSelector;

    // NOTE: alternative soulution:
    // find in Element* for ValueType instead of specialized templates
    template <>
    struct ElementTypeSelector<std::string>
    {
        typedef StringElement ElementType;
    };

    template <>
    struct ElementTypeSelector<char*>
    {
        typedef StringElement ElementType;
    };

    template <>
    struct ElementTypeSelector<double>
    {
        typedef NumberElement ElementType;
    };

    template <>
    struct ElementTypeSelector<int>
    {
        typedef NumberElement ElementType;
    };

    template <>
    struct ElementTypeSelector<bool>
    {
        typedef BooleanElement ElementType;
    };

    struct IElement
    {
        bool hasContent; ///< was content of element already set? \see empty()
        bool useCompactContent;  ///< should be content serialized in compact form? \see compactContent()

        /**
         * define __visitors__ which can visit element
         * via. `content()` method
         */
        typedef typelist::cons<ComparableVisitor, SerializeVisitor, SerializeCompactVisitor, ExpandVisitor>::type Visitors;


        IElement() : hasContent(false), useCompactContent(false)
        {
        }

        template <typename T>
        static IElement* Create(const T& value)
        {
            typedef typename ElementTypeSelector<T>::ElementType ElementType;
            ElementType* element = new ElementType;
            element->set(value);
            return element;
        };

        static IElement* Create(const char* value);

        struct MemberElementCollection : std::vector<MemberElement*>
        {
            const_iterator find(const std::string& name) const;
            MemberElement& operator[](const std::string& name);
            MemberElement& operator[](const int index);
            virtual ~MemberElementCollection();
        };

        MemberElementCollection meta;
        MemberElementCollection attributes;

        /**
         * return "name" of element
         * usualy injected by "trait", but you can set own
         * via pair method `element(std::string)`
         */
        virtual std::string element() const = 0;
        virtual void element(const std::string&) = 0;

        // NOTE: Visiting is now handled by inheritance from `VisitableBy`
        // it uses internally C++ RTTI via `dynamic_cast<>`. 
        // And accepts all visitors declared in typelist IElement::Visitors
        //
        // Alternative solution to avoid RTTI:
        // Add overrided virtual function `content` for every one type of `Visitor`
        
        // NOTE: probably rename to Accept
        virtual void content(IVisitor& v) const = 0;

        virtual IElement* clone() const = 0;

        virtual bool empty() const
        {
            return !hasContent;
        }

        virtual bool compactContent() const
        {
            return useCompactContent;
        }

        virtual void renderCompactContent(bool compact)
        {
            useCompactContent = compact;
        }

        virtual ~IElement()
        {
        }

    };


    bool isReserved(const std::string& element);


    /**
     * CRTP implementation of RefractElement
     */
    template <typename T, typename Trait>
    struct Element : public IElement, public VisitableBy<IElement::Visitors>
    {

        typedef Element<T, Trait> Type;
        typedef Trait TraitType;
        typedef typename TraitType::ValueType ValueType;

        ValueType value;
        std::string element_;

        virtual std::string element() const
        {
            return element_.empty() ? TraitType::element() : element_;
        }

        virtual void element(const std::string& name)
        {
            element_ = name;
        }

        void set(const ValueType& val)
        {
            hasContent = true;
            value = val;
        }

        const ValueType& get() const
        {
            return value;
        }

        virtual void content(IVisitor& v) const
        {
            InvokeVisit(v, static_cast<const T&>(*this));
        }

        virtual IElement* clone() const {
            // FIXME: what about meta && attributes?
            const Type* self = static_cast<const T*>(this);
            Type* element =  new Type;

            element->hasContent = self->hasContent;
            element->useCompactContent = self->useCompactContent;

            TraitType::cloneValue(value, element->value);

            return element;
        }

        virtual ~Element()
        {
            TraitType::release(value);
        }
    };

    struct NullElementTrait
    {
        struct null_type {}; 
        typedef null_type ValueType;

        static const std::string element() { return "null"; }
        static void release(ValueType&) {}
        static void cloneValue(const ValueType&, ValueType&) {}
    };
    struct NullElement : Element<NullElement, NullElementTrait> {};

    struct StringElementTrait
    {
        typedef std::string ValueType;

        static const std::string element() { return "string"; }
        static void release(ValueType&) {}
        static void cloneValue(const ValueType& self, ValueType& other) { other = self; }
    };
    struct StringElement : Element<StringElement, StringElementTrait> {};

    struct NumberElementTrait
    {
        typedef double ValueType;

        static const std::string element() { return "number"; }
        static void release(ValueType&) {}
        static void cloneValue(const ValueType& self, ValueType& other) { other = self; }
    };
    struct NumberElement : Element<NumberElement, NumberElementTrait> {};

    struct BooleanElementTrait
    {
        typedef bool ValueType;

        static const std::string element() { return "boolean"; }
        static void release(ValueType&) {}
        static void cloneValue(const ValueType& self, ValueType& other) { other = self; }
    };
    struct BooleanElement : Element<BooleanElement, BooleanElementTrait> {};

    struct ArrayElementTrait
    {
        typedef std::vector<IElement*> ValueType;

        static const std::string element() { return "array"; }

        static void release(ValueType& array)
        {
            for (ValueType::iterator it = array.begin(); it != array.end(); ++it) {
                delete (*it);
            }
            array.clear();
        }

        static void cloneValue(const ValueType& self, ValueType& other) {
            for(ValueType::const_iterator i = self.begin() ; i != self.end() ; ++i) {
                IElement* e = NULL;
                if((*i)) {
                  e = (*i)->clone();
                }
                other.push_back(e);
            }
        }
    };

    struct ArrayElement : Element<ArrayElement, ArrayElementTrait>
    {
        void push_back(IElement* e)
        {
            // FIXME: warn if meta["name"] is NOT presented
            // \see comment on ObjectElement::push_bask()
            hasContent = true;
            value.push_back(e);
        }
    };

    struct MemberElementTrait
    {
        typedef std::pair<IElement*, IElement*> ValueType;

        static const std::string element() { return "member"; }

        static void release(ValueType& member)
        {
            if (member.first) {
                delete member.first;
                member.first = NULL;
            }
            if (member.second) {
                delete member.second;
                member.second = NULL;
            }
        }

        static void cloneValue(const ValueType& self, ValueType& other) {
            other.first = self.first
                ? static_cast<ValueType::first_type>(self.first->clone())
                : NULL;

            other.second = self.second
                ? self.second->clone()
                : NULL;
        }
    };

    struct MemberElement : Element<MemberElement, MemberElementTrait>
    {
        void set(const std::string& key, IElement* element)
        {
            set(IElement::Create(key) ,element);
        }

        void set(IElement* key, IElement* element)
        {
            if (value.first != NULL) {
                delete value.first;
                value.first = NULL;
            }
            value.first = key;

            if (value.second != NULL) {
                delete value.second;
                value.second = NULL;
            }
            value.second = element;

            hasContent = true;
        }

        MemberElement& operator=(IElement* element)
        {
            if (value.second != NULL) {
                delete value.second;
                value.second = NULL;
            }
            value.second = element;
            return *this;
        }
    };

    struct ObjectElementTrait
    {
        typedef std::vector<IElement*> ValueType;

        // We dont use std::vector<MemberElement*> there, because
        // ObjectElement can contain:
        // - (object)
        // - (array[Member Element])
        // - (Extend Element)
        // - (Select Element)
        // - (Ref Element)
        //

        static const std::string element() { return "object"; }

        static void release(ValueType& obj)
        {
            for (ValueType::iterator it = obj.begin(); it != obj.end(); ++it) {
                delete (*it);
            }
            obj.clear();
        }

        static void cloneValue(const ValueType& self, ValueType& other) {
            for(ValueType::const_iterator i = self.begin() ; i != self.end() ; ++i) {
                IElement* e = (*i)->clone();
                other.push_back(e);
            }
        }
    };

    struct ObjectElement : Element<ObjectElement, ObjectElementTrait>
    {
        void push_back(IElement* e)
        {
            // FIXME: 
            // probably add check for allowed type
            hasContent = true;
            value.push_back(e);
        }
    };

    class Registry {
        // FIXME: potentionally dangerous,
        // if element is deleted and not removed from registry
        // solution: std::shared_ptr<> || std::weak_ptr<>
        typedef std::map<std::string, IElement*> Map;
        Map registrated;

        std::string getElementId(IElement* element) 
        {
            IElement::MemberElementCollection::const_iterator it = element->meta.find("id");
            if (it == element->meta.end()) {
                throw LogicError("Element has no ID");
            }

            SerializeCompactVisitor v;
            (*it)->value.second->content(v);
            // FIXME: it is really ugly,
            // make something like is_a()
            return v.value().str;
        }

    public:

        IElement* find(const std::string& name)
        {
            Map::iterator i = registrated.find(name);
            if(i == registrated.end()) {
                return NULL;
            }
            return i->second;
        }

        bool add(IElement* element) 
        {
            IElement::MemberElementCollection::const_iterator it = element->meta.find("id");
            if (it == element->meta.end()) {
                throw LogicError("Element has no ID");
            }

            std::string id = getElementId(element);
            std::cout << "ID: >" << id << "<"<< std::endl;

            if(isReserved(id)) {
                throw LogicError("You can not registrate basic element");
            }

            if (find(id)) {
                // there is already already element with given name
                return false;
            }

            registrated[id] = element;
            return true;
        }

        bool remove(const std::string& name) {
            Map::iterator i = registrated.find(name);
            if(i == registrated.end()) {
                return false;
            }
            registrated.erase(i);
            return true;
        }

        void clearAll(bool releaseElements = false) {
            if(releaseElements) {
                for (Map::iterator i = registrated.begin() ; i != registrated.end() ; ++i) {
                    delete i->second;
                }
            }
            registrated.clear();
        }

    };

    // FIXME: remove global variable!!
    extern Registry DSRegistry;

    template<typename T, typename V = typename T::ValueType> struct IsExpandable;

    struct ExpandVisitor : IVisitor {

        IElement* result;

        ExpandVisitor() : result(NULL) {};

        template<typename T>
        void visit(const T& e) {
            std::string en = e.element();
            if(isReserved(en)) {
                return;
            }

            refract::ObjectElement* o = new refract::ObjectElement;
            o->element("extend");

            IElement::MemberElementCollection::const_iterator name = e.meta.find("id");
            if (name != e.meta.end() && (*name)->value.second) {
                o->meta["id"] = (*name)->value.second->clone();
            }

           // const IElement* parent = refract::DSRegistry.find(en);
            for (const IElement* parent = refract::DSRegistry.find(en)
                ; parent && !isReserved(en)
                ; en = parent->element(), parent = refract::DSRegistry.find(en) ) {

                IElement* clone = parent->clone();
                clone->meta["ref"] = IElement::Create(en);
                o->push_back(clone);

                //en = parent->element();
                //parent = isReserved(en)
                //    ? NULL
                //    : parent = refract::DSRegistry.find(en);

            } 

            o->push_back(e.clone());
            result = o;
        }

        IElement* get() const {
            return result;
        }
    };

#if 0
    template <typename T, typename V>
    struct IsExpandable {
        bool operator()(const T* e) const {
            std::cout << __PRETTY_FUNCTION__ << e->element() << std::endl;
            //return false;
            if(!isReserved(e->element())) {
                return true;
            }
            return false;
        }
    };

    template <typename T>
    struct IsExpandable<T, std::vector<IElement*> > {
        bool operator()(const T* e) const {
            std::cout << __PRETTY_FUNCTION__ << e->element() << std::endl;

            if(!isReserved(e->element())) {
                return true;
            }

            /*
            for (std::vector<IElement*>::const_iterator i = e->value.begin() ; i != e->value.end() ; ++i ) {
                ExpandVisitor v;
                (*i)->content(v);
                if (v.isExpandable()) {
                    return true;
                }
            }
            */

            return false;
        }
    };
#endif

}; // namespace refract

#endif // #ifndef _REFRACT_ELEMENT_H_
