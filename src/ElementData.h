//
//  ElementData.h
//  drafter
//
//  Created by Jiri Kratochvil on 08/08/17.
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//
#ifndef DRAFTER_ELEMENTDATA_H
#define DRAFTER_ELEMENTDATA_H

#include "RefractSourceMap.h"

namespace drafter
{

    template <typename T>
    struct IsPrimitive {
        using ValueType = typename T::ValueType;
        using type = std::integral_constant<bool,
            !std::is_same<ValueType, refract::RefractElements>::value
                && !std::is_same<ValueType, refract::IElement*>::value>;
    };

    template <typename T>
    struct ElementData {
        using ElementType = T;

        using ValueType = typename ElementType::ValueType;
        using ValueSourceMapType = snowcrash::SourceMap<ValueType>;


        //typedef typename std::conditional<std::is_same<ValueType, refract::RefractElements>::value
        //        || std::is_same<ValueType, refract::IElement*>::value, // check for primitive values
        //    std::false_type,
        //    std::true_type>::type IsPrimitive;

        // This is required because snowcrash internal stuctures holds data
        // for primitive types as "string" for complex types as "element array"
        // it will be converted into apropriated element type once all required data are colected

        using StoredType = typename std::conditional<IsPrimitive<T>::type::value,
            std::string,             // for primitive values, we will hold data as string
            refract::RefractElements // for complex types, we will hold elements
            >::type;

        using ElementInfo = std::tuple<StoredType, ValueSourceMapType>;                      // [value, sourceMap]
        using DescriptionInfo = std::tuple<std::string, snowcrash::SourceMap<std::string> >; // [description, sourceMap]

        using ElementInfoContainer = std::vector<ElementInfo>;
        using DescriptionInfoContainer = std::vector<DescriptionInfo>;

        ElementInfoContainer values;
        ElementInfoContainer defaults;
        ElementInfoContainer samples;
        ElementInfoContainer enumerations;

        DescriptionInfoContainer descriptions;
    };

} // namespace drafter

#endif // #ifndef DRAFTER_ELEMENTDATA_H
