//
//  RefractSourceMap.h
//  drafter
//
//  Created by Jiri Kratochvil on 31/07/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_REFRACTSOURCEMAP_H
#define DRAFTER_REFRACTSOURCEMAP_H

#include "Serialize.h"

namespace drafter
{

    class ConversionContext;

    std::unique_ptr<refract::IElement> SourceMapToRefract(const mdp::CharactersRangeSet& sourceMap);
    std::unique_ptr<refract::IElement> SourceMapToRefractWithColumnLineInfo(
        const mdp::CharactersRangeSet& sourceMap, const ConversionContext& context);

    template <typename T>
    void AttachSourceMap(refract::IElement& element, const T& nodeInfo)
    {
        if (!nodeInfo.sourceMap->sourceMap.empty()) {
            element.attributes().set(SerializeKey::SourceMap, SourceMapToRefract(nodeInfo.sourceMap->sourceMap));
        }
    }

    template <typename T>
    std::unique_ptr<refract::IElement> PrimitiveToRefract(const NodeInfo<T>& primitive)
    {
        auto element = refract::from_primitive(*primitive.node);
        AttachSourceMap(*element, primitive);
        return std::move(element);
    }

    std::unique_ptr<refract::StringElement> LiteralToRefract(
        const NodeInfo<std::string>& literal, ConversionContext& context);
}

#endif // #ifndef DRAFTER_REFRACTSOURCEMAP_H
