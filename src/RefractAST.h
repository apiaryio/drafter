//
//  SerializeAST.h
//  drafter
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_REFRACT_AST_H
#define DRAFTER_REFRACT_AST_H

#include "Serialize.h"
#include "refract/Element.h"

namespace snowcrash {
    struct DataStructure;
}


namespace drafter {

    sos::Object DataStructureToRefract(const snowcrash::DataStructure& dataStructure);

}

#endif // #ifndef DRAFTER_REFRACT_AST_H

