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
#include "SectionProcessor.h" // from Blueprint

namespace drafter {

    struct WrappingContext {
        const ASTType astType;
        const bool expandMSON;
        const bool exportSourceMap;

        WrappingContext(const ASTType astType, const bool expandMSON, const bool exportSourceMap) 
            : astType(astType), expandMSON(expandMSON), exportSourceMap(exportSourceMap) {
        }
    };

    sos::Object WrapBlueprint(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint, const WrappingContext& context);
}

#endif
