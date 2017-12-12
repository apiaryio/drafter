//
//  refract/dsd/Array.h
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_DSD_ARRAY_H
#define REFRACT_DSD_ARRAY_H

#include <vector>
#include <memory>
#include <algorithm>

#include "../ElementIfc.h"
#include "Traits.h"
#include "Utils.h"

namespace refract
{
    namespace dsd
    {
        ///
        /// Data structure definition (DSD) of a Refract Array Element
        ///
        /// @remark Defined by a sequence of Refract Elements
        /// TODO move value to attributes and leave this defined by its type
        ///
        class Array final : public container_traits<Array, std::vector<std::unique_ptr<IElement> > >
        {
            container_type elements_; //< sequence of Elements

        public:
            static const char* name; //< syntactical name of the DSD

        public:
            ///
            /// Initialize an empty Array DSD
            ///
            Array() = default;

            ///
            /// Initialize an Array DSD from its member elements
            ///
            /// @param elements     elements to be contained
            ///
            template <typename... Element>
            Array(std::unique_ptr<Element>... elements) : elements_()
            {
                elements_.reserve(sizeof...(elements));
                utils::move_back(elements_, std::move(elements)...);
            }

            ///
            /// Initialize an Array DSD from another by consuming its memory representation
            ///
            /// @param other   Array DSD to be consumed
            ///
            Array(Array&& other) = default;

            ///
            /// Initialize an Array DSD from another by cloning its children
            ///
            /// @param other   Array DSD to be cloned from
            ///
            Array(const Array& other);

            ///
            /// Clear children and consume another Array DSD's memory representation
            ///
            /// @param rhs   Array DSD to be consumed
            ///
            Array& operator=(Array&& rhs) = default;

            ///
            /// Clear children and clone them from another Array DSD
            ///
            /// @param rhs   Array DSD to be cloned from
            ///
            Array& operator=(const Array& rhs);

            ~Array() = default;

        public:
            ///
            /// Get reference to memory representation
            /// @deprecated
            ///
            [[deprecated]] const container_type& data() const noexcept
            {
                return elements_;
            }

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

            using container_traits<Array, container_type>::erase;
        };

        bool operator==(const Array&, const Array&) noexcept;
        bool operator!=(const Array&, const Array&) noexcept;
    }
}

#endif
