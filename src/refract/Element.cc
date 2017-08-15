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

namespace refract
{

    bool isReserved(const std::string& element)
    {
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

    IElement::MemberElementCollection::const_iterator IElement::MemberElementCollection::find(
        const std::string& name) const
    {
        ComparableVisitor v(name);
        Visitor visitor(v);

        return std::find_if(elements.begin(), elements.end(), [&visitor, &v](const MemberElement* e) {
            e->value.first->content(visitor);
            return v.get();
        });
    }

    IElement::MemberElementCollection::iterator IElement::MemberElementCollection::find(const std::string& name)
    {
        ComparableVisitor v(name);
        Visitor visitor(v);

        return std::find_if(elements.begin(), elements.end(), [&visitor, &v](const MemberElement* e) {
            e->value.first->content(visitor);
            return v.get();
        });
    }

    IElement::MemberElementCollection::~MemberElementCollection()
    {
        for (MemberElement* e : elements)
            delete e;
    }

    StringElement* IElement::Create(const char* value)
    {
        return Create(std::string(value));
    };

    MemberElement& IElement::MemberElementCollection::operator[](const std::string& name)
    {
        auto it = find(name);
        if (it != elements.end()) {
            return *(*it);
        }

        elements.emplace_back(new MemberElement(new StringElement(name), nullptr));

        return *elements.back();
    }

    void IElement::MemberElementCollection::clone(const IElement::MemberElementCollection& other)
    {
        for (const auto& el : other.elements) {
            elements.emplace_back(static_cast<MemberElement*>(el->clone()));
        }
    }

    void IElement::MemberElementCollection::erase(const std::string& key)
    {
        auto it = find(key);

        if (it != elements.end()) {
            delete *it;
            elements.erase(it);
        }
    }

    namespace
    {

        bool TypeChecker(const ExtendElement::ValueType& values)
        {
            if (values.empty()) {
                return true;
            }

            TypeQueryVisitor v;
            Visit(v, *values.front());

            const TypeQueryVisitor::ElementType base = v.get();
            for (ExtendElement::ValueType::const_iterator i = values.begin(); i != values.end(); ++i) {
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

    namespace
    {

        class ElementMerger
        {

            IElement* result;
            TypeQueryVisitor::ElementType base;

            /**
             * Merge strategy for Primitive types - just replace by latest value
             */
            template <typename T, typename V = typename T::ValueType>
            struct ValueMerge {
                V& value;
                ValueMerge(T& element) : value(element.value)
                {
                }

                void operator()(const T& merge)
                {
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

                ValueMerge(T& element) : value(element.value)
                {
                }

                void operator()(const T& merge)
                {
                    typedef std::map<std::string, MemberElement*> MapKeyToMember;
                    typedef std::map<std::string, RefElement*> MapNameToRef;

                    MapKeyToMember keysBase;
                    MapNameToRef refBase;

                    for (RefractElements::iterator it = value.begin(); it != value.end(); ++it) {
                        if (MemberElement* member = TypeQueryVisitor::as<MemberElement>(*it)) {

                            if (StringElement* key = TypeQueryVisitor::as<StringElement>(member->value.first)) {
                                keysBase[key->value] = member;
                            }
                        } else if (RefElement* ref = TypeQueryVisitor::as<RefElement>(*it)) {
                            refBase[ref->value] = ref;
                        }
                    }

                    for (RefractElements::const_iterator it = merge.value.begin(); it != merge.value.end(); ++it) {
                        if (MemberElement* member = TypeQueryVisitor::as<MemberElement>(*it)) {
                            if (StringElement* key = TypeQueryVisitor::as<StringElement>(member->value.first)) {
                                MapKeyToMember::iterator iKey = keysBase.find(key->value);

                                if (iKey != keysBase.end()) { // key is already presented, replace value
                                    delete iKey->second->value.second;
                                    iKey->second->value.second = member->value.second->clone();

                                    std::set<std::string> emptySet;
                                    InfoMerge<T>(iKey->second->meta, emptySet)(member->meta);
                                    InfoMerge<T>(iKey->second->attributes, emptySet)(member->attributes);
                                } else { // unknown key, append value
                                    MemberElement* clone = static_cast<MemberElement*>(member->clone());
                                    value.push_back(clone);
                                    keysBase[key->value] = clone;
                                }
                            }
                        } else if (RefElement* ref = TypeQueryVisitor::as<RefElement>(*it)) {
                            if (refBase.find(ref->value) == refBase.end()) {
                                RefElement* clone = static_cast<RefElement*>(member->clone());
                                value.push_back(clone);
                                refBase[ref->value] = clone;
                            }
                        } else if (!(*it)->empty()) { // merge member is not MemberElement, append value
                            value.push_back((*it)->clone());
                        }
                    }
                }
            };

            template <typename T>
            class InfoMerge
            {
                IElement::MemberElementCollection& info;
                const std::set<std::string>& noMergeKeys;

            public:
                InfoMerge(IElement::MemberElementCollection& info, const std::set<std::string>& noMergeKeys)
                    : info(info), noMergeKeys(noMergeKeys)
                {
                }

                void operator()(const IElement::MemberElementCollection& append)
                {
                    IElement::MemberElementCollection toAppend;

                    for (IElement::MemberElementCollection::const_iterator it = append.begin(); it != append.end();
                         ++it) {

                        if (!*it) {
                            continue;
                        }

                        if (StringElement* key = TypeQueryVisitor::as<StringElement>((*it)->value.first)) {
                            if (noMergeKeys.find(key->value) != noMergeKeys.end()) {
                                // this key should not be merged
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

                    for (IElement::MemberElementCollection::const_iterator it = toAppend.begin(); it != toAppend.end();
                         ++it) {
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
            static void doMerge(IElement* target, const IElement* append)
            {
                typedef T ElementType;

                // FIXME: static initialization on C++11
                static std::set<std::string> noMeta;
                static std::set<std::string> noAttributes;

                if (noMeta.empty()) {
                    noMeta.insert("id");
                    noMeta.insert("prefix");
                    noMeta.insert("namespace");
                }

                InfoMerge<T>(target->meta, noMeta)(append->meta);
                InfoMerge<T>(target->attributes, noAttributes)(append->attributes);

                ValueMerge<T>(static_cast<ElementType&>(*target))(static_cast<const ElementType&>(*append));
            }

        public:
            ElementMerger() : result(NULL), base(TypeQueryVisitor::Unknown)
            {
            }

            void operator()(const IElement* e)
            {
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

                if (type.get() != base) {
                    throw refract::LogicError("Can not merge different types of elements");
                }

                switch (base) {
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

                    case TypeQueryVisitor::Ref:
                        doMerge<RefElement>(result, e);
                        return;

                    case TypeQueryVisitor::Member:
                    case TypeQueryVisitor::Extend:
                    case TypeQueryVisitor::Null:
                        throw LogicError("Unappropriate kind of element to merging");
                    default:
                        throw LogicError("Element has no implemented merging");
                }
            }

            operator IElement*() const
            {
                return result;
            }
        };
    }

    IElement* ExtendElement::merge() const
    {
        return std::for_each(value.begin(), value.end(), ElementMerger());
    }

}; // namespace refract
