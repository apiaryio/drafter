#include "Element.h"
#include <cassert>

namespace refract
{

    void SerializeVisitor::visit(const IElement& e)
    {
        result.set("element", sos::String(e.element()));

        typedef IElement::MemberElementCollection::const_iterator iterator;

        // FIXME : refactoring
        if (!e.meta.empty()) {
            sos::Object obj;
            for (iterator it = e.meta.begin(); it != e.meta.end(); ++it) {
                SerializeCompactVisitor s;
                s.visit(*(*it));
                obj.set(s.key(), s.value());
            }
            result.set("meta", obj);
        }

        if (!e.attributes.empty()) {
            sos::Object obj;
            for (iterator it = e.attributes.begin(); it != e.attributes.end(); ++it) {
                SerializeCompactVisitor s;
                s.visit(*(*it));
                obj.set(s.key(), s.value());
            }
            result.set("attributes", obj);
        }

        if (e.empty())
            return;

        if (e.compactContent()) {
            SerializeCompactVisitor s;
            e.content(s);
            result.set("content", s.value());
        } else {
            e.content(*this);
            result.set("content", partial);
        }
    }

    static void SetSerializerValue(SerializeVisitor& s, sos::Base& value)
    {
        if (!s.key.empty()) {
            s.result.set(s.key, value);
            s.key.clear();
        } else {
            s.partial = value;
        }
    }

    void SerializeVisitor::visit(const NullElement& e)
    {
        sos::Base value = sos::Null();
        SetSerializerValue(*this, value);
    }

    void SerializeVisitor::visit(const StringElement& e)
    {
        sos::Base value = sos::Null();
        if (!e.empty()) {
            value = sos::String(e.value);
        }
        SetSerializerValue(*this, value);
    }

    void SerializeVisitor::visit(const NumberElement& e)
    {
        sos::Base value = sos::Null();
        if (!e.empty()) {
            value = sos::Number(e.value);
        }
        SetSerializerValue(*this, value);
    }

    void SerializeVisitor::visit(const BooleanElement& e)
    {
        sos::Base value = sos::Null();
        if (!e.empty()) {
            value = sos::Boolean(e.value);
        }
        SetSerializerValue(*this, value);
    }

    void SerializeVisitor::visit(const ArrayElement& e)
    {
        sos::Array array;

        typedef ArrayElement::ValueType::const_iterator iterator;
        for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
            SerializeVisitor s;
            s.visit(*(*it));
            array.push(s.get());
        }
        SetSerializerValue(*this, array);
    }

    void SerializeVisitor::visit(const MemberElement& e)
    {
        throw std::runtime_error("MemberElement is DEPRECATED use Meta[\"name\"] instead");
    }

    void SerializeVisitor::visit(const ObjectElement& e)
    {
        sos::Array array;

        typedef ObjectElement::ValueType::const_iterator iterator;
        for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
            SerializeVisitor s;
            s.visit(static_cast<IElement&>(*(*it)));
            array.push(s.get());
        }
        SetSerializerValue(*this, array);
    }

    void SerializeCompactVisitor::visit(const IElement& e)
    {
        throw std::runtime_error("NI: IElement Compact Serialization");
    }

    void SerializeCompactVisitor::visit(const NullElement& e)
    {
        value_ = sos::Null();
    }

    void SerializeCompactVisitor::visit(const StringElement& e)
    {
        value_ = sos::String(e.value);
    }

    void SerializeCompactVisitor::visit(const NumberElement& e)
    {
        value_ = sos::Number(e.value);
    }

    void SerializeCompactVisitor::visit(const BooleanElement& e)
    {
        value_ = sos::Boolean(e.value);
    }

    void SerializeCompactVisitor::visit(const ArrayElement& e)
    {
        sos::Array array;
        typedef ArrayElement::ValueType::const_iterator iterator;
        for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
            SerializeCompactVisitor s;
            (*it)->content(s);
            array.push(s.value());
        }
        value_ = array;
    }

    void SerializeCompactVisitor::visit(const MemberElement& e)
    {
        if (e.value.first) {
            SerializeCompactVisitor s;
            e.value.first->content(s);
            key_ = s.value().str;
        }

        if (e.value.second) {
            e.value.second->content(*this);
        }
    }

    void SerializeCompactVisitor::visit(const ObjectElement& e)
    {
        sos::Object obj;
        typedef ObjectElement::ValueType::const_iterator iterator;
        for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
            SerializeCompactVisitor sv;
            (*it)->content(sv);

            IElement::MemberElementCollection::const_iterator name = (*it)->meta.find("name");
            if (name == (*it)->meta.end() || (*name)->value.second == NULL) {
                throw std::logic_error("Object has not set name");
            }

            SerializeCompactVisitor sk;
            (*name)->content(sk);
            if (sk.value().type != sos::Base::StringType) {
                throw std::logic_error("Metadata element 'name' does not contain string type");
            }

            obj.set(sk.value().str, sv.value());
        }
        value_ = obj;
    }

    IElement::MemberElementCollection::const_iterator
    IElement::MemberElementCollection::find(const std::string& name) const
    {
        ComparableVisitor v(name);
        const_iterator it = begin();
        for (; it != end(); ++it) {
            (*it)->value.first->content(v);
            if (v)
                return it;
        }
        return it;
    }

    IElement* IElement::Create(const char* value)
    {
        StringElement* element = new StringElement;
        element->set(value);
        return element;
    };

    MemberElement& IElement::MemberElementCollection::operator[](const std::string& name)
    {
        const_iterator it = find(name);
        if (it != end()) {
            return *(*it);
        }

        StringElement* key = new StringElement;
        key->set(name);
        MemberElement* member = new MemberElement;
        member->value.first = key;
        push_back(member);
        return *member;
    }

    IElement::MemberElementCollection::~MemberElementCollection()
    {
        for (iterator it = begin(); it != end(); ++it) {
            delete (*it);
        }
        clear();
    }

    MemberElement& IElement::MemberElementCollection::operator[](const int index)
    {
        // FIXME: static assert;
        throw std::runtime_error("Do nor use number index");
    }

}; // namespace refract
