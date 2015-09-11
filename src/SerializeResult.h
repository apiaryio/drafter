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

#include "SectionParserData.h" // required by BlueprintParserOptions

namespace snowcrash {
    struct Blueprint;
    template <typename T> struct ParseResult;
}

namespace drafter {

    sos::Object WrapParseResult(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint, const snowcrash::BlueprintParserOptions options);
    sos::Object WrapResult(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint, const snowcrash::BlueprintParserOptions options, const ASTType astType);
}

#endif // #ifndef DRAFTER_SERIALIZERESULT_H
