#include "RefractSourceMap.h"

refract::IElement* drafter::BytesRangeToRefract(const mdp::BytesRange& bytesRange)
{
    refract::ArrayElement* range = new refract::ArrayElement;

    range->push_back(refract::IElement::Create(bytesRange.location));
    range->push_back(refract::IElement::Create(bytesRange.length));

    return range;
}
