//
//  RefractElementFactory.h
//  drafter
//
//  Created by Jiri Kratochvil on 04-03-2016
//  Copyright (c) 2016 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_REFRACTELEMENTFACTORY_H
#define DRAFTER_REFRACTELEMENTFACTORY_H

#include <string>
#include <MSON.h>

namespace refract {
    struct IElement;
}

namespace drafter {

    struct RefractElementFactory
    {
        virtual ~RefractElementFactory() {}
        virtual refract::IElement* Create(const std::string& literal, bool) = 0;
    };

    RefractElementFactory& FactoryFromType(const mson::BaseTypeName typeName);

}

#endif /* #ifndef DRAFTER_REFRACTELEMENTFACTORY_H */
