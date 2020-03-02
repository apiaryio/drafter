//
//  SourceMapToApie.cc
//  apib2apie
//
//  Created by Thomas Jandecka on 02/30/20.
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//
//  Note @tjanc: moved from RefractSourceMap.cc

#include "SourceMapToApie.h"

#include "../SerializeKey.h"
#include "ConversionContext.h"

using namespace refract;
using namespace drafter;

namespace
{
    std::unique_ptr<IElement> CharacterRangeToApie(const mdp::CharactersRange& sourceMap)
    {
        return make_element<ArrayElement>( //
            from_primitive(sourceMap.location),
            from_primitive(sourceMap.length));
    }

} // namespace

std::unique_ptr<ArrayElement> apib2apie::SourceMapToApie(const mdp::CharactersRangeSet& sourceMap)
{
    auto sourceMapElement = make_element_t<ArrayElement>(SerializeKey::SourceMap);

    std::transform( //
        sourceMap.begin(),
        sourceMap.end(),
        std::back_inserter(sourceMapElement->get()),
        CharacterRangeToApie);

    return make_element<ArrayElement>(std::move(sourceMapElement));
}

std::unique_ptr<ArrayElement> apib2apie::SourceMapToApieWithColumnLineInfo(
    const mdp::CharactersRangeSet& sourceMap, const ConversionContext& context)
{
    auto sourceMapElement = make_element_t<ArrayElement>(SerializeKey::SourceMap);

    std::transform( //
        sourceMap.begin(),
        sourceMap.end(),
        std::back_inserter(sourceMapElement->get()),
        [&context](const mdp::CharactersRange& sourceMap) {
            auto position = GetLineFromMap(context.newlineIndices(), sourceMap);

            auto location = make_element<NumberElement>(sourceMap.location);
            location->attributes().set("line", from_primitive(position.fromLine));
            location->attributes().set("column", from_primitive(position.fromColumn));

            auto length = make_element<NumberElement>(sourceMap.length);
            length->attributes().set("line", from_primitive(position.toLine));
            length->attributes().set("column", from_primitive(position.toColumn));

            return make_element<ArrayElement>(std::move(location), std::move(length));
        });

    return make_element<ArrayElement>(std::move(sourceMapElement));
}
