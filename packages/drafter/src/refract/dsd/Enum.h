//
//  refract/dsd/Enum.h
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_DSD_ENUM_H
#define REFRACT_DSD_ENUM_H

#include <vector>
#include <memory>

#include "../ElementIfc.h"
#include "Traits.h"

namespace refract
{
    namespace dsd
    {
        ///
        /// Data structure definition (DSD) of a Refract Array Element
        ///
        /// @remark Defined by a sequence of Refract Elements
        ///
        class Enum final : public container_traits<Enum, std::vector<std::unique_ptr<IElement> > >
        {
        public:
            using Data = std::unique_ptr<IElement>;

            static const char* name; //< syntactical name of the DSD

        private:
            Data value_ = nullptr;

        public:
            ///
            /// Initialize an empty Enum DSD
            ///
            Enum();

            ///
            /// Initialize an Enum DSD from its value
            ///
            /// @param value     an Element
            ///
            explicit Enum(Data value);

            ///
            /// Initialize an Enum DSD from another by consuming its memory representation
            ///
            /// @param other   Enum DSD to be consumed
            ///
            Enum(Enum&& other);

            ///
            /// Initialize an Enum DSD from another by cloning its children
            ///
            /// @param other   Enum DSD to be cloned from
            ///
            Enum(const Enum& other);

            ///
            /// Clear children and consume another Enum DSD's memory representation
            ///
            /// @param rhs   Enum DSD to be consumed
            ///
            Enum& operator=(Enum rhs);

            ~Enum() = default;

        public:
            friend void swap(Enum& lhs, Enum& rhs)
            {
                using std::swap;
                swap(lhs.value_, rhs.value_);
            }

        public:
            const IElement* value() const noexcept
            {
                return value_.get();
            }

            ///
            /// Take ownership of the Element of this DSD
            /// @remark sets Element to nullptr
            ///
            /// @return Element of this DSD
            ///
            Data claim() noexcept
            {
                return std::move(value_);
            }
        };

        bool operator==(const Enum&, const Enum&) noexcept;
    }
}

#endif
