//
//  apib2apie/LiteralToApie.h
//  apib2apie
//
//  Created by Thomas Jandecka on 02/30/20.
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//

#ifndef APIB2APIE_LITERAL_TO_APIE_H
#define APIB2APIE_LITERAL_TO_APIE_H

#include <memory>
#include <string>

#include "../NodeInfo.h"
#include "../refract/Element.h"

namespace apib2apie
{
    std::unique_ptr<refract::StringElement> LiteralToApie(const drafter::NodeInfo<std::string>&);
}

#endif
