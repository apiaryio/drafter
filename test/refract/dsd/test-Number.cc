//
//  test/refract/dsd/test-Number.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "catch.hpp"

#include "refract/dsd/Number.h"

using namespace refract;
using namespace dsd;

TEST_CASE("`Number`'s default element name is `number`", "[Element][Number]")
{
    REQUIRE(std::string(Number::name) == "number");
}

SCENARIO("`Number` is default constructed and both copy- and move constructed from", "[ElementData][Number]")
{
    GIVEN("A default initialized Number")
    {
        Number number;

        THEN("its data is zero")
        {
            REQUIRE(number.get() == 0.0);
        }
        THEN("its value is zero")
        {
            double n = number;
            REQUIRE(n == 0.0);
        }

        WHEN("from it another Number is copy constructed")
        {
            Number number2(number);

            THEN("the latter Number's data is also zero")
            {
                REQUIRE(number2.get() == 0);
            }
        }

        WHEN("from it another Number is move constructed")
        {
            Number number2(std::move(number));

            THEN("the latter Number's data is also zero")
            {
                REQUIRE(number2.get() == 0);
            }
        }
    }
}

SCENARIO("Number is constructed from values, both copy- and move constructed from and all its copies are destroyed",
    "[ElementData][Number]")
{
    GIVEN("A double N = 39025.341")
    {
        const double N = 39025.341;

        WHEN("a Number is constructed using it")
        {
            {
                Number number(N);

                THEN("its data is N")
                {
                    REQUIRE(number.get() == N);
                }
                THEN("its value is N")
                {
                    const double P = number;
                    REQUIRE(N == P);
                }
            }
        }
    }

    GIVEN("A Number with value N = 34893.2539")
    {
        const double N = 34893.2539;
        Number number(N);

        WHEN("another Number is copy constructed from it")
        {
            Number number2(number);

            THEN("its data is N")
            {
                REQUIRE(number2.get() == N);
            }
            THEN("its value is N")
            {
                const double P = number2;
                REQUIRE(N == P);
            }
        }

        WHEN("another Number is move constructed from it")
        {
            Number number2(std::move(number));

            THEN("its data is N")
            {
                REQUIRE(number2.get() == N);
            }
            THEN("its value is N")
            {
                const double P = number2;
                REQUIRE(N == P);
            }
        }
    }
}

SCENARIO("number DSDs are tested for equality and inequality", "[Element][Number][equality]")
{
    GIVEN("An number DSD with 42.0 value")
    {
        Number data(42.0);

        GIVEN("An number element constructed equivalently")
        {
            Number data2(42.0);

            THEN("they test positive for equality")
            {
                REQUIRE(data == data2);
            }

            THEN("they test negative for inequality")
            {
                REQUIRE(!(data != data2));
            }
        }

        GIVEN("An number element with different value")
        {
            Number data2(3.14);

            THEN("they test negative for equality")
            {
                REQUIRE(!(data == data2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(data != data2);
            }
        }

        GIVEN("An empty number element")
        {
            Number data2;

            THEN("they test negative for equality")
            {
                REQUIRE(!(data == data2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(data != data2);
            }
        }
    }
}
