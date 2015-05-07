#include "Element.h"
#include <cassert>

namespace refract {

void SerializeVisitor::visit(const IElement& e) {
    result.set("element", sos::String(e.element()));

    typedef IElement::MemberElementCollection::const_iterator iterator;

    // FIXME : refactoring
    if(!e.meta.empty()) {
        sos::Object obj;
        for( iterator it = e.meta.begin() ; it != e.meta.end() ; ++it ) {
          SerializeCompactVisitor s;
          s.visit(*(*it));
          obj.set(s.key(), s.value());
        }
        result.set("meta", obj);
    }

    if(!e.attributes.empty()) {
        sos::Object obj;
        for( iterator it = e.attributes.begin() ; it != e.attributes.end() ; ++it ) {
          SerializeCompactVisitor s;
          s.visit(*(*it));
          obj.set(s.key(), s.value());
        }
        result.set("attributes", obj);
    }

    e.content(*this);
    result.set("content", partial);
}

static void SetSerializerValue(SerializeVisitor& s, sos::Base& value) {
    if(!s.key.empty()) {
        s.result.set(s.key, value);
        s.key.clear();
    } else {
        s.partial = value;
    }
}

void SerializeVisitor::visit(const NullElement& e) {
    sos::Base value = sos::Null();
    SetSerializerValue(*this, value);
}

void SerializeVisitor::visit(const StringElement& e) {
    sos::Base value = sos::String(e.value);
    SetSerializerValue(*this, value);
}

void SerializeVisitor::visit(const NumberElement& e) {
    sos::Base value = sos::Number(e.value);
    SetSerializerValue(*this, value);
}

void SerializeVisitor::visit(const BooleanElement& e) {
    sos::Base value = sos::Boolean(e.value);
    SetSerializerValue(*this, value);
}

void SerializeVisitor::visit(const ArrayElement& e) {
    sos::Array array;

    typedef ArrayElement::value_type::const_iterator iterator;
    for(iterator it = e.value.begin() ; it != e.value.end() ; ++it ) {
        SerializeVisitor s;
        s.visit(*(*it));
        array.push(s.get());
    }
    SetSerializerValue(*this, array);
}

void SerializeVisitor::visit(const MemberElement& e) {
    throw std::runtime_error("Not Implemented");
}

void SerializeVisitor::visit(const ObjectElement& e) {
    sos::Array array;

    typedef ObjectElement::value_type::const_iterator iterator;
    for(iterator it = e.value.begin() ; it != e.value.end() ; ++it ) {
        SerializeVisitor s;
        s.visit(static_cast<IElement&>(*(*it)));
        array.push(s.get());
    }
    SetSerializerValue(*this, array);
}

void SerializeCompactVisitor::visit(const IElement& e) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void SerializeCompactVisitor::visit(const NullElement& e) {
    value_ = sos::Null();
}

void SerializeCompactVisitor::visit(const StringElement& e) {
    value_ = sos::String(e.value);
}

void SerializeCompactVisitor::visit(const NumberElement& e) {
    value_ = sos::Number(e.value);
}

void SerializeCompactVisitor::visit(const BooleanElement& e) {
    value_ = sos::Boolean(e.value);
}

void SerializeCompactVisitor::visit(const ArrayElement& e) {
    sos::Array array;
    typedef ArrayElement::value_type::const_iterator iterator;
    for( iterator it = e.value.begin() ; it != e.value.end() ; ++it ) {
        SerializeCompactVisitor s;
        (*it)->content(s);
        array.push(s.value());
    }
    value_ = array;
}

void SerializeCompactVisitor::visit(const MemberElement& e) {
    if(e.value.first) {
        SerializeCompactVisitor s;
        e.value.first->content(s);
        key_ = s.value().str;
    }

    if(e.value.second) {
        e.value.second->content(*this);
    }
}

void SerializeCompactVisitor::visit(const ObjectElement& e) {
    throw std::runtime_error("Not Implemented");
}


IElement::MemberElementCollection::const_iterator IElement::MemberElementCollection::find(const std::string& name) const {
    ComparableVisitor v(name);
    const_iterator it = begin();
    for ( ; it != end() ; ++it ) {
        (*it)->value.first->content(v);
        if(v) return it;
    }
    return it;
}

/*
IElement& IElement::MemberElementCollection::operator[](const std::string& name) {
    const_iterator it = find(name);
    if(it != end()) {
        return *(*it)->value.second;
    }

    if(it == end()) { // create new member
        StringElement* key = new StringElement;
        key->set(name);
        MemberElement* e = new MemberElement;
        push_back(e);
    }

    //return e
    //if (e.end()) return NULL;
    //return e->value.second;
}

IElement& IElement::MemberElementCollection::operator[](const int index) {
    //return static_cast<std::vector<MemberElement*> >(*this)[index];
}
*/

}; // namespace refract
