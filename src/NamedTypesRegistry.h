//
//  NamedTypesRegistry.h
//  drafter
//
//  Created by Jiri Kratochvil on 19-01-2016
//  Copyright (c) 2016 Apiary Inc. All rights reserved.
//
#ifndef DRAFTER_NAMEDTYPESREGISRTY_H
#define DRAFTER_NAMEDTYPESREGISRTY_H


#include "Blueprint.h"

namespace refract {
    class Registry;
}

namespace drafter {

    template <typename T>
    struct NodeInfo;

    class ConversionContext;

    void RegisterNamedTypes(const NodeInfo<snowcrash::Elements>& elements, ConversionContext& context);

}
#endif // #ifndef DRAFTER_NAMEDTYPESREGISRTY_H
