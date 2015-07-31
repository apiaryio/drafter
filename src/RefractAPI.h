
//
//  RefractAPI.h
//  drafter
//
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_REFRACT_AST_H
#define DRAFTER_REFRACT_AST_H

#include "Serialize.h"

namespace drafter {

    sos::Object SerializeToRefract(const snowcrash::Blueprint& blueprint);


}

#endif // #ifndef DRAFTER_REFRACT_AST_H
