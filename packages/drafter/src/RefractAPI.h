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

namespace snowcrash
{
    struct SourceAnnotation;
}

namespace drafter
{

    class ConversionContext;

    std::unique_ptr<refract::IElement> AnnotationToRefract(
        const snowcrash::SourceAnnotation& annotation, const std::string& key, ConversionContext& context);

    std::unique_ptr<refract::IElement> DataStructureToRefract(
        const NodeInfo<snowcrash::DataStructure>& dataStructure, ConversionContext& context);
    std::unique_ptr<refract::IElement> BlueprintToRefract(
        const NodeInfo<snowcrash::Blueprint>& blueprint, ConversionContext& context);
}

#endif // #ifndef DRAFTER_REFRACTAST_H
