//
//  apib2apie/PayloadToApie.h
//  apib2apie
//
//  Created by Thomas Jandecka on 02/21/2020
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//

#ifndef APIB2APIE_PAYLOAD_TO_APIE_H
#define APIB2APIE_PAYLOAD_TO_APIE_H

#include <memory>
#include <Blueprint.h>
#include "../NodeInfo.h"

namespace drafter
{
    class ConversionContext;
}

namespace refract
{
    class IElement;
}

namespace apib2apie
{
    std::unique_ptr<refract::IElement> PayloadToApie( //
        const drafter::NodeInfo<snowcrash::Payload>&,
        const drafter::NodeInfo<snowcrash::Action>&,
        drafter::ConversionContext&);
}

#endif
