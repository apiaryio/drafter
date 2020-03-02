//
//  LiteralToApie.cc
//  apib2apie
//
//  Created by Thomas Jandecka on 02/30/20.
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//
//  Note @tjanc: moved from RefractSourceMap.cc

#include "LiteralToApie.h"

#include "../Serialize.h"
#include "../NodeInfo.h"
#include "SourceMapToApie.h"

using namespace refract;
using namespace drafter;
using namespace apib2apie;

std::unique_ptr<StringElement> apib2apie::LiteralToApie(const NodeInfo<std::string>& literal)
{
    std::pair<bool, dsd::String> parsed = LiteralTo<dsd::String>(*literal.node);

    auto element = parsed.first ? //
        make_element<StringElement>(parsed.second) :
        make_empty<StringElement>();

    AttachSourceMap(*element, literal);

    return element;
}
