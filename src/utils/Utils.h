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
#include <tuple>

namespace drafter
{
    namespace utils
    {
        template <typename T>
        constexpr const T& maximum(const T& a)
        {
            return a;
        }

        template <typename T, typename... TT>
        constexpr const T& maximum(const T& a, const T& b, const TT&... cs)
        {
            return maximum(a > b ? a : b, cs...);
        }

        template <char Dummy = 0>
        constexpr bool either()
        {
            return false;
        }

        template <typename T, typename... TT>
        constexpr bool either(const T& t, const TT&... ts)
        {
            return t || either(ts...);
        }

        template <char Dummy = 0>
        constexpr bool all_of()
        {
            return false;
        }

        template <typename T, typename... TT>
        constexpr bool all_of(const T& t, const TT&... ts)
        {
            return t || either(ts...);
        }

        template <typename T, typename... TT>
        constexpr bool is_head_in_tail = either(std::is_same<T, TT>::value...);

        template <typename Arg, typename... Args>
        struct head {
            using type = Arg;
        };

        template <typename T>
        using bare = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

        template <typename T>
        constexpr size_t index_of()
        {
            return 0;
        }

        template <typename T, typename Arg, typename... Args>
        constexpr size_t index_of(typename std::enable_if<std::is_same<T, Arg>::value, char>::type = 0)
        {
            return 0;
        }

        template <typename T, typename Arg, typename... Args>
        constexpr size_t index_of(typename std::enable_if<!std::is_same<T, Arg>::value, char>::type = 0)
        {
            return 1 + index_of<T, Args...>();
        }

        template <size_t I, typename... Args>
        using type_at = typename std::tuple_element<I, std::tuple<Args...> >;
    }
}

#endif
