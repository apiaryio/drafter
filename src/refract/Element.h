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
        typedef typelist::cons<ComparableVisitor, SerializeVisitor, SerializeCompactVisitor>::type Visitors;


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

    /**
     * CRTP implementation of RefractElement
     *
     */
    template <typename T, typename Trait>
    struct Element : public IElement, public VisitableBy<IElement::Visitors>
    {

        typedef Element<T, Trait> Type;
        typedef Trait TraitType;
        typedef typename TraitType::ValueType ValueType;

        TraitType trait;
        ValueType value;
        std::string element_;


        virtual std::string element() const
        {
            return element_.empty() ? trait.element() : element_;
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

        virtual ~Element()
        {
            trait.release(value);
        }
    };

    struct NullElementTrait
    {
        const std::string element() const { return "null"; }
        struct null_type {}; 
        typedef null_type ValueType;
        void release(ValueType&) {}
    };
    struct NullElement : Element<NullElement, NullElementTrait> {};

    struct StringElementTrait
    {
        const std::string element() const { return "string"; }
        typedef std::string ValueType;
        void release(ValueType&) {}
    };
    struct StringElement : Element<StringElement, StringElementTrait> {};

    struct NumberElementTrait
    {
        const std::string element() const { return "number"; }
        typedef double ValueType;
        void release(ValueType&) {}
    };
    struct NumberElement : Element<NumberElement, NumberElementTrait> {};

    struct BooleanElementTrait
    {
        const std::string element() const { return "boolean"; }
        typedef bool ValueType;
        void release(ValueType&) {}
    };
    struct BooleanElement : Element<BooleanElement, BooleanElementTrait> {};

    struct ArrayElementTrait
    {
        const std::string element() const { return "array"; }
        typedef std::vector<IElement*> ValueType;
        void release(ValueType& array)
        {
            for (ValueType::iterator it = array.begin(); it != array.end(); ++it) {
                delete (*it);
            }
            array.clear();
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
        const std::string element() const { return "member"; }
        typedef std::pair<IElement*, IElement*> ValueType;
        void release(ValueType& member)
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
    };

    struct MemberElement : Element<MemberElement, MemberElementTrait>
    {
        void set(const std::string& name, IElement* element)
        {
            if (value.first != NULL) {
                delete value.first;
                value.first = NULL;
            }
            StringElement* k = new StringElement;
            k->set(name);
            value.first = k;

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
        const std::string element() const { return "object"; }
        typedef std::vector<MemberElement*> ValueType;
        void release(ValueType& obj)
        {
            for (ValueType::iterator it = obj.begin(); it != obj.end(); ++it) {
                delete (*it);
            }
            obj.clear();
        }
    };

    struct ObjectElement : Element<ObjectElement, ObjectElementTrait>
    {
        void push_back(MemberElement* e)
        {
            // FIXME: 
            // basic diff between ObjectElement and ArrayElement
            // every member of ObjectElement should contain meta["name"] 
            // which can be in compact form of element as "key" of object
            // do we should check for `meta['name']??
            hasContent = true;
            value.push_back(e);
        }
    };

}; // namespace refract

#endif // #ifndef _REFRACT_ELEMENT_H_
