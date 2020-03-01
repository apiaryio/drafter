//
//  apib2apie/CopyToApie.h
//  apib2apie
//
//  Created by Thomas Jandecka on 02/21/2020
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//

#ifndef APIB2APIE_COPY_TO_APIE_H
#define APIB2APIE_COPY_TO_APIE_H

#include <memory>
#include <Blueprint.h>
#include "../NodeInfo.h"

namespace refract
{
    class IElement;
}

namespace apib2apie
{
    std::unique_ptr<refract::IElement> CopyToApie(const drafter::NodeInfo<std::string>&);
}

#endif
