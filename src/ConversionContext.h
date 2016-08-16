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
#include "snowcrash.h"

namespace drafter {

    struct WrapperOptions;

    class ConversionContext {
        refract::Registry registry;

    public:
        const WrapperOptions& options;
        std::vector<snowcrash::Warning> warnings;

        inline refract::Registry& GetNamedTypesRegistry() { return registry; }

        ConversionContext(const WrapperOptions& options)
        : options(options) {}

        void warn(const snowcrash::Warning& warning);
    };

}
#endif // #ifndef DRAFTER_CONVERSIONCONTEXT_H
