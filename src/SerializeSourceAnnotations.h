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

namespace snowcrash { struct Report; }

namespace drafter {
    sos::Object WrapSourceAnnotations(const snowcrash::Report& report, const snowcrash::SourceMap<snowcrash::Blueprint>& sourceMap);
}

#endif
