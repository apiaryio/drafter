//
//  ElementComparator.h
//  drafter
//
//  Created by Jiri Kratochvil on 28/02/18.
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//
#ifndef DRAFTER_ELEMENTCOMPARATOR_H
#define DRAFTER_ELEMENTCOMPARATOR_H

#include "refract/InfoElements.h"
#include "refract/Utils.h"
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <set>

namespace drafter
{

    namespace detail
    {
        struct SortedRef {
            using InfoRef = std::vector<std::reference_wrapper<const refract::InfoElements::value_type> >;

            template <typename IgnorePredicate>
            InfoRef operator()(const refract::InfoElements& infoElements, IgnorePredicate ignore) const
            {
                InfoRef res;

                std::transform(infoElements.begin(),
                    infoElements.end(),
                    std::back_inserter(res),
                    [](const refract::InfoElements::value_type& info) { return std::cref(info); });

                res.erase(std::remove_if(res.begin(),
                              res.end(),
                              [&ignore](const InfoRef::value_type& entry) //
                              {                                           //
                                  return ignore(entry.get().first);       //
                              }),
                    res.end());

                std::sort(res.begin(), res.end(), [](const InfoRef::value_type& l, const InfoRef::value_type& r) {
                    return l.get().first < r.get().first;
                });

                return res;
            }
        };

        struct IgnoreNone {
            bool operator()(const std::string&) const noexcept
            {
                return false;
            }
        };

        struct IgnoreKeys {
            const std::set<std::string> keys;

            explicit IgnoreKeys(const std::set<std::string>&& keys) : keys(std::move(keys)) {}
            IgnoreKeys() {}

            bool operator()(const std::string& key) const noexcept
            {
                return keys.end() != keys.find(key);
            }
        };

        template <typename IgnorePredicate>
        class InfoElementsComparator
        {
            IgnorePredicate ignore;

        public:
            template <typename = typename std::enable_if< //
                          std::is_default_constructible<IgnorePredicate>::value>::type>
            InfoElementsComparator() : ignore{}
            {
            }

            explicit InfoElementsComparator(IgnorePredicate ignoreKey) : ignore(std::move(ignoreKey)) {}

            bool operator()(const refract::InfoElements& rhs, const refract::InfoElements& lhs) const
            {
                const auto l = SortedRef()(lhs, ignore);
                const auto r = SortedRef()(rhs, ignore);

                return l.size() == r.size()
                    && std::equal(l.begin(),
                           l.end(),
                           r.begin(),
                           [](const SortedRef::InfoRef::value_type& l, const SortedRef::InfoRef::value_type& r) {
                               return (l.get().first == r.get().first)
                                   && (*l.get().second.get() == *r.get().second.get());
                           });
            }
        };

        struct IsSourceMap {
            bool operator()(const std::string& key) const noexcept
            {
                return key == "sourceMap";
            }
        };

        template <class IgnoreAttrs, class IgnoreMeta>
        class ElementComparator
        {
            const refract::IElement& rhs;
            const InfoElementsComparator<IgnoreAttrs> ignoreAttrs;
            const InfoElementsComparator<IgnoreMeta> ignoreMeta;

        public:
            explicit ElementComparator(const refract::IElement& rhs_, const IgnoreAttrs& ignoreAttrs, const IgnoreMeta& ignoreMeta) : rhs(rhs_), ignoreAttrs(std::move(ignoreAttrs)), ignoreMeta(std::move(ignoreMeta)) {}

        public:
            template <typename ElementT>
            bool operator()(const ElementT& lhs) const
            {
                return (lhs.empty() == rhs.empty()) && (lhs.element() == rhs.element())
                    && ignoreMeta(rhs.meta(), lhs.meta()) 
                    && ignoreAttrs(rhs.attributes(), lhs.attributes()) 
                    && (lhs.empty() || (lhs.get() == dynamic_cast<const ElementT*>(&rhs)->get()));
            }
        };
    } // namespace detail

    template <typename IgnoreAttrs = detail::IsSourceMap, typename IgnoreMeta = detail::IgnoreNone>
    bool Equal(const refract::IElement& lhs,
        const refract::IElement& rhs,
        IgnoreAttrs&& ignoreAttrs = {},
        IgnoreMeta&& ignoreMeta = {})
    {
        return visit(lhs, detail::ElementComparator<IgnoreAttrs, IgnoreMeta>{ rhs, ignoreAttrs, ignoreMeta });
    }
} // namespace drafter

#endif // ifndef DRAFTER_ELEMENTCOMPARATOR_H
