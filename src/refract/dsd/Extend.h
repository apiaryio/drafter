//
//  refract/dsd/Extend.h
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_DSD_EXTEND_H
#define REFRACT_DSD_EXTEND_H

#include <memory>
#include <vector>

#include "../ElementIfc.h"
#include "Traits.h"
#include "Utils.h"

namespace refract
{
    namespace dsd
    {
        ///
        /// Data structure definition (DSD) of a Refract Extend Element
        ///
        /// @remark Defined by a sequence of Refract Elements
        ///
        class Extend final : public container_traits<Extend, std::vector<std::unique_ptr<IElement> > >
        {
            container_type elements_;

        public:
            static const char* name; //< sequence of Elements

        public:
            ///
            /// Initialize an empty Extend DSD
            ///
            Extend() = default;

            ///
            /// Initialize an Extend DSD from its member elements
            ///
            /// @param elements     elements to be contained
            ///
            template <typename... Element>
            Extend(std::unique_ptr<Element>... elements) : elements_()
            {
                elements_.reserve(sizeof...(elements));
                utils::move_back(elements_, std::move(elements)...);
            }

            ///
            /// Initialize an Extend DSD from another by consuming its memory representation
            ///
            /// @param other   Extend DSD to be consumed
            ///
            Extend(Extend&&) = default;

            ///
            /// Initialize an Extend DSD from another by cloning its children
            ///
            /// @param other   Extend DSD to be cloned from
            ///
            Extend(const Extend& other);

            ///
            /// Clear children and consume another Extend DSD's memory representation
            ///
            /// @param other   Extend DSD to be consumed
            ///
            Extend& operator=(Extend&&) = default;

            ///
            /// Clear children and clone them from another Extend DSD
            ///
            /// @param other   Extend DSD to be cloned from
            ///
            Extend& operator=(const Extend& rhs)
            {
                Extend a(rhs);
                std::swap(a, *this);
                return *this;
            }

            ~Extend() = default;

        public: // iterators
            auto begin() noexcept
            {
                return elements_.begin();
            }
            auto end() noexcept
            {
                return elements_.end();
            }
            auto begin() const noexcept
            {
                return elements_.begin();
            }
            auto end() const noexcept
            {
                return elements_.end();
            }

        public:
            ///
            /// Add a child Element
            ///
            /// @param it   where the element is to be added
            /// @param el   Element to be consumed
            ///
            /// @return iterator to Element added
            ///
            iterator insert(const_iterator it, std::unique_ptr<IElement> el);

            ///
            /// Remove a subsequence of children
            ///
            /// @param b    iterator to the first Element to be removed
            /// @param e    iterator following the last Element to be removed
            ///
            /// @return iterator following the last child removed
            ///
            iterator erase(const_iterator b, const_iterator e);

            using container_traits<Extend, container_type>::erase;

        public:
            std::unique_ptr<IElement> merge() const;
        };

        bool operator==(const Extend&, const Extend&) noexcept;
        bool operator!=(const Extend&, const Extend&) noexcept;
    }
}

#endif
