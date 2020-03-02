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
    void AccumulateToApie(Element& out, const Collection& collection, const SM* sm, F f)
    {
        auto& content = out.get();

        if (sm) {
            auto smIt = sm->collection.begin();
            const auto smEnd = sm->collection.end();
            for (const auto& node : collection) {
                if (smIt == smEnd) {
                    content.push_back(f(node, nullptr));
                } else {
                    content.push_back(f(node, &*smIt));
                    ++smIt;
                }
            }
        } else {
            for (const auto& node : collection) {
                content.push_back(f(node, nullptr));
            }
        }
    }

    template <typename T, typename C, typename F>
    std::unique_ptr<T> CollectionToApie(        //
        const drafter::NodeInfo<C>& collection, //
        F& f,                                   //
        const std::string& key = {})
    {
        assert(collection.node);

        using refract::make_element;
        using refract::make_element_t;

        auto element = key.empty() ? make_element<T>() : make_element_t<T>(std::move(key));

        using NodeInfoT = drafter::NodeInfo<typename C::value_type>;
        using SmPtr = decltype(NodeInfoT::NullSourceMap());

        AccumulateToApie( //
            *element,
            *collection.node,
            collection.sourceMap,
            [&f](const auto& item, const SmPtr sm) { //
                return f(NodeInfoT(&item, sm ? sm : NodeInfoT::NullSourceMap()));
            });

        return element;
    }

    template <typename T, typename C, typename F>
    std::unique_ptr<T> CollectionToApie(        //
        const drafter::NodeInfo<C>& collection, //
        drafter::ConversionContext& context,    //
        F& f,                                   //
        const std::string& key = {})
    {
        assert(collection.node);

        using refract::make_element;
        using refract::make_element_t;

        auto element = key.empty() ? make_element<T>() : make_element_t<T>(std::move(key));

        using NodeInfoT = drafter::NodeInfo<typename C::value_type>;
        using SmPtr = decltype(NodeInfoT::NullSourceMap());

        AccumulateToApie( //
            *element,
            *collection.node,
            collection.sourceMap,
            [&context, &f](const auto& item, const SmPtr sm) { //
                return f(NodeInfoT(&item, sm ? sm : NodeInfoT::NullSourceMap()), context);
            });

        return element;
    }
}

#endif
