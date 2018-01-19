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
#include <map>
#include <string>

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
                std::map<std::string, Value> data;

                Object() = default;
                Object(const Object&) = default;
                Object(Object&&) = default;
                Object& operator=(const Object&) = default;
                Object& operator=(Object&&) = default;
                ~Object() = default;

                explicit Object(std::map<std::string, Value> d) : data(std::move(d)) {}
            };

            struct Array {
                std::vector<Value> data;

                Array() = default;
                Array(const Array&) = default;
                Array(Array&&) = default;
                Array& operator=(const Array&) = default;
                Array& operator=(Array&&) = default;
                ~Array() = default;

                explicit Array(std::vector<Value> d) : data(std::move(d)) {}
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
        }
    }
}

#endif
