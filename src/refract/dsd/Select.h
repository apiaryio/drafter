//
//  refract/dsd/Select.h
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_DSD_SELECT_H
#define REFRACT_DSD_SELECT_H

#include <vector>
#include <memory>

#include "../ElementFwd.h"
#include "Option.h"
#include "Traits.h"
#include "Utils.h"

namespace refract
{
    namespace dsd
    {
        ///
        /// Data structure definition (DSD) of a Refract Select Element
        ///
        /// @remark Defined by a set of Refract Option Elements
        ///
        class Select final : public container_traits<Select, std::vector<std::unique_ptr<OptionElement> > >
        {
            container_type elements_; //< set of Option Elements

        public:
            static const char* name; //< syntactical name of the DSD

        public:
            ///
            /// Initialize an empty Select DSD
            ///
            Select();

            ///
            /// Initialize an Select DSD from its member elements
            ///
            /// @param elements     elements to be contained
            ///
            template <typename... Element>
            explicit Select(std::unique_ptr<Element>... elements) : elements_()
            {
                elements_.reserve(sizeof...(elements));
                utils::move_back(elements_, std::move(elements)...);
            }

            ///
            /// Initialize a Select DSD from another by consuming its memory representation
            ///
            /// @param other   Select DSD to be consumed
            ///
            Select(Select&& other);

            ///
            /// Initialize a Select DSD from another by cloning its children
            ///
            /// @param other   Select DSD to be cloned from
            ///
            Select(const Select& other);

            ///
            /// Clear children and consume another Select DSD's memory representation
            ///
            /// @param other   Select DSD to be consumed
            ///
            Select& operator=(Select rhs);

            ~Select();

        public:
            friend void swap(Select& lhs, Select& rhs)
            {
                using std::swap;
                swap(lhs.elements_, rhs.elements_);
            }

        public:
            ///
            /// Get reference to memory representation
            ///
            const container_type& get() const noexcept
            {
                return elements_;
            }

        public: // iterators
            iterator begin() noexcept
            {
                return elements_.begin();
            }
            iterator end() noexcept
            {
                return elements_.end();
            }
            const_iterator begin() const noexcept
            {
                return elements_.begin();
            }
            const_iterator end() const noexcept
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
            iterator insert(iterator it, std::unique_ptr<OptionElement> el);

            ///
            /// Remove a subsequence of children
            ///
            /// @param b    iterator to the first Element to be removed
            /// @param e    iterator following the last Element to be removed
            ///
            /// @return iterator following the last child removed
            ///
            iterator erase(iterator b, iterator e);

            using container_traits<Select, container_type>::erase;
        };

        bool operator==(const Select&, const Select&) noexcept;
        bool operator!=(const Select&, const Select&) noexcept;
    }
}

#endif
