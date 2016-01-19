//
//  NamedTypesRegistry.h
//  drafter
//
//  Created by Jiri Kratochvil on 16-01-2016.
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
    class NodeInfo;

    refract::Registry& GetNamedTypesRegistry();
    void RegisterNamedTypes(const NodeInfo<snowcrash::Elements>& elements);

}
#endif // #ifndef DRAFTER_NAMEDTYPESREGISRTY_H
