//
//  SerializeSourceAnnotations.h
//  drafter
//
//  Created by Jiri Kratochvil on 27-02-2015
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_SERIALIZE_SOURCE_ANNOTATIONS_H
#define DRAFTER_SERIALIZE_SOURCE_ANNOTATIONS_H

#include "Serialize.h"

namespace snowcrash { 
    struct Blueprint;
    template <typename T> struct ParseResult; 
}

namespace drafter {
    
    sos::Object WrapResult(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint);

}

#endif
