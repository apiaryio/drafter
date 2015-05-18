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

namespace refract {


struct StringElement;
struct NullElement;
struct NumberElement;
struct BooleanElement;
struct ArrayElement;
struct ObjectElement;
struct MemberElement;

struct ComparableVisitor;
struct SerializeVisitor;
struct SerializeCompactVisitor;

#if 0

/*
template<typename T>
bool operator==(const BaseContentProxy& self, const T& other) {
    const ContentProxy<T>& obj = static_cast<const ContentProxy<T>&>(self);
    return obj == other;
}
*/


// NOTE: assign must be solved by function, we cannot declare operator
// out of class declaration

// IDEA: prepare convertor for basic types like "const char*"
// to avoid calling like `assign(e.meta["id"], std::string("str"));`
//template<typename T> void assign(BaseContentProxy* proxy, T* value) {
//    static_cast<ContentProxy<T>*>(proxy)->value = &value;
//}


#endif

template <typename T> struct ElementTypeSelector;

// FIXME: find in Element* for value_type
// instead of specialization templates
template <>
struct ElementTypeSelector<std::string> {
    typedef StringElement ElementType;
};

template <>
struct ElementTypeSelector<char*> {
    typedef StringElement ElementType;
};

template <>
struct ElementTypeSelector<double> {
    typedef NumberElement ElementType;
};

template <>
struct ElementTypeSelector<int> {
    typedef NumberElement ElementType;
};

template <>
struct ElementTypeSelector<bool> {
    typedef BooleanElement ElementType;
};

struct IVisitor;
struct IElement {
    /**
     * define visitors which can work with element
     * via. `content()` method
     */
    typedef typelist::cons<
        ComparableVisitor,
        SerializeVisitor,
        SerializeCompactVisitor
        >::type Visitors;


    bool hasContent;
    bool compactContent;
    IElement() : hasContent(false), compactContent(false) {}


    template<typename T> 
    static IElement* Create(const T& value) {
        typedef typename ElementTypeSelector<T>::ElementType ElementType;
        ElementType* element = new ElementType;
        element->set(value);
        return element;
    };

    static IElement* Create(const char* value);

    struct MemberElementCollection : std::vector<MemberElement*> {
        const_iterator find(const std::string& name) const;
        MemberElement& operator[](const std::string& name);
        MemberElement& operator[](const int index);
        virtual ~MemberElementCollection();
    };

    MemberElementCollection meta;
    MemberElementCollection attributes;

    virtual std::string element() const = 0;
    virtual void element(const std::string&) = 0;

    // FIXME: probably rename to Accept
    // depends on decision:
    //  - visitable actions `VisitableBy` 
    //  - create overrided virtual function for every one `Visitor`
    virtual void content(IVisitor& v) const = 0;

    virtual bool empty() const {
        return !hasContent;
    }

    virtual bool renderCompactContent() const {
        return compactContent;
    }

    virtual void renderCompactContent(bool compact) {
        compactContent = compact;
    }

    virtual ~IElement(){}
};

struct IVisitor {
    virtual ~IVisitor(){};
};

struct ComparableVisitor : IVisitor {
    std::string compare_to;
    bool result;

    ComparableVisitor(const std::string& str) : compare_to(str), result(false) {}

    template <typename T, typename U>
    bool IsEqual(const T& first, const U& second) {
        return false;
    }

    template <typename T>
    bool IsEqual(const T& first, const T& second) {
        return first == second;
    }

    template<typename E> 
    void visit(const E& e) {
        result = IsEqual(compare_to, e.value);
    }

    virtual void visit(const IElement& e) {
        throw std::runtime_error("Fallback impl");
    }

    operator bool() const {
        return result;
    }
};

struct SerializeVisitor : IVisitor {

    sos::Object result; 
    sos::Base partial;
    std::string key;

    SerializeVisitor() : partial(sos::Null()) {}

    void visit(const IElement& e);
    void visit(const NullElement& e); 
    void visit(const StringElement& e); 
    void visit(const NumberElement& e); 
    void visit(const BooleanElement& e); 
    void visit(const ArrayElement& e); 
    void visit(const MemberElement& e); 
    void visit(const ObjectElement& e); 

    sos::Object get() {
        return result;
    }
};

struct SerializeCompactVisitor : IVisitor {
    /*
    template <typename T>
    void visit(const T& e) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
    */
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

    std::string key() {
        return key_;
    }

    sos::Base value() {
        return value_;
    }

    /*

    sos::Object result; 
    sos::Base partial;
    std::string key;

    void visit(const IElement& e);
    void visit(const NullElement& e); 
    void visit(const StringElement& e); 
    void visit(const NumberElement& e); 
    void visit(const BooleanElement& e); 
    void visit(const ArrayElement& e); 
    void visit(const MemberElement& e); 
    void visit(const ObjectElement& e); 

    sos::Object get() {
        return result;
    }
    */
};



template<typename T, typename Trait>
struct Element : public IElement, public VisitableBy<IElement::Visitors> {

    typedef Element<T, Trait> type;

    typedef Trait trait_type;
    trait_type trait;

    typedef typename trait_type::value_type value_type;
    value_type value;


    std::string element_;

    // FIXME return const reference
    virtual std::string element() const { 
        return element_.empty() ? trait.element() : element_; 
    }

    virtual void element(const std::string& name) { 
        element_ = name;
    }

    void set(const value_type& val) {
        hasContent = true;
        value = val;
    }

    const value_type& get() const {
        return value;
    }

    virtual void content(IVisitor& v) const {
        InvokeVisit(v, static_cast<const T&>(*this));
    }

    virtual ~Element() {
        trait.release(value);
    }

};

struct NullElementTrait {
    const std::string element() const { return "null"; }
    struct null_type {};
    typedef null_type value_type;
    void release(value_type&) {}
};
struct NullElement : Element<NullElement, NullElementTrait> {};

struct StringElementTrait {
    const std::string element() const { return "string"; }
    typedef std::string value_type;
    void release(value_type&) {}
};
struct StringElement : Element<StringElement, StringElementTrait> {};

struct NumberElementTrait {
    const std::string element() const { return "number"; }
    typedef double value_type;
    void release(value_type&) {}
};
struct NumberElement : Element<NumberElement, NumberElementTrait> {};

struct BooleanElementTrait {
    const std::string element() const { return "boolean"; }
    typedef bool value_type;
    void release(value_type&) {}
};
struct BooleanElement : Element<BooleanElement, BooleanElementTrait> {};

struct ArrayElementTrait {
    const std::string element() const { return "array"; }
    typedef std::vector<IElement*> value_type;
    void release(value_type& array) {
        for (value_type::iterator it = array.begin() ; it != array.end() ; ++it) {
            delete (*it);
        }
        array.clear();
    }
};

struct ArrayElement : Element<ArrayElement, ArrayElementTrait> {
    void push_back(IElement* e) {
        hasContent = true;
        value.push_back(e);
    }
};

struct MemberElementTrait {
    const std::string element()  const{ return "member"; }
    typedef std::pair<StringElement*, IElement*> value_type;
    void release(value_type& member) {
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

struct MemberElement : Element<MemberElement, MemberElementTrait> {
    void set(const std::string& name, IElement* element) {

        if(value.first != NULL) {
            delete value.first;
            value.first = NULL;
        }
        StringElement* k = new StringElement;
        k->set(name);
        value.first = k;

        if(value.second != NULL) {
            delete value.second;
            value.second = NULL;
        }
        value.second = element;
    }

    MemberElement& operator=(IElement* element) {
        if(value.second != NULL) {
            delete value.second;
            value.second = NULL;
        }
        value.second = element;
        return *this;
    }
};

struct ObjectElementTrait {
    const std::string element() const { return "object"; }
    typedef std::vector<IElement*> value_type;
    void release(value_type& obj) {
        for (value_type::iterator it = obj.begin() ; it != obj.end() ; ++it) {
            delete (*it);
        }
        obj.clear();
    }
};

struct ObjectElement : Element<ObjectElement, ObjectElementTrait> {
    void push_back(IElement* e) {
        // FIXME:  check for meta["name"]
        hasContent = true;
        value.push_back(e);
    }
};

}; // namespace refract

#endif // #if !defined _REFRACT_ELEMENT_H_
