//
//  RefractAPI.h
//  drafter
//
//  Created by Jiri Kratochvil on 31/07/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_REFRACTAST_H
#define DRAFTER_REFRACTAST_H

#include "Serialize.h"

namespace drafter {

    refract::IElement* DataStructureToRefract(const snowcrash::DataStructure& dataStructure);
    refract::IElement* BlueprintToRefract(const snowcrash::Blueprint& blueprint);
}

#endif // #ifndef DRAFTER_REFRACTAST_H
