//
//  ElementInfoUtils.h
//  drafter
//
//  Created by Jiri Kratochvil on  20/02/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "ElementData.h"

#ifndef DRAFTER_MERGEELEMENTINFO_H
#define DRAFTER_MERGEELEMENTINFO_H

namespace drafter
{

    template <typename T, bool IsPrimitive = is_primitive<T>()>
    struct Merge;

    template <typename T>
    struct Merge<T, true> {
        using Info = ElementInfo<T>;
        using Container = std::deque<Info>;

        Info operator()(Container container) const
        {
            if (container.empty()) {
                return ElementInfo<T>();
            } else {
                auto result = std::move(container.front());
                container.pop_front();
                return result;
            }
        }
    };

    template <typename T>
    struct Merge<T, false> {
        using Info = ElementInfo<T>;
        using Container = std::deque<Info>;
        using StoredType = typename stored_type<T>::type;
        using SourceMap = typename content_source_map_type<T>::type;

        Info operator()(Container container) const
        {
            Info result{};

            for (auto& info : container) {
                std::move(info.value.begin(), info.value.end(), std::back_inserter(result.value));
                result.sourceMap.sourceMap.append(info.sourceMap.sourceMap);
            }

            return result;
        }
    };

    template <typename T>
    ElementInfoContainer<T> CloneElementInfoContainer(const ElementInfoContainer<T>& infoContainer)
    {
        ElementInfoContainer<T> copy;
        std::copy(infoContainer.begin(), infoContainer.end(), std::back_inserter(copy));
        return copy;
    }
}

#endif //#ifndef DRAFTER_MERGEELEMENTINFO_H
