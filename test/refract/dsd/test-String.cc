//
//  test/refract/dsd/test-String.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "catch.hpp"

#include "refract/dsd/String.h"

using namespace refract;
using namespace dsd;

TEST_CASE("`String`'s default element name is `string`", "[Element][String]")
{
    REQUIRE(std::string(String::name) == "string");
}

SCENARIO("`String` is default constructed and both copy- and move constructed from", "[ElementData][String]")
{
    GIVEN("A default initialized String")
    {
        String string;

        THEN("its data is an empty string")
        {
            REQUIRE(string.get() == "");
        }
        THEN("its value is an empty string")
        {
            std::string value = string;
            REQUIRE(value == "");
        }
        THEN("it is empty")
        {
            REQUIRE(string.empty());
        }

        WHEN("from it another String is copy constructed")
        {
            String string2(string);

            THEN("the latter String's data is also an empty string")
            {
                REQUIRE(string2.get() == "");
            }
            THEN("it is empty")
            {
                REQUIRE(string2.empty());
            }
        }

        WHEN("from it another String is move constructed")
        {
            String string2(std::move(string));

            THEN("the latter String's data is also an empty string")
            {
                REQUIRE(string2.get() == "");
            }
            THEN("it is empty")
            {
                REQUIRE(string2.empty());
            }
        }
    }
}

SCENARIO("String is constructed from values, both copy- and move constructed from and all its copies are destroyed",
    "[ElementData][String]")
{
    GIVEN("A std::string B = `foobar32dsfjklsgh\\000fdfjks`")
    {
        const std::string B = "foobar32dsfjklsgh\000fdfjks";

        WHEN("a String is constructed using it")
        {
            {
                String string(B);

                THEN("its data is B")
                {
                    REQUIRE(string.get() == B);
                }
                THEN("its value is B")
                {
                    const std::string P = string;
                    REQUIRE(B == P);
                }
                THEN("it is not empty")
                {
                    REQUIRE(!string.empty());
                }
            }
        }
    }

    GIVEN("A String with value B = \"foobar32dsfjklsgh\\000fdfjks\"")
    {
        const std::string B = "foobar32dsfjklsgh\000fdfjks";
        String string(B);

        WHEN("another String is copy constructed from it")
        {
            String string2(string);

            THEN("its data is B")
            {
                REQUIRE(string2.get() == B);
            }
            THEN("its value is B")
            {
                const std::string P = string2;
                REQUIRE(B == P);
            }
        }

        WHEN("another String is move constructed from it")
        {
            String string2(std::move(string));

            THEN("its data is B")
            {
                REQUIRE(string2.get() == B);
            }
            THEN("its value is B")
            {
                const std::string P = string2;
                REQUIRE(B == P);
            }
        }
    }
}

SCENARIO("string DSDs are tested for equality and inequality", "[Element][String][equality]")
{
    GIVEN("An string DSD with \"foobar\" value")
    {
        String data("foobar");

        GIVEN("An string element constructed equivalently")
        {
            String data2("foobar");

            THEN("they test positive for equality")
            {
                REQUIRE(data == data2);
            }

            THEN("they test negative for inequality")
            {
                REQUIRE(!(data != data2));
            }
        }

        GIVEN("An string element with different value")
        {
            String data2("foobarz");

            THEN("they test negative for equality")
            {
                REQUIRE(!(data == data2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(data != data2);
            }
        }

        GIVEN("An empty string element")
        {
            String data2;

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
