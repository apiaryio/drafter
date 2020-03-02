//
//  apib2apie/SourceMapToApie.h
//  apib2apie
//
//  Created by Thomas Jandecka on 02/30/20.
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//
//  Note @tjanc: moved from RefractSourceMap.h

#ifndef APIB2APIE_SOURCE_MAP_TO_APIE_H
#define APIB2APIE_SOURCE_MAP_TO_APIE_H

#include <ByteBuffer.h>

#include "../SerializeKey.h"
#include "../refract/Element.h"

namespace drafter
{
    class ConversionContext;
}

namespace apib2apie
{
    std::unique_ptr<refract::ArrayElement> SourceMapToApie( //
        const mdp::CharactersRangeSet&);

    std::unique_ptr<refract::ArrayElement> SourceMapToApieWithColumnLineInfo( //
        const mdp::CharactersRangeSet&,
        const drafter::ConversionContext&);

    template <typename T, typename Element>
    void AttachSourceMap(Element& element, const T& nodeInfo)
    {
        if (!nodeInfo.sourceMap->sourceMap.empty()) {
            element.attributes().set( //
                drafter::SerializeKey::SourceMap,
                SourceMapToApie(nodeInfo.sourceMap->sourceMap));
        }
    }
}

#endif
