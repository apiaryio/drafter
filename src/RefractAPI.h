//
//  RefractAPI.h
//  drafter
//
//  Created by Jiri Kratochvil on 31/07/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_REFRACTAST_H
#define DRAFTER_REFRACTAST_H

#include "Serialize.h"

namespace snowcrash {
    struct SourceAnnotation;
}

namespace drafter {

    refract::IElement* AnnotationToRefract(const snowcrash::SourceAnnotation& annotation, const std::string& key);

    refract::IElement* DataStructureToRefract(const NodeInfo<snowcrash::DataStructure>& dataStructure, bool expand = false);
    refract::IElement* BlueprintToRefract(const NodeInfo<snowcrash::Blueprint>& blueprint);
}

#endif // #ifndef DRAFTER_REFRACTAST_H
