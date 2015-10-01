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

        WrappingContext(const ASTType astType) : astType(astType) {}
    };

    sos::Object WrapBlueprint(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint, const WrappingContext& context, bool expand = false);
}

#endif
