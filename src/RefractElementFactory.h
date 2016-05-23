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
#include "MSON.h"

namespace refract {
    struct IElement;
}

namespace drafter {

    /**
     * This interface is used while conversion from legacy AST
     * into refract tree.
     *
     * For usage see test/test-ElementFactoryTest.cc
     *
     * WARNING:
     * There is little bit different behavior for primitives and for complex elements.
     *
     * For any case:
     * - if `literal` is empty - return __empty__ element of required type
     *
     * For primitives (string|number|bool):
     * - `sample` eValue - return typed element with __value__  set via `ToLiteral<>` conversion
     * - `sample` eSample - return typed element with __attributes.samples__ set via `ToLiteral<>` conversion
     * - `sample` eElement - return typed element with set __element name__ to `literal`
     *
     * For complex elements (object|array|enum):
     * - `sample` eSample - return StringElement with name __generic__ with value set to `literal`
     * - `sample` eValue|eElement - return return typed element with set __element name__ to `literal`
     *
     * This inconsistent behavior is determined by rules of converting legacy AST to Refract.
     *
     * Code will be deprecated (refactoring or remove) after remove legacy AST.
     */

    enum FactoryCreateMethod {
        eValue,
        eSample,
        eElement,
    };

    struct RefractElementFactory
    {
        virtual ~RefractElementFactory() {}
        virtual refract::IElement* Create(const std::string& literal, FactoryCreateMethod type) const = 0;
    };

    /**
     * Do not change const return type!!!
     * it is due to thread safety
     */
    const RefractElementFactory& FactoryFromType(const mson::BaseTypeName typeName);

}

#endif /* #ifndef DRAFTER_REFRACTELEMENTFACTORY_H */
