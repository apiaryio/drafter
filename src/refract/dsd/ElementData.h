//
//  refract/dsd/ElementData.h
//  librefract
//
//  Created by Thomas Jandecka on 05/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_DSD_ELEMENTDATA_H
#define REFRACT_DSD_ELEMENTDATA_H

#include "Array.h"
#include "Bool.h"
#include "Enum.h"
#include "Extend.h"
#include "Holder.h"
#include "Member.h"
#include "Null.h"
#include "Number.h"
#include "Object.h"
#include "Option.h"
#include "Ref.h"
#include "Select.h"
#include "String.h"

namespace refract
{
    namespace dsd
    {
        ///
        /// Wrap type with associated data type
        ///
        /// Obtains the data representation of a primitive type
        ///
        /// @tparam T   primitive type to wrap
        ///
        template <typename T>
        struct data_of {
        };

        template <>
        struct data_of<bool> {
            using type = dsd::Boolean;
        };

        template <>
        struct data_of<std::nullptr_t> {
            using type = dsd::Null;
        };

        template <>
        struct data_of<int> {
            using type = dsd::Number;
        };

        template <>
        struct data_of<size_t> {
            using type = dsd::Number;
        };

        template <>
        struct data_of<std::string> {
            using type = dsd::String;
        };

        template <size_t N>
        struct data_of<char[N]> {
            using type = dsd::String;
        };
    }
}

#endif
