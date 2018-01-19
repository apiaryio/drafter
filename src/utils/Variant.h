//
//  utils/Variant.h
//  librefract
//
//  Created by Thomas Jandecka on 17/01/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_UTILS_VARIANT_H
#define DRAFTER_UTILS_VARIANT_H

#include <cassert>
#include <type_traits>
#include <new>
#include "Utils.h"

namespace drafter
{
    namespace utils
    {
        template <typename T>
        struct in_place_type {
            in_place_type() = default;
        };

        struct monostate {
        };

        class bad_variant_access : public std::exception
        {
        };

        template <typename Visitor, typename Which, typename Storage, typename... Args>
        decltype(auto) vtable_entry(Storage&& storage, Visitor&& visitor, Args&&... args)
        {
            using storage_value = typename std::remove_reference<Storage>::type;

            using which_const_adjusted = typename std::conditional< //
                std::is_const<storage_value>::value,
                const Which,
                Which>::type;

            using which_type = typename std::conditional< //
                std::is_reference<Storage>::value,
                which_const_adjusted&,
                which_const_adjusted>::type;

            return visitor(reinterpret_cast<which_type>(storage), std::forward<Args>(args)...);
        }

        template <typename Which, typename... Variants>
        constexpr size_t index_of_variant = index_of<bare<Which>, Variants...>();

        template <typename... Variants>
        class variant
        {
            static_assert(
                sizeof...(Variants) > 0, "variant must contain at least one type; use variant<monostate> instead");

        private:
            using storage_type =
                typename std::aligned_storage<maximum(sizeof(Variants)...), maximum(alignof(Variants)...)>::type;

        private:
            size_t which_ = 0;
            storage_type storage_;

        private:
            struct dispatcher {
                template <typename Visitor, typename Storage, typename... Args>
                decltype(auto) operator()(Storage&& storage, size_t which, Visitor&& visitor, Args&&... args)
                {
                    using result_type = typename std::common_type< //
                        decltype(vtable_entry<Visitor, Variants, Storage>(std::forward<Storage>(storage),
                            std::forward<Visitor>(visitor),
                            std::forward<Args>(args)...))...>::type;

                    using vtable_entry_type = result_type (*)(Storage&&, Visitor&&, Args && ...);

                    static vtable_entry_type vtable[] = { &vtable_entry<Visitor, Variants, Storage, Args...>... };

                    assert(which >= 0 && which < sizeof...(Variants));

                    return (*vtable[which])( //
                        std::forward<Storage>(storage),
                        std::forward<Visitor>(visitor),
                        std::forward<Args>(args)...);
                }
            };

            template <typename Which, typename... Args>
            auto& fill_storage(Args&&... args)
            {
                using T = bare<Which>;
                return *new (&storage_) T(std::forward<Args>(args)...);
            }

        public:
            template <typename Visitor, typename... Args>
            decltype(auto) do_visit(Visitor&& visitor, Args&&... args) const
            {
                return dispatcher()(storage_, which_, std::forward<Visitor>(visitor), std::forward<Args>(args)...);
            }

            template <typename Visitor, typename... Args>
            decltype(auto) do_visit(Visitor&& visitor, Args&&... args)
            {
                return dispatcher()(storage_, which_, std::forward<Visitor>(visitor), std::forward<Args>(args)...);
            }

            constexpr size_t index() const noexcept
            {
                return which_;
            }

            template <size_t I, typename... Args>
            auto& emplace(Args&&... args)
            {
                static_assert(I >= 0 && I < sizeof...(Variants));
                using Which = typename type_at<I, Variants...>::type;

                do_visit(destructor{});

                which_ = I;
                return this->template fill_storage<Which>(std::forward<Args>(args)...);
            }

            template <size_t I>
            auto& get() &
            {
                static_assert(I >= 0 && I < sizeof...(Variants));
                using Which = typename type_at<I, Variants...>::type;

                if (which_ != I)
                    throw bad_variant_access{};
                return reinterpret_cast<Which&>(storage_);
            }

            template <size_t I>
            const auto& get() const &
            {
                static_assert(I >= 0 && I < sizeof...(Variants));
                using Which = typename type_at<I, Variants...>::type;

                if (which_ != I)
                    throw bad_variant_access{};
                return reinterpret_cast<const Which&>(storage_);
            }

            void swap(variant& other)
            {
                if (which_ == other.which_) {
                    do_visit(
                        [](auto& lhs, auto& rhs) { //
                            swap(lhs, rhs.template get<index_of_variant<decltype(lhs), Variants...> >());
                        },
                        other);
                } else {
                    using std::swap;
                    swap(which_, other.which_);
                    swap(storage_, other.storage_);
                }
            }

        private:
            struct destructor {
                template <typename T>
                void operator()(T& t)
                {
                    t.~T();
                }
            };

        public:
            template <char = 0>
            explicit variant(
                typename std::enable_if<std::is_default_constructible<typename head<Variants...>::type>::value,
                    char>::type
                = 0) noexcept(std::is_nothrow_default_constructible<typename head<Variants...>::type>::value)
                : which_(0)
            {
                this->fill_storage<typename head<Variants...>::type>();
            }

            template <typename Which,
                typename = typename std::enable_if<is_head_in_tail<bare<Which>, Variants...> >::type>
            constexpr variant(Which&& v) noexcept(std::is_nothrow_constructible<bare<Which>, Which>::value)
                : which_(index_of_variant<Which, Variants...>)
            {
                fill_storage<Which>(std::forward<Which>(v));
            }

            variant(const variant& other) : which_(other.which_)
            {
                other.do_visit([this](const auto& o) { fill_storage<bare<decltype(o)> >(o); });
            }

            variant(variant&& other) : which_(other.which_)
            {
                other.do_visit([this](auto& o) { fill_storage<bare<decltype(o)> >(std::move(o)); });
            }

            template <typename Which, typename... Args>
            explicit variant(in_place_type<Which> ip_, Args&&... args) : which_(index_of_variant<Which, Variants...>)
            {
                fill_storage<Which>(std::forward<Args>(args)...);
            }

            variant& operator=(const variant& rhs)
            {
                if (which_ == rhs.which_) {
                    do_visit(
                        [](auto& lhs, const auto& rhs) -> void { //
                            lhs = rhs.template get<index_of_variant<decltype(lhs), Variants...> >();
                        },
                        rhs);
                } else {
                    rhs.do_visit(
                        [](const auto& rhs, auto& lhs) -> void { //
                            lhs.template emplace<index_of_variant<decltype(rhs), Variants...> >(rhs);
                        },
                        *this);
                }

                return *this;
            }

            variant& operator=(variant&& rhs)
            {
                if (which_ == rhs.which_) {
                    do_visit(
                        [](auto& lhs, auto&& rhs) -> void { //
                            lhs = std::move(rhs.template get<index_of_variant<decltype(lhs), Variants...> >());
                        },
                        std::move(rhs));
                } else {
                    rhs.do_visit(
                        [](auto& rhs, auto& lhs) -> void { //
                            lhs.template emplace<index_of_variant<decltype(rhs), Variants...> >(std::move(rhs));
                        },
                        *this);
                }

                return *this;
            }

            template <typename Which,
                typename = typename std::enable_if<is_head_in_tail<bare<Which>, Variants...> >::type>
            variant& operator=(const Which& rhs)
            {
                emplace<index_of_variant<Which, Variants...> >(std::move(rhs));
                return *this;
            }

            ~variant()
            {
                do_visit(destructor{});
            }
        };

        template <size_t I, typename... Variants>
        auto& get(variant<Variants...>& v)
        {
            return v.template get<I>();
        }

        template <typename Which, typename... Variants>
        auto& get(variant<Variants...>& v)
        {
            return v.template get<index_of<Which, Variants...>()>();
        }

        template <typename Which, typename... Variants>
        bool holds_alternative(const variant<Variants...>& v) noexcept
        {
            return v.index() == index_of<Which, Variants...>();
        }

        template <size_t I, typename... Variants>
        bool holds_alternative(const variant<Variants...>& v) noexcept
        {
            return v.index() == I;
        }

        template <size_t I, typename... Variants>
        auto get_if(variant<Variants...>& v) -> decltype(&get<I>(v))
        {
            return holds_alternative<I>(v) ? &get<I>(v) : nullptr;
        }

        template <typename Which, typename... Variants>
        Which* get_if(variant<Variants...>& v)
        {
            return holds_alternative<Which>(v) ? (&get<Which>(v)) : nullptr;
        }

        template <typename Variant, typename Visitor, typename... Args>
        decltype(auto) visit(Variant&& v, Visitor&& visitor, Args&&... args)
        {
            return v.do_visit(std::forward<Visitor>(visitor), std::forward<Args>(args)...);
        }
    }
}

#endif
