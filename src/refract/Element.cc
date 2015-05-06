#include "Element.h"

namespace refract {

void ElementSerializer::visit(const IElement& e) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;

    ElementSerializer s;

    result.set("element", sos::String(e.element()));
    result.set("content", sos::Object());
    
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

void ElementSerializer::visit(const NullElement& e) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    //value = "null";
}

void ElementSerializer::visit(const StringElement& e) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    //std::stringstream s;
    //s << "\"" << e.value << "\"";
    ////value = s.str();
}

void ElementSerializer::visit(const NumberElement& e) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    //std::stringstream s;
    //s << e.value;
    //value = s.str();
}

void ElementSerializer::visit(const BooleanElement& e) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    //value = e.value ? "true" : "false";
}

void ElementSerializer::visit(const ArrayElement& e) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    //std::stringstream s;
    //s << "["; 
    //for(ArrayElement::value_type::const_iterator i = e.value.begin() ; i != e.value.end() ; ++ i) {
    //    s << std::endl;
    //    Stringify f;
    //    f.visit(*(*i));
    //    s << f.get() << ",";
    //}
    //s << "\n]";
    //value = s.str();
}

void ElementSerializer::visit(const MemberElement& e) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    //std::stringstream s;
    //Stringify f;

    //s << "{\n"; 

    //NullElement null;
    //IElement* serializable = e.value.first ? e.value.first : &null;
    //f.visit(*(serializable));
    //s << "\"key\": " << f.get() << ",\n";

    //serializable = e.value.second ? e.value.second : &null;
    //f.visit(*(serializable));
    //s << "\"value\": " << f.get() << "\n";

    //s << "}";

    //value = s.str();
}

void ElementSerializer::visit(const ObjectElement& e) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    //std::stringstream s;
    //s << "["; 
    //for(ObjectElement::value_type::const_iterator i = e.value.begin() ; i != e.value.end() ; ++ i) {
    //    s << std::endl;
    //    Stringify f;
    //    f.visit((IElement&)*(*i));
    //    s << f.get() << ",";
    //}
    //s << "\n]";
    //value = s.str();
}

void ElementSerializer::visit(const RefElement& e) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    //std::stringstream s;
    //if(e.value.prefix.empty() && e.value.path.empty()) {
    //    s << "\"" << e.value.href << "\"";
    //} else  {
    //    s << "{\n";
    //    if(!e.value.prefix.empty()) {
    //        s << "\"prefix\": \"" << e.value.prefix << "\",\n";
    //    }
    //    s << "\"href\": \"" << e.value.href << "\"\n";
    //    if(!e.value.path.empty()) {
    //        s << "\"path\": \"" << e.value.path << "\",\n";
    //    }
    //    s << "}";
    //}
    //value = s.str();
}

/////////////

void Stringify::visit(const IElement& e) {
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
}

void Stringify::visit(const NullElement& e) {
    value = "null";
}

void Stringify::visit(const StringElement& e) {
    std::stringstream s;
    s << "\"" << e.value << "\"";
    value = s.str();
}

void Stringify::visit(const NumberElement& e) {
    std::stringstream s;
    s << e.value;
    value = s.str();
}

void Stringify::visit(const BooleanElement& e) {
    value = e.value ? "true" : "false";
}

void Stringify::visit(const ArrayElement& e) {
    std::stringstream s;
    s << "["; 
    for(ArrayElement::value_type::const_iterator i = e.value.begin() ; i != e.value.end() ; ++ i) {
        s << std::endl;
        Stringify f;
        f.visit(*(*i));
        s << f.get() << ",";
    }
    s << "\n]";
    value = s.str();
}

void Stringify::visit(const MemberElement& e) {
    std::stringstream s;
    Stringify f;

    s << "{\n"; 

    NullElement null;
    IElement* serializable = e.value.first ? e.value.first : &null;
    f.visit(*(serializable));
    s << "\"key\": " << f.get() << ",\n";

    serializable = e.value.second ? e.value.second : &null;
    f.visit(*(serializable));
    s << "\"value\": " << f.get() << "\n";

    s << "}";

    value = s.str();
}

void Stringify::visit(const ObjectElement& e) {
    std::stringstream s;
    s << "["; 
    for(ObjectElement::value_type::const_iterator i = e.value.begin() ; i != e.value.end() ; ++ i) {
        s << std::endl;
        Stringify f;
        f.visit((IElement&)*(*i));
        s << f.get() << ",";
    }
    s << "\n]";
    value = s.str();
}

void Stringify::visit(const RefElement& e) {
    std::stringstream s;
    if(e.value.prefix.empty() && e.value.path.empty()) {
        s << "\"" << e.value.href << "\"";
    } else  {
        s << "{\n";
        if(!e.value.prefix.empty()) {
            s << "\"prefix\": \"" << e.value.prefix << "\",\n";
        }
        s << "\"href\": \"" << e.value.href << "\"\n";
        if(!e.value.path.empty()) {
            s << "\"path\": \"" << e.value.path << "\",\n";
        }
        s << "}";
    }
    value = s.str();
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
