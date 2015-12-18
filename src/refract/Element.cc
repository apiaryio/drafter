//
//  refract/Element.cc
//  librefract
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#include "Element.h"
#include <cassert>

#include <set>
#include <string>

#include "Visitors.h"

namespace refract
{

    bool isReserved(const std::string& element) {
        static std::set<std::string> reserved;
        if (reserved.empty()) {
            reserved.insert("null");
            reserved.insert("boolean");
            reserved.insert("number");
            reserved.insert("string");

            reserved.insert("member");

            reserved.insert("array");
            reserved.insert("enum");
            reserved.insert("object");

            reserved.insert("ref");
            reserved.insert("select");
            reserved.insert("option");
            reserved.insert("extend");

            reserved.insert("generic");
        }

        return reserved.find(element) != reserved.end();
    }

    IElement::MemberElementCollection::const_iterator IElement::MemberElementCollection::find(const std::string& name) const
    {
        ComparableVisitor v(name);
        const_iterator it;

        for (it = begin(); it != end(); ++it) {
            (*it)->value.first->content(v);

            if (v.get()) {
                return it;
            }
        }

        return it;
    }

    IElement::MemberElementCollection::iterator IElement::MemberElementCollection::find(const std::string& name)
    {
        ComparableVisitor v(name);
        iterator it;

        for (it = begin(); it != end(); ++it) {
            (*it)->value.first->content(v);

            if (v.get()) {
                return it;
            }
        }

        return it;
    }

    StringElement* IElement::Create(const char* value)
    {
        return Create(std::string(value));
    };

    MemberElement& IElement::MemberElementCollection::operator[](const std::string& name)
    {
        const_iterator it = find(name);
        if (it != end()) {
            return *(*it);
        }

        // key not found - create new one and return reference
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
        // IDEA : use static assert;
        throw LogicError("Do not use number index");
    }

    void IElement::MemberElementCollection::clone(const IElement::MemberElementCollection& other)
    {
        for (const_iterator it = other.begin() ; it != other.end() ; ++it) {
            push_back(static_cast<value_type>((*it)->clone()));
        }
    }

    void IElement::MemberElementCollection::erase(const std::string& key)
    {
        iterator it = find(key);

        if (it != end()) {
            if (*it) {
                delete (*it);
            }

            std::vector<MemberElement*>::erase(it);
        }
    }

    namespace {
        struct TypeChecker {
            const ExtendElement::ValueType& values;

            TypeChecker(const ExtendElement::ValueType& values) : values(values)
            {
            }

            operator bool() {
                if (values.empty()) {
                    return true;
                }

                TypeQueryVisitor v;
                v.visit(*values.front());

                const TypeQueryVisitor::ElementType base = v.get();
                for (ExtendElement::ValueType::const_iterator i = values.begin() ; i != values.end() ; ++i) {
                    v.visit(*(*i));
                    if (base != v.get()) {
                        return false;
                    }
                }
                return true;
            }
        };
    }

    void ExtendElement::set(const ExtendElement::ValueType& val)
    {
        if (!TypeChecker(val)) {
            throw LogicError("ExtendElement require to be composed form Elements of Equal Type");
        }

        if (val.empty()) {
            return;
        }

        hasContent = true;
        value = val;
    }

    void ExtendElement::push_back(IElement* e) 
    {

        if (!e) {
            return;
        }

        if (!value.empty()) {
            TypeQueryVisitor baseType;
            baseType.visit(*value.front());

            TypeQueryVisitor type;
            type.visit(*e);

            if (baseType.get() != type.get()) {
                throw LogicError("ExtendElement must be composed from Elements of same type");
            }
        }

        hasContent = true;
        value.push_back(e);
    }

    namespace {
        typedef std::vector<IElement*> RefractElements;

        class ElementMerger {

            IElement* result;
            TypeQueryVisitor::ElementType base;


            /**
             * Merge strategy for Primitive types - just replace by latest value
             */
            template <typename T, typename V = typename T::ValueType> 
            struct ValueMerge {
                V& value;
                ValueMerge(T& element) : value(element.value) {}

                void operator()(const T& merge) { 
                    value = merge.value;
                }
            };

            /**
             * Merge stategy for objects/array
             * - if member 
             *   - without existing key -> append
             *   - with existing key - replace old value
             * - if not member
             *   - if empty value -> ignore (type holder for array)
             *   - else append
             */
            template <typename T>
            struct ValueMerge<T, RefractElements> {
                typename T::ValueType& value;

                ValueMerge(T& element) : value(element.value) {}

                void operator()(const T& merge) {
                    typedef std::map<std::string, MemberElement*> MapKeyToMember;
                    MapKeyToMember keysBase;

                    for (RefractElements::iterator it = value.begin() ; it != value.end() ; ++it) {
                        if (MemberElement* member = TypeQueryVisitor::as<MemberElement>(*it)) {

                            if (StringElement* key = TypeQueryVisitor::as<StringElement>(member->value.first)) {
                                keysBase[key->value] = member;
                            }
                        }
                    }

                    for (RefractElements::const_iterator it = merge.value.begin() ; it != merge.value.end() ; ++it) {
                        if (MemberElement* member = TypeQueryVisitor::as<MemberElement>(*it)) {
                            if (StringElement* key = TypeQueryVisitor::as<StringElement>(member->value.first)) {
                                MapKeyToMember::iterator iKey = keysBase.find(key->value);

                                if (iKey != keysBase.end()) { // key is already presented, replace value
                                    delete iKey->second->value.second;
                                    iKey->second->value.second = member->value.second->clone();
                                } 
                                else { // unknown key, append value
                                    MemberElement* clone = static_cast<MemberElement*>(member->clone());
                                    value.push_back(clone);
                                    keysBase[key->value] = clone;
                                }
                            }
                        }
                        else if(!(*it)->empty()) { // merge member is not MemberElement, append value
                            value.push_back((*it)->clone());
                        }
                    }
                }
            };

            template <typename T>
            struct InfoMerge {
                IElement::MemberElementCollection& info;
                InfoMerge(IElement::MemberElementCollection& info) : info(info) {}

                void operator()(const IElement::MemberElementCollection& append) {
                    for (IElement::MemberElementCollection::const_iterator it = append.begin() ; it != append.end() ; ++it) {
                        if (StringElement* key = TypeQueryVisitor::as<StringElement>((*it)->value.first)) {
                            IElement::MemberElementCollection::iterator item = info.find(key->value);

                            if (item != info.end()) {
                                delete (*item)->value.second;
                                (*item)->value.second = (*it)->value.second->clone();
                            }
                        }
                    }
                }
            };

            /**
             * precondition - target && append element MUST BE of same type
             * we use static_cast<> without checking type t is responsibility if caller
             */
            template <typename T>
            static void doMerge(IElement* target, const IElement* append) {
                typedef T ElementType;

                InfoMerge<T>(target->meta)(append->meta);
                InfoMerge<T>(target->attributes)(append->attributes);

                ValueMerge<T>(static_cast<ElementType&>(*target))
                             (static_cast<const ElementType&>(*append));
            }

        public:

            ElementMerger() : result(NULL), base(TypeQueryVisitor::Unknown) {}

            void operator()(const IElement* e) {
                if (!e) {
                    return;
                }

                if (!result) {
                    result = e->clone();

                    TypeQueryVisitor type;
                    type.visit(*result);
                    base = type.get();
                    return;
                }

                TypeQueryVisitor type;
                e->content(type);

                if(type.get() != base) {
                    throw refract::LogicError("Can not merge different types of elements");
                }

                switch(base) {
                    case TypeQueryVisitor::String:
                        doMerge<StringElement>(result, e);
                        return;

                    case TypeQueryVisitor::Number:
                        doMerge<NumberElement>(result, e);
                        return;

                    case TypeQueryVisitor::Boolean:
                        doMerge<BooleanElement>(result, e);
                        return;

                    case TypeQueryVisitor::Array:
                        doMerge<ArrayElement>(result, e);
                        return;

                    case TypeQueryVisitor::Object:
                        doMerge<ObjectElement>(result, e);
                        return;

                    case TypeQueryVisitor::Member:
                        doMerge<MemberElement>(result, e);
                        return;

                    case TypeQueryVisitor::Null:
                    case TypeQueryVisitor::Extend:
                    default:
                        ;
                }
            }

            operator IElement* () const { 
                return result; 
            }

        };
    }

    IElement* ExtendElement::merge() const
    {
        return std::for_each(value.begin(), value.end(), ElementMerger());
    }

}; // namespace refract
