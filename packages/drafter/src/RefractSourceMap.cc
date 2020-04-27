#include "RefractSourceMap.h"
#include "ConversionContext.h"

using namespace refract;

namespace
{
    std::unique_ptr<IElement> CharacterRangeToRefract(const mdp::CharactersRange& sourceMap)
    {
        return make_element<ArrayElement>( //
            from_primitive(sourceMap.location),
            from_primitive(sourceMap.length));
    }

} // namespace

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

std::unique_ptr<IElement> drafter::SourceMapToRefractWithColumnLineInfo(
    const mdp::CharactersRangeSet& sourceMap, const ConversionContext& context)
{
    auto sourceMapElement = make_element<ArrayElement>();
    sourceMapElement->element(SerializeKey::SourceMap);

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
