//
//  RefractDataStructure.h
//  drafter
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_REFRACTDATASTRUCTURE_H
#define DRAFTER_REFRACTDATASTRUCTURE_H

#include <memory>
#include "refract/ElementIfc.h"
#include "NodeInfo.h"

namespace drafter
{

    class ConversionContext;

    std::unique_ptr<refract::IElement> MSONToRefract(
        const NodeInfo<snowcrash::DataStructure>& dataStructure, ConversionContext& context);
    std::unique_ptr<refract::IElement> ExpandRefract(
        std::unique_ptr<refract::IElement> element, ConversionContext& context);
}

#endif // #ifndef DRAFTER_REFRACTDATASTRUCTURE_H
