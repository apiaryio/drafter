//
//  apib2apie/CollectionToApie.h
//  apib2apie
//
//  Created by Thomas Jandecka on 02/21/20.
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//
//  Note @tjanc: moved from RefractAPI.cc, changed to run without buffering
//                                         node info collection

#ifndef APIB2APIE_COLLECTION_TO_APIE_H
#define APIB2APIE_COLLECTION_TO_APIE_H

#include <string>

#include "../NodeInfo.h"
#include "../refract/Element.h"
#include "../ConversionContext.h"

namespace apib2apie
{
    template <typename Element, typename Collection, typename SM, typename F, typename... Args>
    void AccumulateToApie(Element& out, const Collection& collection, const SM* sm, F& f, Args&&... args)
    {
        auto& content = out.get();

        if (sm) {
            auto smIt = sm->collection.begin();
            const auto smEnd = sm->collection.end();
            for (const auto& node : collection) {
                if (smIt == smEnd) {
                    content.push_back(f(node, nullptr, std::forward<Args>(args)...));
                } else {
                    content.push_back(f(node, &*smIt, std::forward<Args>(args)...));
                    ++smIt;
                }
            }
        } else {
            for (const auto& node : collection) {
                content.push_back(f(node, nullptr, std::forward<Args>(args)...));
            }
        }
    }

    template <typename T, typename C, typename F, typename... Args>
    std::unique_ptr<T> CollectionToApie( //
        std::string key,
        const drafter::NodeInfo<C>& collection, //
        F& f,                                   //
        Args&&... args)
    {
        assert(collection.node);

        auto element = refract::make_element_t<T>(std::move(key));

        AccumulateToApie( //
            *element,
            *collection.node,
            collection.sourceMap,
            f,
            std::forward<Args>(args)...);

        return element;
    }

    template <typename T, typename C, typename F, typename... Args>
    std::unique_ptr<T> CollectionToApie(        //
        const drafter::NodeInfo<C>& collection, //
        F& f,                                   //
        Args&&... args)
    {
        assert(collection.node);

        auto element = refract::make_element<T>();

        AccumulateToApie( //
            *element,
            *collection.node,
            collection.sourceMap,
            f,
            std::forward<Args>(args)...);

        return element;
    }
}

#endif
