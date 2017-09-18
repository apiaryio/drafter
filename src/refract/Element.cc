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
        static const std::set<std::string> reserved = { "null",
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

            "generic" };

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

            TypeQueryVisitor visitor;
            Visit(visitor, *values.front());

            const TypeQueryVisitor::ElementType base = visitor.get();

            return std::all_of(values.begin(), values.end(), [&visitor, base](const IElement* e) {
                Visit(visitor, *e);
                return base == visitor.get();
            });
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

            template <typename T>
            struct ValueMerge<T, IElement*> {
                IElement*& value;
                ValueMerge(T& element) : value(element.value)
                {
                }

                void operator()(const T& merge)
                {
                    if (!value && merge.value) {
                        value = merge.value->clone();
                    }
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

                    for (auto& element : value) {
                        if (MemberElement* member = TypeQueryVisitor::as<MemberElement>(element)) {

                            if (StringElement* key = TypeQueryVisitor::as<StringElement>(member->value.first)) {
                                keysBase[key->value] = member;
                            }
                        } else if (RefElement* ref = TypeQueryVisitor::as<RefElement>(element)) {
                            refBase[ref->value] = ref;
                        }
                    }

                    for (auto const& element : merge.value) {
                        if (MemberElement* member = TypeQueryVisitor::as<MemberElement>(element)) {
                            if (StringElement* key = TypeQueryVisitor::as<StringElement>(member->value.first)) {
                                MapKeyToMember::iterator iKey = keysBase.find(key->value);

                                if (iKey != keysBase.end()) { // key is already presented, replace value
                                    delete iKey->second->value.second;
                                    iKey->second->value.second = member->value.second->clone();

                                    CollectionMerge<T>(iKey->second->meta, {})(member->meta);
                                    CollectionMerge<T>(iKey->second->attributes, {})(member->attributes);
                                } else { // unknown key, append value
                                    MemberElement* clone = static_cast<MemberElement*>(member->clone());
                                    value.push_back(clone);
                                    keysBase[key->value] = clone;
                                }
                            }
                        } else if (RefElement* ref = TypeQueryVisitor::as<RefElement>(element)) {
                            if (refBase.find(ref->value) == refBase.end()) {
                                RefElement* clone = static_cast<RefElement*>(member->clone());
                                value.push_back(clone);
                                refBase[ref->value] = clone;
                            }
                        } else if (!(element)->empty()) { // merge member is not MemberElement, append value
                            value.push_back(element->clone());
                        }
                    }
                }
            };

            template <typename T>
            class CollectionMerge
            {
                using KeySet = std::set<std::string>;
                IElement::MemberElementCollection& info;
                const KeySet& noMergeKeys;

            public:
                CollectionMerge(IElement::MemberElementCollection& info, const KeySet& noMergeKeys)
                    : info(info), noMergeKeys(noMergeKeys)
                {
                }

                void operator()(const IElement::MemberElementCollection& append)
                {
                    IElement::MemberElementCollection toAppend;

                    for (const auto& member : append) {

                        assert(member);

                        if (!member) {
                            continue;
                        }

                        if (StringElement* key = TypeQueryVisitor::as<StringElement>(member->value.first)) {
                            if (noMergeKeys.find(key->value) != noMergeKeys.end()) {
                                // this key should not be merged
                                continue;
                            }

                            IElement::MemberElementCollection::iterator item = info.find(key->value);
                            if (item != info.end()) {
                                // this key alrady exist, replace value
                                delete (*item)->value.second;
                                (*item)->value.second = member->value.second->clone();
                                continue;
                            }
                        }

                        toAppend.push_back(static_cast<MemberElement*>(member->clone()));
                    }

                    for (const auto& member : toAppend) {
                        info.push_back(member);
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

                CollectionMerge<T>(target->meta, { "id", "prefix", "namespace" })(append->meta);
                CollectionMerge<T>(target->attributes, {})(append->attributes);

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
