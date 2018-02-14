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
            constexpr in_place_type() noexcept = default;
        };

        struct monostate {
        };

        class bad_variant_access : public std::exception
        {
        };

        namespace detail
        {
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

            template <typename Which, typename Storage, typename... Args>
            auto& fill_storage(Storage&& storage, Args&&... args)
            {
                using T = typename bare<Which>::type;
                return *new (&storage) T(std::forward<Args>(args)...);
            }

            template <typename Which, typename... Variants>
            using index = index_of<typename bare<Which>::type, Variants...>;
        }

        template <typename... Variants>
        class variant
        {
            static_assert(
                sizeof...(Variants) > 0, "variant must contain at least one type; use variant<monostate> instead");

        private:
            static constexpr size_t storage_size = maximum(sizeof(Variants)...);
            static constexpr size_t storage_align = maximum(alignof(Variants)...);

            using storage_type = typename std::aligned_storage<storage_size, storage_align>::type;

        private:
            size_t which_ = 0;
            storage_type storage_;

        private:
            struct dispatcher {
                template <typename Visitor, typename Storage, typename... Args>
                decltype(auto) operator()(Storage&& storage, size_t which, Visitor&& visitor, Args&&... args)
                {
                    using result_type = typename std::common_type< //
                        decltype(detail::vtable_entry<Visitor, Variants, Storage>(std::forward<Storage>(storage),
                            std::forward<Visitor>(visitor),
                            std::forward<Args>(args)...))...>::type;

                    using vtable_entry_type = result_type (*)(Storage&&, Visitor&&, Args && ...);

                    static vtable_entry_type vtable[]
                        = { &detail::vtable_entry<Visitor, Variants, Storage, Args...>... };

                    assert(which >= 0 && which < sizeof...(Variants));

                    return (*vtable[which])( //
                        std::forward<Storage>(storage),
                        std::forward<Visitor>(visitor),
                        std::forward<Args>(args)...);
                }
            };

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
                static_assert(I >= 0 && I < sizeof...(Variants), "invalid index");
                using Which = typename type_at<I, Variants...>::type;

                do_visit(destructor{});

                which_ = I;
                return detail::fill_storage<Which>(storage_, std::forward<Args>(args)...);
            }

            template <size_t I>
            auto& get() &
            {
                static_assert(I >= 0 && I < sizeof...(Variants), "invalid index");
                using Which = typename type_at<I, Variants...>::type;

                if (which_ != I)
                    throw bad_variant_access{};
                return reinterpret_cast<Which&>(storage_);
            }

            template <size_t I>
            const auto& get() const &
            {
                static_assert(I >= 0 && I < sizeof...(Variants), "invalid index");
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
                            swap(lhs, rhs.template get<detail::index<decltype(lhs), Variants...>::value>());
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
                void operator()(T& t) const
                {
                    t.~T();
                }
            };

            struct move_into_storage {
                template <typename Other, typename Storage>
                void operator()(Other& other, Storage& storage) const
                {
                    detail::fill_storage<typename bare<Other>::type>(storage, std::move(other));
                }
            };

            struct copy_into_storage {
                template <typename Other, typename Storage>
                void operator()(const Other& other, Storage& storage) const
                {
                    detail::fill_storage<typename bare<Other>::type>(storage, other);
                }
            };

        public:
            template <typename
                = typename std::enable_if<std::is_default_constructible<typename head<Variants...>::type>::value>::type>
            variant() noexcept(std::is_nothrow_default_constructible<typename head<Variants...>::type>::value)
                : which_(0)
            {
                using Which = typename head<Variants...>::type;
                detail::fill_storage<Which>(storage_);
            }

            template <typename Which,
                typename
                = typename std::enable_if<is_head_in_tail<typename bare<Which>::type, Variants...>::value>::type>
            constexpr variant(Which&& v)
                noexcept(std::is_nothrow_constructible<typename bare<Which>::type, Which>::value)
                : which_(detail::index<Which, Variants...>::value)
            {
                detail::fill_storage<Which>(storage_, std::forward<Which>(v));
            }

            variant(const variant& other) : which_(other.which_)
            {
                other.do_visit(copy_into_storage{}, storage_);
            }

            variant(variant&& other) : which_(other.which_)
            {
                other.do_visit(move_into_storage{}, storage_);
            }

            template <typename Which, typename... Args>
            explicit variant(in_place_type<Which> ip_, Args&&... args)
                : which_(detail::index<Which, Variants...>::value)
            {
                detail::fill_storage<Which>(storage_, std::forward<Args>(args)...);
            }

            variant& operator=(const variant& rhs)
            {
                if (which_ == rhs.which_) {
                    do_visit(
                        [](auto& lhs, const auto& rhs) -> void { //
                            lhs = rhs.template get<detail::index<decltype(lhs), Variants...>::value>();
                        },
                        rhs);
                } else {
                    rhs.do_visit(
                        [](const auto& rhs, auto& lhs) -> void { //
                            lhs.template emplace<detail::index<decltype(rhs), Variants...>::value>(rhs);
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
                            lhs = std::move(rhs.template get<detail::index<decltype(lhs), Variants...>::value>());
                        },
                        std::move(rhs));
                } else {
                    rhs.do_visit(
                        [](auto& rhs, auto& lhs) -> void { //
                            lhs.template emplace<detail::index<decltype(rhs), Variants...>::value>(std::move(rhs));
                        },
                        *this);
                }

                return *this;
            }

            template <typename Which,
                typename
                = typename std::enable_if<is_head_in_tail<typename bare<Which>::type, Variants...>::value>::type>
            variant& operator=(Which&& rhs)
            {
                emplace<detail::index<Which, Variants...>::value>(std::forward<Which>(rhs));
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

        template <size_t I, typename... Variants>
        const auto& get(const variant<Variants...>& v)
        {
            return v.template get<I>();
        }

        template <typename Which, typename... Variants>
        auto& get(variant<Variants...>& v)
        {
            constexpr size_t I = index_of<Which, Variants...>::value;
            return v.template get<I>();
        }

        template <typename Which, typename... Variants>
        const auto& get(const variant<Variants...>& v)
        {
            constexpr size_t I = index_of<Which, Variants...>::value;
            return v.template get<I>();
        }

        template <typename Which, typename... Variants>
        bool holds_alternative(const variant<Variants...>& v) noexcept
        {
            return v.index() == index_of<Which, Variants...>::value;
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

        template <size_t I, typename... Variants>
        auto get_if(const variant<Variants...>& v) -> decltype(&get<I>(v))
        {
            return holds_alternative<I>(v) ? &get<I>(v) : nullptr;
        }

        template <typename Which, typename... Variants>
        Which* get_if(variant<Variants...>& v)
        {
            return holds_alternative<Which>(v) ? (&get<Which>(v)) : nullptr;
        }

        template <typename Which, typename... Variants>
        const Which* get_if(const variant<Variants...>& v)
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
