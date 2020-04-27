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
#include <mpark/variant.hpp>

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

            using Value = mpark::variant<Null, True, False, String, Number, Object, Array>;

            struct from_list {
            };

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
                explicit Object(from_list, Values&&... values) : data({ std::forward<Values>(values)... })
                {
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
                explicit Array(from_list, Values&&... values)

                    : data({ std::forward<Values>(values)... })
                {
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
                std::string data = "0";

                Number() = default;
                Number(const Number&) = default;
                Number(Number&&) = default;
                Number& operator=(const Number&) = default;
                Number& operator=(Number&&) = default;
                ~Number() = default;

                explicit Number(std::string d) : data(std::move(d)) {}

                template <typename N, typename = typename std::enable_if<std::is_integral<N>::value>::type>
                explicit Number(N v) noexcept : data(std::to_string(v))
                {
                }
            };
        } // namespace so

        namespace so
        {
            bool operator==(const Null& lhs, const Null& rhs);
            bool operator==(const True& lhs, const True& rhs);
            bool operator==(const False& lhs, const False& rhs);
            bool operator==(const String& lhs, const String& rhs);
            bool operator==(const Number& lhs, const Number& rhs);
            bool operator==(const Object& lhs, const Object& rhs);
            bool operator==(const Array& lhs, const Array& rhs);
            bool operator==(const Value& lhs, const Value& rhs);

            template <typename ValueType>
            void emplace_unique(Object& c, std::string key, ValueType&& value)
            {
                auto it = std::find_if(                                       //
                    c.data.begin(),                                           //
                    c.data.end(),                                             //
                    [&key](const Object::container_type::value_type& entry) { //
                        return entry.first == key;
                    });

                if (it == c.data.end())
                    c.data.emplace_back(key, std::forward<ValueType>(value));
                else
                    it->second = std::forward<ValueType>(value);
            }

            void emplace_unique(Object& c, Object::container_type::value_type&& property);

            void emplace_unique(Array& c, Value&& value);

            Value* find(Object& c, const std::string& key);
        } // namespace so
    }     // namespace utils
} // namespace drafter

#endif
