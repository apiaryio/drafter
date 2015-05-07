#include "Element.h"
#include <cassert>

namespace refract {

void SerializaleVisitor::visit(const IElement& e) {
    result.set("element", sos::String(e.element()));

    typedef IElement::MemberElementCollection::const_iterator iterator;

    // FIXME : refactoring
    if(!e.meta.empty()) {
        sos::Array array;
        for( iterator it = e.meta.begin() ; it != e.meta.end() ; ++it ) {
          SerializaleVisitor s;
          s.visit((IElement&)*(*it));
          array.push(s.get());
        }
        result.set("meta", array);
    }

    if(!e.attributes.empty()) {
        sos::Array array;
        for( iterator it = e.meta.begin() ; it != e.meta.end() ; ++it ) {
          SerializaleVisitor s;
          s.visit((IElement&)*(*it));
          array.push(s.get());
        }
        result.set("attributes", array);
    }

    e.content(*this);
    result.set("content", partial);
    
    
    /*
    std::stringstream s;
    Stringify f;
    e.content(f);

    s << "{\n  \"element\": \"" << e.element() << "\",\n";
    // FIXME: refactoring
    if(!e.meta.empty()) {
        s << "  \"meta\": [\n";
        for( IElement::MemberElementCollection::const_iterator it = e.meta.begin() ; it != e.meta.end() ; ++it ) {
          Stringify f;
          f.visit((IElement&)*(*it));
          s << f.get();
        }
        s << "  ],\n";
    }
    if(!e.attributes.empty()) {
        s << "  \"attributes\": [\n";
        for( IElement::MemberElementCollection::const_iterator it = e.attributes.begin() ; it != e.attributes.end() ; ++it ) {
          Stringify f;
          f.visit((IElement&)*(*it));
          s << f.get();
        }
        s << "  ],\n";
    }
    s << "  \"content\": " << f.get() << "\n}";
    value = s.str();
    */
}

static void SetSerializerValue(SerializaleVisitor& s, sos::Base& value) {
    if(!s.key.empty()) {
        s.result.set(s.key, value);
        s.key.clear();
    } else {
        s.partial = value;
    }
}

void SerializaleVisitor::visit(const NullElement& e) {
    sos::Base value = sos::Null();
    SetSerializerValue(*this, value);
}

void SerializaleVisitor::visit(const StringElement& e) {
    sos::Base value = sos::String(e.value);
    SetSerializerValue(*this, value);
}

void SerializaleVisitor::visit(const NumberElement& e) {
    sos::Base value = sos::Number(e.value);
    SetSerializerValue(*this, value);
}

void SerializaleVisitor::visit(const BooleanElement& e) {
    sos::Base value = sos::Boolean(e.value);
    SetSerializerValue(*this, value);
}

void SerializaleVisitor::visit(const ArrayElement& e) {
    sos::Array array;

    typedef ArrayElement::value_type::const_iterator iterator;
    for(iterator it = e.value.begin() ; it != e.value.end() ; ++it ) {
        SerializaleVisitor s;
        s.visit(*(*it));
        array.push(s.get());
    }
    SetSerializerValue(*this, array);
}

void SerializaleVisitor::visit(const MemberElement& e) {
    sos::Object object;

    NullElement null;

    IElement* k = e.value.first ? e.value.first : &null;
    SerializaleVisitor sk;
    sk.visit(*k);
    object.set("key", sk.get());


    k = e.value.second ? e.value.second : &null;
    SerializaleVisitor sv;
    sv.visit(*k);
    object.set("value", sv.get());

    SetSerializerValue(*this, object);
}

void SerializaleVisitor::visit(const ObjectElement& e) {
    sos::Array array;

    typedef ObjectElement::value_type::const_iterator iterator;
    for(iterator it = e.value.begin() ; it != e.value.end() ; ++it ) {
        SerializaleVisitor s;
        s.visit(static_cast<IElement&>(*(*it)));
        array.push(s.get());
    }
    SetSerializerValue(*this, array);
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
