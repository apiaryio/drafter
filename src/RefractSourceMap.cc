#include "RefractSourceMap.h"

using namespace refract;

namespace
{
    std::unique_ptr<IElement> CharacterRangeToRefract(const mdp::CharactersRange& sourceMap)
    {
        return make_element<ArrayElement>( //
            from_primitive(static_cast<double>(sourceMap.location)),
            from_primitive(static_cast<double>(sourceMap.length)));
    }
}

std::unique_ptr<IElement> drafter::SourceMapToRefract(const mdp::CharactersRangeSet& sourceMap)
{
    auto sourceMapElement = make_element<ArrayElement>();
    sourceMapElement->element(SerializeKey::SourceMap);

    std::transform( //
        sourceMap.begin(),
        sourceMap.end(),
        std::back_inserter(sourceMapElement->get()),
        CharacterRangeToRefract);

    return make_element<ArrayElement>(std::move(sourceMapElement));
}

std::unique_ptr<StringElement> drafter::LiteralToRefract(
    const NodeInfo<std::string>& literal, ConversionContext& context)
{
    std::pair<bool, dsd::String> parsed = LiteralTo<dsd::String>(*literal.node);

    auto element = parsed.first ? //
        make_element<StringElement>(parsed.second) :
        make_empty<StringElement>();

    AttachSourceMap(*element, literal);

    return element;
}
