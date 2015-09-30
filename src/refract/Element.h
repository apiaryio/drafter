//
//  refract/Element.h
//  librefract
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_ELEMENT_H
#define REFRACT_ELEMENT_H

#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <iterator>

#include "Typelist.h"
#include "VisitableBy.h"

#include "Exception.h"
#include "Visitor.h"

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

    class ComparableVisitor;
    class SerializeVisitor;
    class SerializeCompactVisitor;
    class ExpandVisitor;
    class IsExpandableVisitor;
    class TypeQueryVisitor;
    class RenderJSONVisitor;

    template <typename T> struct ElementTypeSelector;

    // NOTE: alternative solution:
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

        /**
         * define __visitors__ which can visit element
         * via. `content()` method
         */
        typedef typelist::cons<
            ComparableVisitor,
            SerializeVisitor,
            SerializeCompactVisitor,
            ExpandVisitor,
            IsExpandableVisitor,
            TypeQueryVisitor,
            RenderJSONVisitor
        >::type Visitors;

        IElement() : hasContent(false), renderStrategy(rDefault)
        {
        }

        /**
         * Returns new element with content set as `value`
         * Type of returned element depends on type of `value`
         *
         * In current implementation iis able create just primitive element types: (Bool|Number|String)
         */
        template <typename T>
        static typename ElementTypeSelector<T>::ElementType* Create(const T& value)
        {
            typedef typename ElementTypeSelector<T>::ElementType ElementType;
            ElementType* element = new ElementType;
            element->set(value);

            return element;
        };

        /**
         * overrided for static function `Create()` with classic c-string
         */
        static StringElement* Create(const char* value);

        struct MemberElementCollection : public std::vector<MemberElement*>
        {
            virtual const_iterator find(const std::string& name) const;
            virtual iterator find(const std::string& name);
            MemberElement& operator[](const std::string& name);
            MemberElement& operator[](const int index);
            virtual void clone(const MemberElementCollection& other); /// < clone elements from `other` to `this`
            virtual void erase(const std::string& key);
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

        /**
         * Flags for clone() element - select parts of refract element to be clonned
         * \see Element<T>::clone()
         */
        typedef enum {
            cMeta       = 0x01,
            cAttributes = 0x02,
            cValue      = 0x04,
            cElement    = 0x08,
            cAll = cMeta | cAttributes | cValue | cElement,

            cNoMetaId   = 0x10,
        } cloneFlags;


        virtual IElement* clone(const int flag = cAll) const = 0;

        virtual bool empty() const
        {
            return !hasContent;
        }

        /**
         * select seriaization/rendering type of element
         * by default are elements serialized in expanded form,
         * with compact form of meta and attributes
         *
         * `renderType()` allows to change default behavior of selected element
         * (behavioration must be implemented in serialization visitors - it is partially done)
         */

        typedef enum {
            rDefault = 0,
            rFull,
            rCompact,
            rCompactContent
        } renderFlags;

        renderFlags renderStrategy;

        virtual renderFlags renderType() const
        {
            return renderStrategy;
        }

        virtual void renderType(const renderFlags render)
        {
            renderStrategy = render;
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

        virtual IElement* clone(const int flags = cAll) const {
            const Type* self = static_cast<const T*>(this);
            Type* element =  new Type;

            element->hasContent = self->hasContent;
            element->renderStrategy = self->renderStrategy;

            if (flags & cElement) {
                element->element_ = self->element_;
            }

            if (flags & cAttributes) {
                element->attributes.clone(self->attributes);
            }

            if (flags & cMeta) {
                element->meta.clone(self->meta);

                if (flags & cNoMetaId) {
                    element->meta.erase("id");
                }
            }

            if (flags & cValue) {
                TraitType::cloneValue(value, element->value);
            }

            return element;
        }

        Element() : value(TraitType::init())
        {
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

        static ValueType init() { return ValueType(); }
        static const std::string element() { return "null"; }
        static void release(ValueType&) {}
        static void cloneValue(const ValueType&, ValueType&) {}
    };
    struct NullElement : Element<NullElement, NullElementTrait> {};

    struct StringElementTrait
    {
        typedef std::string ValueType;

        static ValueType init() { return ValueType(); }
        static const std::string element() { return "string"; }
        static void release(ValueType&) {}
        static void cloneValue(const ValueType& self, ValueType& other) { other = self; }
    };
    struct StringElement : Element<StringElement, StringElementTrait> {};

    struct NumberElementTrait
    {
        typedef double ValueType;

        static ValueType init() { return 0; }
        static const std::string element() { return "number"; }
        static void release(ValueType&) {}
        static void cloneValue(const ValueType& self, ValueType& other) { other = self; }
    };
    struct NumberElement : Element<NumberElement, NumberElementTrait> {};

    struct BooleanElementTrait
    {
        typedef bool ValueType;

        static ValueType init() { return false; }
        static const std::string element() { return "boolean"; }
        static void release(ValueType&) {}
        static void cloneValue(const ValueType& self, ValueType& other) { other = self; }
    };
    struct BooleanElement : Element<BooleanElement, BooleanElementTrait> {};

    struct ArrayElementTrait
    {
        typedef std::vector<IElement*> ValueType;

        static ValueType init() { return ValueType(); }
        static const std::string element() { return "array"; }

        static void release(ValueType& array)
        {
            for (ValueType::iterator it = array.begin(); it != array.end(); ++it) {
                delete (*it);
            }

            array.clear();
        }

        static void cloneValue(const ValueType& self, ValueType& other) {
            std::transform(self.begin(), self.end(),
                           std::back_inserter(other),
                           std::bind2nd(std::mem_fun(&IElement::clone), IElement::cAll));
        }
    };

    struct ArrayElement : Element<ArrayElement, ArrayElementTrait>
    {
        void push_back(IElement* e)
        {
            // FIXME: warn if MemberElement
            // there is no way to present "key: value" in array
            hasContent = true;
            value.push_back(e);
        }
    };

    struct MemberElementTrait
    {
        typedef std::pair<IElement*, IElement*> ValueType;

        static ValueType init() { return ValueType(); }
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
            set(IElement::Create(key), element);
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

        // We don't use std::vector<MemberElement*> there, because
        // ObjectElement can contain:
        // - (array[Member Element])
        // - (object)
        // - (Extend Element)
        // - (Select Element)
        // - (Ref Element)
        //
        // FIXME: behavioration for content types different than
        // `(array[Member Element])` is not currently implemented

        static ValueType init() { return ValueType(); }

        static const std::string element() { return "object"; }

        static void release(ValueType& obj)
        {
            for (ValueType::iterator it = obj.begin(); it != obj.end(); ++it) {
                delete (*it);
            }

            obj.clear();
        }

        static void cloneValue(const ValueType& self, ValueType& other) {
            std::transform(self.begin(), self.end(),
                           std::back_inserter(other),
                           std::bind2nd(std::mem_fun(&IElement::clone), IElement::cAll));
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
};

#endif // #ifndef REFRACT_ELEMENT_H
