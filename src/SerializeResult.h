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
#include "SectionParserData.h"

namespace snowcrash {
    struct SourceAnnotation;
}

namespace refract {
    struct IElement;
}

namespace drafter {

    class ConversionContext;

    sos::Object WrapAnnotation(const snowcrash::SourceAnnotation& annotation);
    sos::Object WrapResult(snowcrash::ParseResult<snowcrash::Blueprint>& blueprint, const WrapperOptions& options);

    refract::IElement* WrapParseResultRefract(snowcrash::ParseResult<snowcrash::Blueprint>& blueprint,
                                              ConversionContext& context);
}

#endif // #ifndef DRAFTER_SERIALIZERESULT_H
