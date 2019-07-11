//
//  MsonOneOfSectionToApie.h
//  drafter
//
//  Created by Thomas Jandecka on 11/07/19.
//  Copyright (c) 2019 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_MSONONEOFSECTIONTOAPIE_H
#define DRAFTER_MSONONEOFSECTIONTOAPIE_H

#include <MSON.h>
#include <MSONSourcemap.h>
#include <memory>

#include "refract/ElementIfc.h"
#include "NodeInfo.h"

namespace drafter
{
    class ConversionContext;

    std::unique_ptr<refract::IElement> MsonOneOfSectionToApie( //
        const mson::OneOf&,
        const snowcrash::SourceMap<mson::OneOf>*,
        ConversionContext&);
}

#endif
