//
//  test/utils/so/test-Value.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 23/01/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include <catch.hpp>

#include "utils/so/ValueIo.h"

using namespace drafter;
using namespace utils;
using namespace so;

SCENARIO("Serialize a utils::so::Value into json", "[simple-object]")
{
    GIVEN("an in place constructed Null in Value")
    {
        Value value(in_place_type<Null>{});

        WHEN("it is serialized into stringstream as JSON")
        {
            std::stringstream ss;
            ss << JsonValue(value);

            THEN("the stringstream contains: null")
            {
                REQUIRE(ss.str() == "null");
            }
        }
    }

    GIVEN("an in place constructed True in Value")
    {
        Value value(in_place_type<True>{});

        WHEN("it is serialized into stringstream as JSON")
        {
            std::stringstream ss;
            ss << JsonValue(value);

            THEN("the stringstream contains: true")
            {
                REQUIRE(ss.str() == "true");
            }
        }
    }

    GIVEN("an in place constructed False in Value")
    {
        Value value(in_place_type<False>{});

        WHEN("it is serialized into stringstream as JSON")
        {
            std::stringstream ss;
            ss << JsonValue(value);

            THEN("the stringstream contains: false")
            {
                REQUIRE(ss.str() == "false");
            }
        }
    }

    GIVEN("an in place constructed String{`Hello world!`}")
    {
        Value value(in_place_type<String>{}, "Hello world!");

        WHEN("it is serialized into stringstream as JSON")
        {
            std::stringstream ss;
            ss << JsonValue(value);

            THEN("the stringstream contains: \"Hello world!\"")
            {
                REQUIRE(ss.str() == "\"Hello world!\"");
            }
        }
    }

    GIVEN("an in place constructed Array{`Hello world!`}")
    {
        Value value(in_place_type<Array>{}, String{ "Hello world!" }, Number{ 5 });

        WHEN("it is serialized into stringstream as JSON")
        {
            std::stringstream ss;
            ss << JsonValue(value);

            THEN("the stringstream contains: [\"Hello world!\", 5]")
            {
                REQUIRE(ss.str() == "[\"Hello world!\",5]");
            }
        }
    }
}
