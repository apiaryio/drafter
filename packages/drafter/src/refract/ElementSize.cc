//
//  refract/ElementSize.cc
//  librefract
//
//  Created by Thomas Jandecka on 21/03/2019
//  Copyright (c) 2019 Apiary Inc. All rights reserved.
//

#include "ElementSize.h"

#include "Element.h"
#include "ElementUtils.h"
#include "../utils/log/Trivial.h"
#include <algorithm>
#include <numeric>
#include <limits>

using namespace refract;

namespace
{
    const ArrayElement* enumerations(const EnumElement& e)
    {
        auto it = e.attributes().find("enumerations");
        if (it != e.attributes().end())
            if (const ArrayElement* enums = get<const ArrayElement>(it->second.get()))
                return enums;
        return nullptr;
    }

    template <typename It>
    cardinal sizeOfMult(It begin, It end, bool inheritsFixed) noexcept
    {
        using ElementPtr = decltype(*begin);
        return std::accumulate(begin, end, cardinal{ 1 }, [inheritsFixed](cardinal a, const ElementPtr& b) { //
            return a * sizeOf(*b, inheritsFixed);
        });
    }

    template <typename It>
    cardinal sizeOfSum(It begin, It end, bool inheritsFixed) noexcept
    {
        using ElementPtr = decltype(*begin);
        return std::accumulate(begin, end, cardinal::empty(), [inheritsFixed](cardinal a, const ElementPtr& b) { //
            return a + sizeOf(*b, inheritsFixed);
        });
    }

    cardinal wrapNullable(cardinal s, const IElement& e)
    {
        return hasNullableTypeAttr(e) ? (s + cardinal{ 1 }) : s;
    }

    struct SizeOfVisitor {
        bool inheritsFixed;

        template <typename ElementT>
        cardinal operator()(const ElementT& el) const
        {
            return sizeOf(el, inheritsFixed);
        }
    };
}

cardinal refract::sizeOf(const IElement& e, bool inheritsFixed)
{
    return refract::visit(e, SizeOfVisitor{ inheritsFixed });
}

cardinal refract::sizeOf(const ObjectElement& e, bool inheritsFixed)
{
    auto baseSize = cardinal::open();
    const bool isFixed = inheritsFixed || hasFixedTypeAttr(e);
    if (isFixed || hasFixedTypeTypeAttr(e)) {
        if (e.empty())
            baseSize = cardinal{ 1 };
        else
            baseSize = sizeOfMult(e.get().begin(), e.get().end(), isFixed);
    }
    return wrapNullable(baseSize, e);
}

cardinal refract::sizeOf(const MemberElement& e, bool inheritsFixed)
{
    if (e.empty() || !e.get().value())
        return cardinal::empty();
    const auto keySize = isVariable(e) ? cardinal::open() : cardinal{ 1 };
    return wrapNullable(keySize * sizeOf(*e.get().value(), inheritsFixed), e);
}

cardinal refract::sizeOf(const ArrayElement& e, bool inheritsFixed)
{
    auto baseSize = cardinal::open();

    if (inheritsFixed || hasFixedTypeAttr(e))
        if (e.empty())
            baseSize = cardinal{ 1 };
        else
            baseSize = sizeOfMult(e.get().begin(), e.get().end(), true);
    else if (hasFixedTypeTypeAttr(e)) {
        if (e.empty())
            baseSize = cardinal::empty();
        else if (sizeOfSum(e.get().begin(), e.get().end(), false) != cardinal::empty())
            baseSize = cardinal::open();
    }
    return wrapNullable(baseSize, e);
}

cardinal refract::sizeOf(const EnumElement& e, bool inheritsFixed)
{
    const auto* enums = enumerations(e);

    if (!enums || enums->empty())
        return cardinal::empty();

    inheritsFixed = inheritsFixed || hasFixedTypeAttr(e);
    const auto baseSize = sizeOfSum(enums->get().begin(), enums->get().end(), inheritsFixed);

    return wrapNullable(baseSize, e);
}

cardinal refract::sizeOf(const NullElement& e, bool inheritsFixed)
{
    return cardinal{ 1 };
}

cardinal refract::sizeOf(const StringElement& e, bool inheritsFixed)
{
    auto baseSize = cardinal::open();
    if ((definesValue(e) && inheritsFixed) || hasFixedTypeAttr(e))
        baseSize = cardinal{ 1 };
    return wrapNullable(baseSize, e);
}

cardinal refract::sizeOf(const NumberElement& e, bool inheritsFixed)
{
    auto baseSize = cardinal::open();
    if ((definesValue(e) && inheritsFixed) || hasFixedTypeAttr(e))
        baseSize = cardinal{ 1 };
    return wrapNullable(baseSize, e);
}

cardinal refract::sizeOf(const BooleanElement& e, bool inheritsFixed)
{
    auto baseSize = cardinal{ 2 };
    if ((definesValue(e) && inheritsFixed) || hasFixedTypeAttr(e))
        baseSize = cardinal{ 1 };
    return wrapNullable(baseSize, e);
}

cardinal refract::sizeOf(const ExtendElement& e, bool inheritsFixed)
{
    if (e.empty())
        return cardinal::empty();
    if (const auto merged = e.get().merge())
        return sizeOf(*merged, inheritsFixed);
    return cardinal::empty();
}

cardinal refract::sizeOf(const RefElement& e, bool inheritsFixed)
{
    if (const IElement* resolved = resolve(e))
        return sizeOf(*resolved, inheritsFixed);
    LOG(warning) << "ignoring unresolved reference calculating type cardinality";
    return cardinal::empty();
}

cardinal refract::sizeOf(const HolderElement& e, bool inheritsFixed)
{
    if (e.empty() || !e.get().data())
        return cardinal::empty();
    return sizeOf(*e.get().data(), inheritsFixed);
}

cardinal refract::sizeOf(const SelectElement& e, bool inheritsFixed)
{
    if (e.empty())
        return cardinal::empty();
    return sizeOfSum(e.get().begin(), e.get().end(), inheritsFixed);
}

cardinal refract::sizeOf(const OptionElement& e, bool inheritsFixed)
{
    if (e.empty())
        return cardinal::empty();
    return sizeOfMult(e.get().begin(), e.get().end(), inheritsFixed);
}
