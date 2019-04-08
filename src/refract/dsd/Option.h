//
//  refract/dsd/Option.h
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_DSD_OPTION_H
#define REFRACT_DSD_OPTION_H

#include <vector>
#include <memory>

#include "../ElementIfc.h"
#include "Traits.h"
#include "Utils.h"

namespace refract
{
    namespace dsd
    {
        ///
        /// Data structure definition (DSD) of a Refract Option Element
        ///
        /// @remark Defined by a set of Refract Elements
        ///
        class Option final : public container_traits<Option, std::vector<std::unique_ptr<IElement> > >
        {
            container_type elements_; //< set of Elements

        public:
            static const char* name; //< syntactical name of the DSD

        public:
            ///
            /// Initialize an empty Option DSD
            ///
            Option();

            ///
            /// Initialize an Option DSD from its member elements
            ///
            /// @param elements     elements to be contained
            ///
            template <typename... Element>
            explicit Option(std::unique_ptr<Element>... elements) : elements_()
            {
                elements_.reserve(sizeof...(elements));
                utils::move_back(elements_, std::move(elements)...);
            }

            ///
            /// Initialize an Option DSD from another by consuming its memory representation
            ///
            /// @param other   Option DSD to be consumed
            ///
            Option(Option&& other);

            ///
            /// Initialize an Option DSD from another by cloning its children
            ///
            /// @param other   Option DSD to be cloned from
            ///
            Option(const Option& other);

            ///
            /// Clear children and consume another Option DSD's memory representation
            ///
            /// @param other   Option DSD to be consumed
            ///
            Option& operator=(Option other);

            ~Option() = default;

        public:
            friend void swap(Option& lhs, Option& rhs)
            {
                using std::swap;
                swap(lhs.elements_, rhs.elements_);
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
            iterator insert(iterator it, std::unique_ptr<IElement> el);

            ///
            /// Remove a subsequence of children
            ///
            /// @param b    iterator to the first Element to be removed
            /// @param e    iterator following the last Element to be removed
            ///
            /// @return iterator following the last child removed
            ///
            iterator erase(iterator b, iterator e);

            using container_traits<Option, container_type>::erase;
        };

        bool operator==(const Option&, const Option&) noexcept;
        bool operator!=(const Option&, const Option&) noexcept;
    }
}

#endif
