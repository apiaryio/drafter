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

    /**
     * NOTE: depracated as entry point for serialization
     *
     * Since version 2.0.0 you should use everywhere `WrapResult()` function instead of this one
     *
     * This function now works just as AST serialization wrapper
     * additionaly there is changed function interface 
     */
    sos::Object WrapBlueprint(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint, const bool expandMSON);
}

#endif
