//
//  refract/dsd/Object.h
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_DSD_OBJECT_H
#define REFRACT_DSD_OBJECT_H

#include <algorithm>
#include <cassert>
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
        /// Data structure definition (DSD) of a Refract Object Element
        ///
        /// @remark Defined by a sequence of Refract Elements
        /// TODO possibly store mixins, members, etc in separate sequences
        ///
        class Object final : public container_traits<Object, std::vector<std::unique_ptr<IElement> > >
        {
            container_type elements_; //< sequence of Elements

        public:
            static const char* name; //< syntactical name of the DSD

        public:
            ///
            /// Initialize an empty Object DSD
            ///
            Object() = default;

            ///
            /// Initialize an Object DSD from a sequence of elements
            ///
            /// @param elements     elements to be contained
            ///
            template <typename... Element>
            Object(std::unique_ptr<Element>... elements) : elements_()
            {
                elements_.reserve(sizeof...(elements));
                utils::move_back(elements_, std::move(elements)...);
            }

            ///
            /// Initialize an Object DSD from another by consuming its memory representation
            ///
            /// @param other   Object DSD to be consumed
            ///
            Object(Object&& other) = default;

            ///
            /// Initialize an Object DSD from another by cloning its children
            ///
            /// @param other   Object DSD to be cloned from
            ///
            Object(const Object& other);

            ///
            /// Clear children and consume another Object DSD's memory representation
            ///
            /// @param rhs   Object DSD to be consumed
            ///
            Object& operator=(Object&& rhs) = default;

            ///
            /// Clear children and clone them from another Object DSD
            ///
            /// @param rhs   Object DSD to be cloned from
            ///
            Object& operator=(const Object& rhs);

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

        public: // collection-specific
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

            using container_traits<Object, container_type>::erase;

        public: // object-specific
            ///
            /// Add a property to this DSD
            ///
            /// @param name    name of the property to be added
            /// @param value   value of the property to be added
            ///
            /// @return iterator to member element representing this property
            ///
            iterator addMember(std::string name, std::unique_ptr<IElement> value);
        };

        bool operator==(const Object&, const Object&) noexcept;
        bool operator!=(const Object&, const Object&) noexcept;
    }
}

#endif
