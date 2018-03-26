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
#include <set>
#include <algorithm>

namespace drafter
{

    namespace detail
    {

        struct SortedRef {
            using InfoRef = std::vector<std::reference_wrapper<const refract::InfoElements::value_type> >;

            InfoRef operator()(const refract::InfoElements& infoElements, const std::set<std::string>& ignore) const
            {
                InfoRef res;

                std::transform(infoElements.begin(), infoElements.end(), std::back_inserter(res), [](auto& info) {
                    return std::cref(info);
                });

                res.erase(std::remove_if(res.begin(),
                              res.end(),
                              [&ignore](const auto& info) { return ignore.find(info.get().first) != ignore.end(); }),
                    res.end());

                std::sort(res.begin(), res.end(), [](auto l, auto r) { return l.get().first < r.get().first; });

                return res;
            }
        };
    };

    using namespace refract;
    struct InfoElementsComparator {
        bool operator()(const InfoElements& rhs, const InfoElements& lhs) const
        {
            const auto l = detail::SortedRef()(lhs, { "sourceMap" });
            const auto r = detail::SortedRef()(rhs, { "sourceMap" });

            return std::equal(l.begin(), l.end(), r.begin(), r.end(), [](const auto& l, const auto& r) {
                return l.get().first == r.get().first && *l.get().second.get() == *r.get().second.get();
            });
        }
    };

    struct ElementComparator {
        const IElement& rhs;

        template <typename ElementT>
        bool operator()(const ElementT& lhs) const
        {
            return (lhs.empty() == rhs.empty()) && (lhs.element() == rhs.element())
                && (InfoElementsComparator{}(rhs.attributes(), lhs.attributes()))
                && (InfoElementsComparator{}(rhs.meta(), lhs.meta()))
                && (lhs.empty() || (lhs.get() == dynamic_cast<const ElementT*>(&rhs)->get()));
        }
    };
} // namespace drafter

#endif // ifndef DRAFTER_ELEMENTCOMPARATOR_H
