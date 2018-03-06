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
    using namespace refract;
    struct InfoElementsComparator {
        bool operator()(const InfoElements& rhs, const InfoElements& lhs)
        {

            const std::set<std::string> ignore = { "sourceMap" };
            std::set<std::string> keys;

            if (std::find_if(lhs.begin(),
                    lhs.end(),
                    [&keys, &rhs, &ignore](const auto& info) {
                        auto key = info.first;
                        if (ignore.find(key) != ignore.end()) {
                            return false;
                        }
                        keys.insert(key);

                        auto rInfo = rhs.find(key);
                        if (rInfo == rhs.end()) {
                            return true;
                        }

                        return !(*rInfo->second == *info.second);
                    })
                != lhs.end()) {
                return false;
            }

            if (std::find_if(rhs.begin(),
                    rhs.end(),
                    [&keys, &lhs, &ignore](const auto& info) {
                        auto key = info.first;
                        if (ignore.find(key) != ignore.end()) {
                            return false;
                        }

                        if (keys.find(key) != ignore.end()) {
                            return false;
                        }

                        return true;
                    })
                != rhs.end()) {
                return false;
            }

            return true;
        }
    };

    struct ElementComparator {
        const IElement& rhs;

        template <typename ElementT>
        bool operator()(const ElementT& lhs)
        {
            if (auto rhsptr = dynamic_cast<const ElementT*>(&rhs)) {
                return (lhs.empty() == rhs.empty()) && (lhs.element() == rhs.element())
                    && (InfoElementsComparator{}(rhs.attributes(), lhs.attributes()))
                    && (InfoElementsComparator{}(rhs.meta(), lhs.meta()))
                    && (lhs.empty() || (lhs.get() == rhsptr->get()));
            } else
                return false;
        }
    };
} // namespace drafter

#endif // ifndef DRAFTER_ELEMENTCOMPARATOR_H
