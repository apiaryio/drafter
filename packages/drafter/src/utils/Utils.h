//
//  utils/Utils.h
//  librefract
//
//  Created by Thomas Jandecka on 20/01/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_UTILS_H
#define DRAFTER_UTILS_H

#include <type_traits>
#include <iterator>
#include <tuple>

namespace drafter
{
    namespace utils
    {
        constexpr size_t maximum(size_t a)
        {
            return a;
        }

        template <typename... TT>
        constexpr size_t maximum(size_t a, size_t b, const TT&... cs)
        {
            return maximum(a > b ? a : b, cs...);
        }

        template <bool... TT>
        struct any_of;

        template <bool T, bool... TT>
        struct any_of<T, TT...> {
            static constexpr bool value = T || any_of<TT...>::value;
        };

        template <>
        struct any_of<> {
            static constexpr bool value = false;
        };

        template <bool... TT>
        struct all_of;

        template <bool T, bool... TT>
        struct all_of<T, TT...> {
            static constexpr bool value = T && all_of<TT...>::value;
        };

        template <>
        struct all_of<> {
            static constexpr bool value = true;
        };

        template <typename T, typename... TT>
        struct is_head_in_tail {
            static constexpr bool value = any_of<std::is_same<T, TT>::value...>::value;
        };

        template <typename Arg, typename... Args>
        struct head {
            using type = Arg;
        };

        template <typename T>
        struct bare {
            using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
        };

        template <typename T, typename... Args>
        struct index_of;

        template <typename T, typename Arg, typename... Args>
        struct index_of<T, Arg, Args...> {
            static constexpr size_t value = std::is_same<T, Arg>::value ? 0 : 1 + index_of<T, Args...>::value;
        };

        template <typename T>
        struct index_of<T> {
            static constexpr size_t value = 0;
        };

        template <size_t I, typename... Args>
        using type_at = typename std::tuple_element<I, std::tuple<Args...> >;

        template <typename T, typename = void>
        struct is_iterator {
            static constexpr bool value = false;
        };

        template <typename T>
        struct is_iterator<T,
            typename std::enable_if<
                !std::is_same<typename std::iterator_traits<T>::iterator_category, void>::value>::type> {
            static constexpr bool value = true;
        };
    }
}

#endif
