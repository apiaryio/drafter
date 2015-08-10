//
//  RefractDataStructure.h
//  drafter
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_REFRACTDATASTRUCTURE_H
#define DRAFTER_REFRACTDATASTRUCTURE_H

#include "Serialize.h"

namespace refract {
    struct IElement;
    struct Registry;
}

namespace drafter {

    refract::IElement* DataStructureToRefract(const snowcrash::DataStructure& dataStructure);
    refract::IElement* ExpandRefract(refract::IElement*, const refract::Registry&);

    sos::Object SerializeRefract(refract::IElement*);
}

#endif // #ifndef DRAFTER_REFRACTDATASTRUCTURE_H

