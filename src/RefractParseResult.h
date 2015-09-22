//
//  RefractParseResult.h
//  drafter
//
//  Created by Pavan Kumar Sunkara on 25/09/15.
//  Copyright (c) 2015 Apiary. All rights reserved.
//

#ifndef DRAFTER_REFRACTPARSERESULT_H
#define DRAFTER_REFRACTPARSERESULT_H

#include "Serialize.h"
#include "SectionProcessor.h"

namespace drafter {

    refract::IElement* ParseResultToRefract(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint);
}

#endif
