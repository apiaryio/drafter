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
            Array();

            ///
            /// Initialize an Array DSD from its member elements
            ///
            /// @param elements     elements to be contained
            ///
            template <typename... Element>
            explicit Array(std::unique_ptr<Element>... elements) : Array()
            {
                elements_.reserve(sizeof...(elements));
                utils::move_back(elements_, std::move(elements)...);
            }

            ///
            /// Initialize an Array DSD from another by consuming its memory representation
            ///
            /// @param other   Array DSD to be consumed
            ///
            Array(Array&& other);

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
            Array& operator=(Array rhs);

            ~Array() = default;

        public:
            friend void swap(Array& lhs, Array& rhs)
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

            using container_traits<Array, container_type>::erase;
        };

        bool operator==(const Array&, const Array&) noexcept;
        bool operator!=(const Array&, const Array&) noexcept;
    }
}

#endif
