//
//  SerializeResult.h
//  drafter
//
//  Created by Jiri Kratochvil on 27-02-2015
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_SERIALIZE_RESULT_H
#define DRAFTER_SERIALIZE_RESULT_H

#include "Serialize.h"

#include "SectionParserData.h" // required by BlueprintParserOptions

namespace snowcrash {
    struct Blueprint;
    template <typename T> struct ParseResult; 
}

namespace drafter {

    sos::Object WrapResult(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint, const snowcrash::BlueprintParserOptions options);
}

#endif // #ifndef DRAFTER_SERIALIZE_RESULT_H
