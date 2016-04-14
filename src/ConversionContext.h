//
//  ConversionContext.h
//  drafter
//
//  Created by Jiri Kratochvil on 06-04-2016
//  Copyright (c) 2016 Apiary Inc. All rights reserved.
//
#ifndef DRAFTER_CONVERSIONCONTEXT_H
#define DRAFTER_CONVERSIONCONTEXT_H

#include "refract/Registry.h"

namespace drafter {

    class ConversionContext {
        refract::Registry registry;

    public:
        bool generateSourceMap;
        bool expandMSON;

        inline refract::Registry& GetNamedTypesRegistry() { return registry; }

        ConversionContext(const WrapperOptions& options)
        {
            generateSourceMap = options.generateSourceMap;
            expandMSON = options.expandMSON;
        }
    };

}
#endif // #ifndef DRAFTER_CONVERSIONCONTEXT_H
