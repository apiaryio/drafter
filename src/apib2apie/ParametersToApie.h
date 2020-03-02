//
//  apib2apie/ParametersToApie.h
//  apib2apie
//
//  Created by Thomas Jandecka on 02/21/20.
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//

#ifndef APIB2APIE_PARAMETERS_TO_APIE_H
#define APIB2APIE_PARAMETERS_TO_APIE_H

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
    std::unique_ptr<refract::IElement> ParametersToApie( //
        const drafter::NodeInfo<snowcrash::Parameters>&, //
        drafter::ConversionContext&);
}

#endif
