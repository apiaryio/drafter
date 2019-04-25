//
//  refract/dsd/Extend.cc
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "Extend.h"

#include <algorithm>
#include <cassert>
#include <map>
#include <set>

#include "../Exception.h"
#include "../Element.h"
#include "../InfoElements.h"
#include "../TypeQueryVisitor.h"
#include "../Utils.h"
#include "../PrintVisitor.h"

using namespace refract;
using namespace dsd;

const char* Extend::name = "extend";

static_assert(supports_erase<Extend>::value, "");
static_assert(supports_empty<Extend>::value, "");
static_assert(supports_insert<Extend>::value, "");
static_assert(supports_push_back<Extend>::value, "");
static_assert(supports_begin<Extend>::value, "");
static_assert(supports_end<Extend>::value, "");
static_assert(supports_size<Extend>::value, "");
static_assert(supports_erase<Extend>::value, "");
static_assert(is_iterable<Extend>::value, "");
static_assert(supports_merge<Extend>::value, "");

static_assert(!supports_key<Extend>::value, "");
static_assert(!supports_value<Extend>::value, "");
static_assert(!is_pair<Extend>::value, "");

namespace
{
    template <typename Skip>
    struct InfoMerge : private Skip {
        InfoElements& operator()(InfoElements& target, const InfoElements& source) const noexcept
        {
            for (const auto& entry : source) {
                assert(entry.second);
                if (!Skip::operator()(entry.first))
                    target.set(entry.first, entry.second->clone());
            }
            return target;
        }
    };

    struct SkipNothing {
        bool operator()(const std::string&) const noexcept
        {
            return false;
        }
    };

    struct SkipMetaKeywords {
        bool operator()(const std::string& key) const noexcept
        {
            return (key == "id" || key == "prefix" || key == "namespace");
        }
    };

    template <typename T, bool IsContainer = is_iterable<typename T::ValueType>::value>
    struct ValueMerge;

    /**
     * Merge strategy for Primitive types - just replace by latest value */
    template <typename T>
    struct ValueMerge<T, false> {
        void operator()(T& value, const T& merge) const
        {
            value.set(merge.get());
        }
    };

    template <>
    struct ValueMerge<ObjectElement, true> {

        void operator()(ObjectElement& value, const ObjectElement& merge) const
        {
            if (!merge.empty()) {
                for (const auto& m : merge.get()) {
                    if (value.empty())
                        value.set();
                    auto valueMatch = [&value, &m]() {
                        if (auto mergeMember = TypeQueryVisitor::as<const MemberElement>(m.get())) {
                            assert(mergeMember);

                            auto mergeKey = TypeQueryVisitor::as<const StringElement>(mergeMember->get().key());
                            assert(mergeKey);

                            return std::find_if(
                                value.get().begin(), value.get().end(), [mergeKey](const std::unique_ptr<IElement>& e) {
                                    if (auto valueMember = TypeQueryVisitor::as<const MemberElement>(e.get())) {
                                        auto valueKey
                                            = TypeQueryVisitor::as<const StringElement>(valueMember->get().key());
                                        assert(valueKey);

                                        return mergeKey->get() == valueKey->get();
                                    }

                                    if (auto valueSelect = TypeQueryVisitor::as<const SelectElement>(e.get())) {
                                        return valueSelect->get().end()
                                            != std::find_if(valueSelect->get().begin(),
                                                   valueSelect->get().end(),
                                                   [mergeKey](const std::unique_ptr<OptionElement>& option) {
                                                       return option->get().end()
                                                           != std::find_if(option->get().begin(),
                                                                  option->get().end(),
                                                                  [mergeKey](const std::unique_ptr<IElement>& optEl) {
                                                                      if (auto optElMember
                                                                          = TypeQueryVisitor::as<const MemberElement>(
                                                                              optEl.get())) {
                                                                          auto optElMemberKey = TypeQueryVisitor::as<
                                                                              const StringElement>(
                                                                              optElMember->get().key());
                                                                          assert(optElMemberKey);
                                                                          return optElMemberKey->get()
                                                                              == mergeKey->get();
                                                                      }
                                                                      return false;
                                                                  });
                                                   });
                                    }

                                    return false;
                                });
                        } else if (auto mergeRef = TypeQueryVisitor::as<const RefElement>(m.get())) {
                            return std::find_if(
                                value.get().begin(), value.get().end(), [mergeRef](const std::unique_ptr<IElement>& e) {
                                    if (auto valueRef = TypeQueryVisitor::as<const RefElement>(e.get()))
                                        return mergeRef->get() == valueRef->get();
                                    else
                                        return false;
                                });
                        } else {
                            return value.get().end();
                        }
                    }();

                    if (valueMatch == value.get().end()) {
                        value.get().push_back(clone(*m));
                    } else {
                        value.get().insert(value.get().erase(valueMatch), clone(*m));
                    }
                }
            }
        }
    };

    template <typename T>
    struct ValueMerge<T, true> {

        void operator()(T& value, const T& merge) const
        {
            if (!merge.empty() && !value.empty())
                std::transform(merge.get().begin(),
                    merge.get().end(),
                    std::back_inserter(value.get()),
                    [](const std::unique_ptr<IElement>& e) { return clone(*e); });
        }
    };

    template <typename T>
    struct ElementMerge {
        void operator()(IElement& target, const IElement& append) const noexcept
        {
            assert(TypeQueryVisitor::as<const T>(&target));
            assert(TypeQueryVisitor::as<const T>(&append));

            InfoMerge<SkipMetaKeywords>{}(target.meta(), append.meta());
            InfoMerge<SkipNothing>{}(target.attributes(), append.attributes());

            if (!append.empty())
                ValueMerge<T>{}(static_cast<T&>(target), static_cast<const T&>(append));
        }
    };

    struct SkipEnumerations {
        bool operator()(const std::string& key) const noexcept
        {
            return (key == "enumerations");
        }
    };

    struct TypeEqual {
        const IElement& rhs;
        template <typename ElementT>
        bool operator()(const ElementT& lhs) const noexcept
        {
            return rhs == lhs;
        }
    };

    template <>
    struct ElementMerge<EnumElement> {
        void operator()(IElement& target, const IElement& append) const noexcept
        {
            assert(TypeQueryVisitor::as<const EnumElement>(&target));
            assert(TypeQueryVisitor::as<const EnumElement>(&append));

            InfoMerge<SkipMetaKeywords>{}(target.meta(), append.meta());
            InfoMerge<SkipEnumerations>{}(target.attributes(), append.attributes());

            auto target_enums_it = target.attributes().find("enumerations");
            auto append_enums_it = append.attributes().find("enumerations");

            if (append_enums_it != append.attributes().end()) {
                auto append_enums = TypeQueryVisitor::as<const ArrayElement>(append_enums_it->second.get());
                assert(append_enums);

                assert(!append_enums->empty());
                if (!append_enums->get().empty()) {
                    if (target_enums_it == target.attributes().end()) {
                        target.attributes().set("enumerations", clone(*append_enums));
                    } else {
                        auto target_enums = TypeQueryVisitor::as<ArrayElement>(target_enums_it->second.get());
                        assert(target_enums);

                        for (const auto& append_enum : append_enums->get()) {
                            auto it = std::find_if( //
                                target_enums->get().begin(),
                                target_enums->get().end(),
                                [&append_enum](const std::unique_ptr<IElement>& target_enum) {
                                    return visit(*target_enum, TypeEqual{ *append_enum });
                                });
                            if (target_enums->get().end() != it) {
                                target_enums->get().erase(it);
                            }
                            target_enums->get().push_back(clone(*append_enum));
                        }
                    }
                }
            }

            if (!append.empty())
                ValueMerge<EnumElement>{}(static_cast<EnumElement&>(target), static_cast<const EnumElement&>(append));
        }
    };

    class ElementMerger
    {
        std::unique_ptr<IElement> result;
        TypeQueryVisitor::ElementType base;

    public:
        ElementMerger() : result(nullptr), base(TypeQueryVisitor::Unknown) {}

        template <typename E>
        void operator()(const E& e)
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
                    ElementMerge<StringElement>{}(*result, *e);
                    return;

                case TypeQueryVisitor::Number:
                    ElementMerge<NumberElement>{}(*result, *e);
                    return;

                case TypeQueryVisitor::Boolean:
                    ElementMerge<BooleanElement>{}(*result, *e);
                    return;

                case TypeQueryVisitor::Array:
                    ElementMerge<ArrayElement>{}(*result, *e);
                    return;

                case TypeQueryVisitor::Object:
                    ElementMerge<ObjectElement>{}(*result, *e);
                    return;

                case TypeQueryVisitor::Enum:
                    ElementMerge<EnumElement>{}(*result, *e);
                    return;

                case TypeQueryVisitor::Ref:
                    ElementMerge<RefElement>{}(*result, *e);
                    return;

                case TypeQueryVisitor::Member:
                case TypeQueryVisitor::Extend:
                case TypeQueryVisitor::Null:
                    throw LogicError("Unappropriate kind of element to merging");
                default:
                    throw LogicError("Element has no implemented merging");
            }
        }

        operator std::unique_ptr<IElement>()
        {
            return std::move(result);
        }
    };
}

Extend::Extend() : elements_() {}

Extend& Extend::operator=(Extend other)
{
    swap(*this, other);
    return *this;
}

Extend::Extend(Extend&& other) : Extend()
{
    swap(*this, other);
}

Extend::Extend(const Extend& other) : elements_()
{
    elements_.reserve(other.elements_.size());
    std::transform(other.elements_.begin(),
        other.elements_.end(),
        std::back_inserter(elements_),
        [](const value_type& el) -> std::unique_ptr<IElement> {
            assert(el);
            return el->clone();
        });
}

Extend::iterator Extend::insert(Extend::iterator it, std::unique_ptr<IElement> el)
{
    assert(it >= begin());
    assert(it <= end());
    assert(el);

    if (end() - begin() > 0) {
        if (typeid(decltype(*el)) != typeid(decltype(*elements_.front()))) {
            throw LogicError("ExtendElement must be composed from Elements of same type");
        }
    }

    return elements_.insert(it, std::move(el));
}

Extend::iterator Extend::erase(Extend::iterator b, Extend::iterator e)
{
    return elements_.erase(b, e);
}

std::unique_ptr<IElement> Extend::merge() const
{
    return std::for_each(begin(), end(), ElementMerger());
}

bool dsd::operator==(const Extend& lhs, const Extend& rhs) noexcept
{
    return lhs.size() == rhs.size()
        && std::equal( //
               lhs.begin(),
               lhs.end(),
               rhs.begin(),
               [](const Extend::value_type& l, const Extend::value_type& r) {
                   assert(l);
                   assert(r);
                   return *l == *r;
               });
}

bool dsd::operator!=(const Extend& lhs, const Extend& rhs) noexcept
{
    return !(lhs == rhs);
}
