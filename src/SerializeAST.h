//
//  SerializeAST.h
//  drafter
//
//  Created by Pavan Kumar Sunkara on 18/01/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_SERIALIZEAST_H
#define DRAFTER_SERIALIZEAST_H

#include "Serialize.h"

namespace drafter {

    sos::Object WrapBlueprint(const snowcrash::Blueprint& blueprint, const ASTType astType, bool expand = false);
}

#endif
