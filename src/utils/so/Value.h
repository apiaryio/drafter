//
//  utils/so/Value.h
//  librefract
//
//  Created by Thomas Jandecka on 17/01/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_UTILS_SO_VALUE_H
#define DRAFTER_UTILS_SO_VALUE_H

#include <vector>
#include <string>
#include <algorithm>
#include <boost/container/vector.hpp>

#include "../Variant.h"

namespace drafter
{
    namespace utils
    {
        namespace so
        {
            struct Null;
            struct True;
            struct False;
            struct String;
            struct Number;
            struct Object;
            struct Array;

            using Value = variant<Null, True, False, String, Number, Object, Array>;

            struct Null {
            };

            struct True {
            };

            struct False {
            };

            struct Object {
                using container_type = boost::container::vector<std::pair<std::string, Value> >;
                container_type data;

                Object() = default;
                Object(const Object&) = default;
                Object(Object&&) = default;
                Object& operator=(const Object&) = default;
                Object& operator=(Object&&) = default;
                ~Object() = default;

                template <typename... Values>
                explicit Object(Values&&... values) : data({ std::forward<Values>(values)... })
                {
                    using namespace drafter::utils;
                    static_assert(all_of<!std::is_same<drafter::utils::bare<Values>, Object>::value...>::value, "");
                }
            };

            struct Array {
                using container_type = boost::container::vector<Value>;
                container_type data;

                Array() = default;
                Array(const Array&) = default;
                Array(Array&&) = default;
                Array& operator=(const Array&) = default;
                Array& operator=(Array&&) = default;
                ~Array() = default;

                template <typename... Values>
                explicit Array(Values&&... values) : data({ std::forward<Values>(values)... })
                {
                    using namespace drafter::utils;
                    static_assert(all_of<!std::is_same<drafter::utils::bare<Values>, Array>::value...>::value, "");
                }
            };

            struct String {
                std::string data; // unescaped

                String() = default;
                String(const String&) = default;
                String(String&&) = default;
                String& operator=(const String&) = default;
                String& operator=(String&&) = default;
                ~String() = default;

                explicit String(std::string d) : data(d) {}
            };

            struct Number {
                double data = 0;

                Number() = default;
                Number(const Number&) = default;
                Number(Number&&) = default;
                Number& operator=(const Number&) = default;
                Number& operator=(Number&&) = default;
                ~Number() = default;

                explicit Number(double d) : data(std::move(d)) {}
            };
        } // namespace so

        namespace so
        {
            template <typename ValueType>
            void emplace_unique(so::Object& c, std::string key, ValueType&& value)
            {
                auto it = std::find_if(
                    c.data.begin(), c.data.end(), [&key](const auto& entry) { return entry.first == key; });
                if (it == c.data.end())
                    c.data.emplace_back(key, std::forward<ValueType>(value));
                else
                    it->second = std::forward<ValueType>(value);
            }
        } // namespace so
    }     // namespace utils
} // namespace drafter

#endif
