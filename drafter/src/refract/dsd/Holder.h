//
//  refract/dsd/Holder.h
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_DSD_HOLDER_H
#define REFRACT_DSD_HOLDER_H

#include <memory>

#include "../ElementIfc.h"

namespace refract
{
    namespace dsd
    {
        ///
        /// Data structure definition (DSD) of a Holder Element
        ///
        /// @remark Defined by another Refract Element
        /// FIXME: This is just a temporary element which is not in the refract spec
        /// until the Element implementation is moved away from abstraction.
        class Holder final
        {
        public:
            using Data = std::unique_ptr<IElement>;

            static const char* name; //< syntactical name of the DSD

        private:
            Data data_ = nullptr;

        public:
            ///
            /// Initialize a Holder DSD with its Element set to nullptr
            ///
            Holder();

            ///
            /// Initialize a Holder DSD from another by consuming its Element
            ///
            /// @param other   Holder DSD to be consumed
            ///
            Holder(Holder&& other);

            ///
            /// Initialize a Holder DSD from another by cloning its Element
            ///
            /// @param other   Holder DSD to be cloned from
            ///
            Holder(const Holder& other);

            ///
            /// Initialize a Member DSD from an Element
            ///
            /// @param el   Element to be consumed
            ///
            explicit Holder(std::unique_ptr<IElement> el);

            ///
            /// Consume another Holder DSD's Element
            ///
            /// @param other   Holder DSD to be consumed
            ///
            Holder& operator=(Holder other);

            ~Holder() = default;

        public:
            friend void swap(Holder& lhs, Holder& rhs)
            {
                using std::swap;
                swap(lhs.data_, rhs.data_);
            }

        public:
            const IElement* data() const noexcept
            {
                return data_.get();
            }

            ///
            /// Take ownership of the Element of this DSD
            /// @remark sets Element to nullptr
            ///
            /// @return Element of this DSD
            ///
            Data claim() noexcept
            {
                return std::move(data_);
            }
        };

        bool operator==(const Holder&, const Holder&) noexcept;
    }
}

#endif
