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
struct SerializaleVisitor;

/*
template<typename T>
bool operator==(const BaseContentProxy& self, const T& other) {
    const ContentProxy<T>& obj = static_cast<const ContentProxy<T>&>(self);
    return obj == other;
}
*/

#if 0

// NOTE: assign must be solved by function, we cannot declare operator
// out of class declaration

// IDEA: prepare convertor for basic types like "const char*"
// to avoid calling like `assign(e.meta["id"], std::string("str"));`
//template<typename T> void assign(BaseContentProxy* proxy, T* value) {
//    static_cast<ContentProxy<T>*>(proxy)->value = &value;
//}

template <typename T>
struct ElementTypeSelector {
};

#endif

struct IVisitor;
struct IElement {
    /**
     * define visitors which can work with element
     * via. `content()` method
     */
    typedef typelist::cons<
        ComparableVisitor,
        SerializaleVisitor
        >::type Visitors;

    struct MemberElementCollection : std::vector<MemberElement*> {
        const_iterator find(const std::string& name) const;
        //IElement& operator[](const std::string& name);
        //IElement& operator[](const int index);
    };

    MemberElementCollection meta;
    MemberElementCollection attributes;

    virtual std::string element() const = 0;

    // FIXME: probably rename to Accept
    // depends on decision:
    //  - visitable actions `VisitableBy` 
    //  - create overrided virtual function for every one `Visitor`
    virtual void content(IVisitor& v) const = 0;

    //virtual const MemberElementCollection& meta() const = 0;

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

struct SerializaleVisitor : IVisitor {

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
};

template<typename T, typename Trait>
struct Element : public IElement, public VisitableBy<IElement::Visitors> {

    typedef Element<T, Trait> type;

    typedef Trait trait_type;
    trait_type trait;

    typedef typename trait_type::value_type value_type;
    value_type value;

    std::string element() const { return trait.element(); }

    void set(const value_type& val) {
        value = val;
    }

    const value_type& get() const {
        return value;
    }

    virtual void content(IVisitor& v) const {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        InvokeVisit(v, static_cast<const T&>(*this));
    }
};

struct ElementTrait {
    const std::string element() const { return "element"; }
};

struct NullElementTrait : public ElementTrait {
    const std::string element() const { return "null"; }
    struct null_type {};
    typedef null_type value_type;
};

struct StringElementTrait : public ElementTrait {
    const std::string element() const { return "string"; }
    typedef std::string value_type;
};

struct NumberElementTrait : public ElementTrait {
    const std::string element() const { return "number"; }
    typedef double value_type;
};

struct BooleanElementTrait : public ElementTrait {
    const std::string element() const { return "boolean"; }
    typedef bool value_type;
};

struct ArrayElementTrait : public ElementTrait {
    const std::string element() const { return "array"; }
    typedef std::vector<IElement*> value_type;
};

struct MemberElementTrait : public ElementTrait {
    const std::string element()  const{ return "member"; }
    typedef std::pair<IElement*, IElement*> value_type;
};

struct ObjectElementTrait : public ElementTrait {
    const std::string element() const { return "object"; }
    typedef std::vector<MemberElement*> value_type;
};

//
// NOTE: maybe use typedef instead of Inheritance
// 

struct NullElement : Element<NullElement, NullElementTrait> {};
struct StringElement : Element<StringElement, StringElementTrait> {};
struct NumberElement : Element<NumberElement, NumberElementTrait> {};
struct BooleanElement : Element<BooleanElement, BooleanElementTrait> {};
struct MemberElement : Element<MemberElement, MemberElementTrait> {};

struct ArrayElement : Element<ArrayElement, ArrayElementTrait> {
    void push_back(IElement* e) {
        value.push_back(e);
    }
};

struct ObjectElement : Element<ObjectElement, ObjectElementTrait> {
    void push_back(MemberElement* e) {
        value.push_back(e);
    }
};

}; // namespace refract

#endif // #if !defined _REFRACT_ELEMENT_H_
