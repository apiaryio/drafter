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
#include <deque>

namespace drafter
{
    template <typename ElementT, typename = std::enable_if<std::is_base_of<refract::IElement, ElementT>::value> >
    constexpr bool is_primitive = false;

    template <>
    constexpr bool is_primitive<refract::BooleanElement> = true;

    template <>
    constexpr bool is_primitive<refract::NumberElement> = true;

    template <>
    constexpr bool is_primitive<refract::StringElement> = true;

    // template <>
    // constexpr bool is_primitive<refract::NullElement> = true;

    template <typename ElementT>
    struct content_source_map_type {
        using type = snowcrash::SourceMap<typename ElementT::ValueType>;
    };

    template <typename T>
    struct stored_type {
        using type = typename std::conditional<is_primitive<T>,
            std::string,                                    // for primitive values, we will hold data as string
            std::deque<std::unique_ptr<refract::IElement> > // for complex types, we will hold elements
            >::type;
    };

    template <typename ElementT,
        typename StoredT = typename stored_type<ElementT>::type,
        typename SourceMapT = typename content_source_map_type<ElementT>::type>
    struct ElementInfo final {
        StoredT value;
        SourceMapT sourceMap;

        ElementInfo() = default;
        ElementInfo(StoredT v, SourceMapT m) : value(std::move(v)), sourceMap(std::move(m)) {}

        ElementInfo(const ElementInfo&) = delete;
        ElementInfo(ElementInfo&&) = default;

        ElementInfo& operator=(const ElementInfo&) = delete;
        ElementInfo& operator=(ElementInfo&&) = default;

        ~ElementInfo() = default;
    };

    struct DescriptionInfo {
        std::string description;
        snowcrash::SourceMap<std::string> sourceMap;
    };

    template <typename T>
    using ElementInfoContainer = std::deque<ElementInfo<T> >;

    using DescriptionInfoContainer = std::deque<DescriptionInfo>;

    template <typename T>
    struct ElementData {
        // typedef typename std::conditional<std::is_same<ValueType, refract::RefractElements>::value
        //        || std::is_same<ValueType, refract::IElement*>::value, // check for primitive values
        //    std::false_type,
        //    std::true_type>::type IsPrimitive;

        // This is required because snowcrash internal stuctures holds data
        // for primitive types as "string" for complex types as "element array"
        // it will be converted into apropriated element type once all required data are colected

        ElementInfoContainer<T> values;
        ElementInfoContainer<T> defaults;
        ElementInfoContainer<T> samples;
        ElementInfoContainer<T> enumerations;

        DescriptionInfoContainer descriptions;
    };

} // namespace drafter

#endif // #ifndef DRAFTER_ELEMENTDATA_H
