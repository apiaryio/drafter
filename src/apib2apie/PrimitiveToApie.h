//
//  apib2apie/PrimitiveToApie.h
//  apib2apie
//
//  Created by Thomas Jandecka on 02/30/20.
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//

#ifndef APIB2APIE_PRIMITIVE_TO_APIE_H
#define APIB2APIE_PRIMITIVE_TO_APIE_H

#include "../NodeInfo.h"
#include "../refract/Element.h"

#include "SourceMapToApie.h"

namespace apib2apie
{
    template <typename T>
    std::unique_ptr<refract::PrimitiveElement<T> > PrimitiveToApie(const drafter::NodeInfo<T>& primitive)
    {
        assert(primitive.node);
        auto element = refract::from_primitive(*primitive.node);
        AttachSourceMap(*element, primitive);
        return std::move(element);
    }

    template <typename T>
    std::unique_ptr<refract::PrimitiveElement<T> > PrimitiveToApie(
        std::string name, const drafter::NodeInfo<T>& primitive)
    {
        assert(primitive.node);
        auto element = refract::from_primitive_t(name, *primitive.node);
        AttachSourceMap(*element, primitive);
        return std::move(element);
    }
}

#endif
