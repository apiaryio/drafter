//
//  test/refract/dsd/test-Bool.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "catch.hpp"

#include "refract/dsd/Bool.h"

using namespace refract;
using namespace dsd;

TEST_CASE("`Boolean`'s default element name is `bool`", "[Element][Boolean]")
{
    REQUIRE(std::string(Boolean::name) == "boolean");
}

SCENARIO("`Boolean` is default constructed and both copy- and move constructed from", "[ElementData][Boolean]")
{
    GIVEN("A default initialized Boolean")
    {
        Boolean boolean;

        THEN("its data is false")
        {
            REQUIRE(boolean.get() == false);
        }
        THEN("its value is false")
        {
            bool value = boolean;
            REQUIRE(value == false);
        }

        WHEN("from it another Boolean is copy constructed")
        {
            Boolean boolean2(boolean);

            THEN("the latter Boolean's data is also false")
            {
                REQUIRE(boolean2.get() == false);
            }
        }

        WHEN("from it another Boolean is move constructed")
        {
            Boolean boolean2(std::move(boolean));

            THEN("the latter Boolean's data is also false")
            {
                REQUIRE(boolean2.get() == false);
            }
        }
    }
}

SCENARIO("Boolean is constructed from a value, copy- and move constructed from and all its copies are destroyed",
    "[ElementData][Boolean]")
{
    GIVEN("A bool B = true")
    {
        const bool B = true;

        WHEN("a Boolean is constructed using it")
        {
            {
                Boolean boolean(B);

                THEN("its data is B")
                {
                    REQUIRE(boolean.get() == B);
                }
                THEN("its value is B")
                {
                    const bool P = boolean;
                    REQUIRE(B == P);
                }
            }
        }
    }

    GIVEN("A Boolean with value B = true")
    {
        const bool B = true;
        Boolean boolean(B);

        WHEN("another Boolean is copy constructed from it")
        {
            Boolean boolean2(boolean);

            THEN("its data is B")
            {
                REQUIRE(boolean2.get() == B);
            }
            THEN("its value is B")
            {
                const bool P = boolean2;
                REQUIRE(B == P);
            }
        }

        WHEN("another Boolean is move constructed from it")
        {
            Boolean boolean2(std::move(boolean));

            THEN("its data is B")
            {
                REQUIRE(boolean2.get() == B);
            }
            THEN("its value is B")
            {
                const bool P = boolean2;
                REQUIRE(B == P);
            }
        }
    }
}

SCENARIO("bool DSDs are tested for equality and inequality", "[Element][Boolean][equality]")
{
    GIVEN("An bool DSD with true value")
    {
        Boolean data(true);

        GIVEN("An bool element constructed equivalently")
        {
            Boolean data2(true);

            THEN("they test positive for equality")
            {
                REQUIRE(data == data2);
            }

            THEN("they test negative for inequality")
            {
                REQUIRE(!(data != data2));
            }
        }

        GIVEN("An bool element with different value")
        {
            Boolean data2(false);

            THEN("they test negative for equality")
            {
                REQUIRE(!(data == data2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(data != data2);
            }
        }

        GIVEN("An empty bool element")
        {
            Boolean data2;

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
