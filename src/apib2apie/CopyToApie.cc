//
//  CopyToApie.cc
//  apib2apie
//
//  Created by Thomas Jandecka on 25/02/20.
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//
//  Note @tjanc: moved from RefractAPI.cc

#include "CopyToApie.h"

#include <SourceAnnotation.h>

#include "../RefractDataStructure.h"
#include "../Render.h"
#include "../RefractSourceMap.h"

using namespace drafter;
using namespace refract;

std::unique_ptr<IElement> apib2apie::CopyToApie(const NodeInfo<std::string>& copy)
{
    if (copy.node->empty()) {
        return nullptr;
    }

    auto element = PrimitiveToRefract(copy);
    element->element(SerializeKey::Copy);

    return element;
}
