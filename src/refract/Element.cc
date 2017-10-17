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
#include <map>
#include <string>

#include "ComparableVisitor.h"
#include "TypeQueryVisitor.h"

#include <string.h>

namespace refract
{

    namespace {
        const constexpr std::array<const char*, 13> reservedKeywords = {

            "null",
            "boolean",
            "number",
            "string",

            "member",

            "array",
            "enum",
            "object",

            "ref",
            "select",
            "option",
            "extend",

            "generic",
        };

        const constexpr std::array<const char*, 3> noMetaKeywords = {
            "id",
            "prefix",
            "namespace" 
        };

        const constexpr std::array<const char*, 0> emptyArray = {
        };

        template <typename Container>
        struct inKeys {
            const Container& keywords;
            inKeys(const Container& keywords) : keywords(keywords) {}

            bool operator()(const std::string& searched) {
                return std::any_of(keywords.begin(), keywords.end(),
                                   [&searched](const char* key) {
                                       return !strcmp(key, searched.c_str());
                                   });
            }
        };

        template <typename Container>
        inKeys<Container> InKeysChecker(const Container& keywords) {
            return inKeys<Container>(keywords);
        }
    }



    bool isReserved(const std::string& element) {
        return InKeysChecker(reservedKeywords)(element);
    }

    IElement::MemberElementCollection::const_iterator IElement::MemberElementCollection::find(const std::string& name) const
    {
        ComparableVisitor v(name);
        Visitor visitor(v);
        const_iterator it;

        for (it = begin(); it != end(); ++it) {
            (*it)->value.first->content(visitor);

            if (v.get()) {
                return it;
            }
        }

        return it;
    }

    IElement::MemberElementCollection::iterator IElement::MemberElementCollection::find(const std::string& name)
    {
        ComparableVisitor v(name);
        Visitor visitor(v);
        iterator it;

        for (it = begin(); it != end(); ++it) {
            (*it)->value.first->content(visitor);

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
        MemberElement* member = new MemberElement;
        member->value.first = new StringElement(name);
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

        bool TypeChecker(const ExtendElement::ValueType& values)
        {
            if (values.empty()) {
                return true;
            }

            TypeQueryVisitor v;
            Visit(v, *values.front());

            const TypeQueryVisitor::ElementType base = v.get();
            for (ExtendElement::ValueType::const_iterator i = values.begin() ; i != values.end() ; ++i) {
                Visit(v, *(*i));
                if (base != v.get()) {
                    return false;
                }
            }
            return true;
        }
    }

    void ExtendElement::set(const ExtendElement::ValueType& val)
    {
        if (!TypeChecker(val)) {
            throw LogicError("ExtendElement must be composed from Elements of same type");
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
            Visit(baseType, *value.front());

            TypeQueryVisitor type;
            Visit(type, *e);

            if (baseType.get() != type.get()) {
                throw LogicError("ExtendElement must be composed from Elements of same type");
            }
        }

        hasContent = true;
        value.push_back(e);
    }

    namespace {

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

                                    InfoMerge()(iKey->second->meta, member->meta, InKeysChecker(noMetaKeywords));
                                    InfoMerge()(iKey->second->attributes, member->attributes, InKeysChecker(emptyArray));
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


            class InfoMerge {

            public:

                void operator()(IElement::MemberElementCollection& info, const IElement::MemberElementCollection& append, std::function<bool (const std::string&)> noMerge) {
                    IElement::MemberElementCollection toAppend;

                    for (IElement::MemberElementCollection::const_iterator it = append.begin() ; it != append.end() ; ++it) {

                        if (!*it) {
                            continue;
                        }

                        if (StringElement* key = TypeQueryVisitor::as<StringElement>((*it)->value.first)) {
                            if (noMerge(key->value)) {
                                continue;
                            }

                            IElement::MemberElementCollection::iterator item = info.find(key->value);
                            if (item != info.end()) {
                                // this key alrady exist, replace value
                                delete (*item)->value.second;
                                (*item)->value.second = (*it)->value.second->clone();
                                continue;
                            }
                        }

                        toAppend.push_back(static_cast<MemberElement*>((*it)->clone()));
                    }

                    for (IElement::MemberElementCollection::const_iterator it = toAppend.begin() ; it != toAppend.end() ; ++it) {
                        info.push_back(*it);
                    }

                    toAppend.clear();
                }
            };

            /**
             * precondition - target && append element MUST BE of same type
             * we use static_cast<> without checking type this is responsibility of caller
             */
            template <typename T>
            static void doMerge(IElement* target, const IElement* append) {
                typedef T ElementType;

                InfoMerge()(target->meta, append->meta, InKeysChecker(noMetaKeywords));
                InfoMerge()(target->attributes, append->attributes, InKeysChecker(emptyArray));

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
                    Visit(type, *result);
                    base = type.get();
                    return;
                }

                TypeQueryVisitor type;
                VisitBy(*e, type);

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

                    case TypeQueryVisitor::Enum:
                        doMerge<EnumElement>(result, e);
                        return;

                    case TypeQueryVisitor::Member:
                    case TypeQueryVisitor::Extend:
                    case TypeQueryVisitor::Null:
                        throw LogicError("Unappropriate kind of element to merging");
                    default:
                        throw LogicError("Element has no implemented merging");
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
