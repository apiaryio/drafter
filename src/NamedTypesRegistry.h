//
//  NamedTypesRegistry.h
//  drafter
//
//  Created by Jiri Kratochvil on 19-01-2016
//  Copyright (c) 2016 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_NAMDTYPESREGISTRY_H
#define DRAFTER_NAMDTYPESREGISTRY_H

#include "Blueprint.h"

namespace refract {
    struct Registry;
}

namespace drafter {

    template <typename T>
    struct NodeInfo;

    refract::Registry& GetNamedTypesRegistry();
    void RegisterNamedTypes(const drafter::NodeInfo<snowcrash::Elements>& elements);

} // ns drafter
#endif // #ifndef DRAFTER_NAMDTYPESREGISTRY_H
