//
//  SerializeResult.h
//  drafter
//
//  Created by Jiri Kratochvil on 27-02-2015
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_SERIALIZERESULT_H
#define DRAFTER_SERIALIZERESULT_H

#include "Serialize.h"

#include "SectionProcessor.h"

namespace drafter {

    sos::Object WrapAnnotation(const snowcrash::SourceAnnotation& annotation);
    sos::Object WrapParseResult(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint, const snowcrash::BlueprintParserOptions options);
    sos::Object WrapResult(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint, const snowcrash::BlueprintParserOptions options, const ASTType astType);
}

#endif // #ifndef DRAFTER_SERIALIZERESULT_H
