//
//  refract/SerializeSo.h
//  librefract
//
//  Created by Thomas Jandecka on 11/07/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_SERIALIZE_H
#define REFRACT_SERIALIZE_H

#include "../utils/so/Value.h"
#include "ElementIfc.h"

namespace refract
{
    namespace serialize
    {
        ///
        /// Translate an API Element tree to the Simple Object format.
        /// @note   JSON and YAML serializers can be used on Simple Objects
        ///
        /// @param el           API Element to be translated
        /// @param sourceMaps   whether to print source maps; source maps on
        ///                     Annotation Elements are always rendered
        ///
        /// @return             Simple Object value representing given tree
        ///
        drafter::utils::so::Value renderSo(const IElement& el, bool sourceMaps);

    } // namespace serialize
} // namespace refract

#endif
