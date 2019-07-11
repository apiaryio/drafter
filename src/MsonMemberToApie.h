//
//  MsonMemberToApie.h
//  drafter
//
//  Created by Thomas Jandecka on 11/07/19.
//  Copyright (c) 2019 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_MSONMEMBERTOAPIE_H
#define DRAFTER_MSONMEMBERTOAPIE_H

#include <MSON.h>
#include <MSONSourcemap.h>
#include <memory>

#include "refract/ElementIfc.h"
#include "NodeInfo.h"

namespace drafter
{
    class ConversionContext;
}

namespace drafter {
    std::unique_ptr<refract::IElement> MsonMemberToApie( //
        const NodeInfo<mson::PropertyMember>& nodeInfo,
        ConversionContext&,
        mson::BaseTypeName,
        mson::BaseTypeName);

    std::unique_ptr<refract::IElement> MsonMemberToApie( //
        const NodeInfo<mson::ValueMember>& nodeInfo,
        ConversionContext&,
        mson::BaseTypeName,
        mson::BaseTypeName);
}

#endif
