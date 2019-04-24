//
//  refract/dsd/Traits.h
//  librefract
//
//  Created by Thomas Jandecka on 23/08/17.
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_DSD_TRAITS_H
#define REFRACT_DSD_TRAITS_H

#include <iterator>
#include <type_traits>

namespace refract
{
    namespace dsd
    {
        template <typename T, typename = decltype(std::declval<T>().begin())>
        std::true_type supports_begin_test(const T&);
        std::false_type supports_begin_test(...);
        template <typename T>
        using supports_begin = decltype(supports_begin_test(std::declval<T>()));

        template <typename T, typename = decltype(std::declval<T>().end())>
        std::true_type supports_end_test(const T&);
        std::false_type supports_end_test(...);
        template <typename T>
        using supports_end = decltype(supports_end_test(std::declval<T>()));

        template <typename T,
            typename = decltype(
                std::declval<T>().insert(std::declval<typename T::iterator>(), std::declval<typename T::value_type>()))>
        std::true_type supports_insert_test(const T&);
        std::false_type supports_insert_test(...);
        template <typename T>
        using supports_insert = decltype(supports_insert_test(std::declval<T>()));

        template <typename T,
            typename = decltype(
                std::declval<T>().erase(std::declval<typename T::iterator>(), std::declval<typename T::iterator>()))>
        std::true_type supports_erase_test(const T&);
        std::false_type supports_erase_test(...);
        template <typename T>
        using supports_erase = decltype(supports_erase_test(std::declval<T>()));

        template <typename T>
        using supports_size = std::integral_constant<bool, supports_begin<T>::value && supports_end<T>::value>;

        template <typename T>
        using supports_empty = std::integral_constant<bool, supports_size<T>::value>;

        template <typename T>
        using supports_clear = std::integral_constant<bool, supports_erase<T>::value>;

        template <typename T>
        using supports_push_back = std::integral_constant<bool, supports_insert<T>::value && supports_end<T>::value>;

        template <typename T, typename = decltype(std::declval<T>().merge())>
        std::true_type supports_merge_test(const T&);
        std::false_type supports_merge_test(...);
        template <typename T>
        using supports_merge = decltype(supports_merge_test(std::declval<T>()));

        template <typename T, typename = decltype(std::declval<T>().key())>
        std::true_type supports_key_test(const T&);
        std::false_type supports_key_test(...);
        template <typename T>
        using supports_key = decltype(supports_key_test(std::declval<T>()));

        template <typename T, typename = decltype(std::declval<T>().value())>
        std::true_type supports_value_test(const T&);
        std::false_type supports_value_test(...);
        template <typename T>
        using supports_value = decltype(supports_value_test(std::declval<T>()));

        template <typename T>
        using is_iterable = std::integral_constant<bool, supports_begin<T>::value && supports_end<T>::value>;

        template <typename T>
        using is_pair = std::integral_constant<bool, supports_key<T>::value && supports_value<T>::value>;

        ///
        /// CRTP implementing a common interface to classes holding an STL container
        ///
        /// Specifically, the following are defined on the Child
        ///     * STL conforming aliases to common types, i.e. value_type
        ///     * Convenience methods above the container, if possible
        ///
        /// Convenience methods access the container via calls to these methods on Child:
        ///     * begin()
        ///     * end()
        ///     * begin() const
        ///     * end() const
        ///     * erase(const_iterator, const_iterator)
        ///     * insert(const_iterator, value_type)
        ///
        /// Not all of these methods need to be implemented by Child. In fact, none have to: `container_traits` will
        /// only implement convenience methods that are dependent on the provided subset of this list.
        ///
        /// @tparam Child       type this inherits from (see CRTP pattern)
        /// @tparam Container   type of the STL container held by Child
        ///
        template <typename Child, typename Container>
        class container_traits
        {
            Child& self() noexcept
            {
                return *static_cast<Child*>(this);
            }

            const Child& self() const noexcept
            {
                return *static_cast<const Child*>(this);
            }

        public:
            using container_type = Container;

        public: // STL-like container
            using const_iterator = typename container_type::const_iterator;
            using const_pointer = typename container_type::const_pointer;
            using const_reference = typename container_type::const_reference;
            using difference_type = typename container_type::difference_type;
            using iterator = typename container_type::iterator;
            using size_type = typename container_type::size_type;
            using value_type = typename container_type::value_type;

        public:
            ///
            /// Remove a specific member from collection
            ///
            /// Defined iff the Child declares `erase(const_iterator, const_iterator)`
            ///
            /// @param data  collection to be modified
            /// @param it    iterator to the member to be removed
            ///
            /// @return iterator following the member removed
            ///
            iterator erase(iterator it)
            {
                return self().erase(it, std::next(it));
            }

            ///
            /// Query the size of the collection
            ///
            /// Defined iff the Child declares both
            ///     `begin() const` and
            ///     `end() const`
            ///
            /// @return size of collection
            ///
            size_type size() const noexcept
            {
                return self().end() - self().begin();
            }

            ///
            /// Query whether the collection is empty
            ///
            /// Defined iff the Child declares both
            ///     `begin() const` and
            ///     `end() const`
            ///
            /// @return true iff the collection doesn't have children
            ///
            bool empty() const noexcept
            {
                return size() == 0;
            }

            ///
            /// Remove all members from collection
            ///
            /// Defined iff the Child declares all of
            ///     `begin() const`,
            ///     `end() const`, and
            ///     `erase(const_iterator, const_iterator)`,
            ///
            void clear()
            {
                self().erase(self().begin(), self().end());
            }

            ///
            /// Add member to end of collection
            ///
            /// Defined iff the Child declares both
            ///     `end() const`, and
            ///     `insert(const_iterator, value_type)`,
            ///
            /// @param el    value to be consumed
            ///
            void push_back(value_type el)
            {
                self().insert(self().end(), std::move(el));
            }
        };
    }
}

#endif
