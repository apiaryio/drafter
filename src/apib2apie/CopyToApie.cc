//
//  CopyToApie.cc
//  apib2apie
//
//  Created by Thomas Jandecka on 02/25/20.
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//
//  Note @tjanc: moved from RefractAPI.cc

#include "CopyToApie.h"

#include "PrimitiveToApie.h"
#include "../SerializeKey.h"

using namespace drafter;
using namespace refract;

std::unique_ptr<StringElement> apib2apie::CopyToApie(const NodeInfo<std::string>& copy)
{
    if (copy.node->empty())
        return nullptr;

    auto element = PrimitiveToApie(SerializeKey::Copy, copy);

    return element;
}
