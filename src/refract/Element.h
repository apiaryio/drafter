#if !defined _REFRACT_ELEMENT_H_
#define _REFRACT_ELEMENT_H_

#include <stdio.h>

#include <string>
#include <map>
#include <vector>

#include <sstream>
#include <iostream>

#include <stdexcept>

#include "Typelist.h"
#include "VisitableBy.h"

#include "sos.h"

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


    // Forward declarations of Visitors
    // if you want add new one, do not forget add IElement::Visitors
    struct ComparableVisitor;
    struct SerializeVisitor;
    struct SerializeCompactVisitor;

    struct LogicError : std::logic_error {
        explicit LogicError(const std::string& msg) : std::logic_error(msg) {}
    };
    // will be removed in future
    struct NotImplemented : std::runtime_error {
        explicit NotImplemented(const std::string& msg) : std::runtime_error(msg) {}
    }; 
    struct Deprecated : std::logic_error {
        explicit Deprecated(const std::string& msg) : std::logic_error(msg) {}
    }; 

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

    struct IVisitor;
    struct IElement
    {
        /**
         * define __visitors__ which can visit element
         * via. `content()` method
         */
        typedef typelist::cons<ComparableVisitor, SerializeVisitor, SerializeCompactVisitor>::type Visitors;

        bool hasContent; ///< was content of element already set? \see empty()
        bool useCompactContent;  ///< should be content serialized in compact form? \see compactContent()

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

    struct IVisitor
    {
        virtual ~IVisitor(){};
    };

    struct ComparableVisitor : IVisitor
    {
        std::string compare_to;
        bool result;

        ComparableVisitor(const std::string& str) : compare_to(str), result(false)
        {
        }

        template <typename T, typename U>
        bool IsEqual(const T& first, const U& second)
        {
            return false;
        }

        template <typename T>
        bool IsEqual(const T& first, const T& second)
        {
            return first == second;
        }

        template <typename E>
        void visit(const E& e)
        {
            result = IsEqual(compare_to, e.value);
        }

        virtual void visit(const IElement& e)
        {
            throw LogicError("Fallback impl - behavioration for Base class IElement is not defined");
        }

        operator bool() const
        {
            return result;
        }
    };

    struct SerializeVisitor : IVisitor
    {

        sos::Object result;
        sos::Base partial;
        std::string key;

        SerializeVisitor() : partial(sos::Null())
        {
        }

        void visit(const IElement& e);
        void visit(const NullElement& e);
        void visit(const StringElement& e);
        void visit(const NumberElement& e);
        void visit(const BooleanElement& e);
        void visit(const ArrayElement& e);
        void visit(const MemberElement& e);
        void visit(const ObjectElement& e);

        sos::Object get()
        {
            return result;
        }
    };

    struct SerializeCompactVisitor : IVisitor
    {
        std::string key_;
        sos::Base value_;

        void visit(const IElement& e);
        void visit(const NullElement& e);
        void visit(const StringElement& e);
        void visit(const NumberElement& e);
        void visit(const BooleanElement& e);
        void visit(const ArrayElement& e);
        void visit(const MemberElement& e);
        void visit(const ObjectElement& e);

        std::string key()
        {
            return key_;
        }

        sos::Base value()
        {
            return value_;
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
        TraitType trait;

        typedef typename TraitType::ValueType ValueType;
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
            hasContent = true;
            value.push_back(e);
        }
    };

    struct MemberElementTrait
    {
        const std::string element() const { return "member"; }
        typedef std::pair<StringElement*, IElement*> ValueType;
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
        typedef std::vector<IElement*> ValueType;
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
        void push_back(IElement* e)
        {
            // NOTE: 
            // basic diff between ObjectElement and ArrayElement
            // every member of ObjectElement should contain meta["name"] 
            // which can be in compact form of element as "key" of object
            // do we should check for `meta['name']??
            hasContent = true;
            value.push_back(e);
        }
    };

}; // namespace refract

#endif // #if !defined _REFRACT_ELEMENT_H_
