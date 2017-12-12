//
//  test/refract/dsd/test-Ref.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "catch.hpp"

#include "refract/dsd/Ref.h"

using namespace refract;
using namespace dsd;

TEST_CASE("`Ref`'s default element name is `ref`", "[Element][Ref]")
{
    REQUIRE(std::string(Ref::name) == "ref");
}

SCENARIO("`Ref` is default constructed and both copy- and move constructed from", "[ElementData][Ref]")
{
    GIVEN("A default initialized Ref")
    {
        Ref ref;

        THEN("its symbol is an empty string")
        {
            REQUIRE(ref.symbol() == "");
        }

        THEN("it equals an empty string")
        {
            REQUIRE(ref == "");
        }

        WHEN("from it another Ref is copy constructed")
        {
            Ref ref2(ref);

            THEN("the latter Ref's symbol is also an empty string")
            {
                REQUIRE(ref2.symbol() == "");
            }

            THEN("the latter Ref also equals empty string")
            {
                REQUIRE(ref2 == "");
            }
        }

        WHEN("from it another Ref is move constructed")
        {
            Ref ref2(std::move(ref));

            THEN("the latter Ref's symbol is also an empty string")
            {
                REQUIRE(ref2.symbol() == "");
            }

            THEN("the latter Ref also equals empty string")
            {
                REQUIRE(ref2 == "");
            }
        }
    }
}

SCENARIO("Ref is constructed from values, both copy- and move constructed from and all its copies are destroyed",
    "[ElementData][Ref]")
{
    GIVEN("A std::string B = `foobar32dsfjklsgh\\000fdfjks`")
    {
        const std::string B = "foobar32dsfjklsgh\000fdfjks";

        WHEN("a Ref is constructed using it")
        {
            {
                Ref ref(B);

                THEN("its symbol is B")
                {
                    REQUIRE(ref.symbol() == B);
                }

                THEN("it is equal to B")
                {
                    REQUIRE(ref == B);
                }
            }
        }
    }

    GIVEN("A Ref with value B = \"foobar32dsfjklsgh\\000fdfjks\"")
    {
        const std::string B = "foobar32dsfjklsgh\000fdfjks";
        Ref ref(B);

        WHEN("another Ref is copy constructed from it")
        {
            Ref ref2(ref);

            THEN("its symbol is B")
            {
                REQUIRE(ref2.symbol() == B);
            }

            THEN("it is equal to B")
            {
                REQUIRE(ref2 == B);
            }
        }

        WHEN("another Ref is move constructed from it")
        {
            Ref ref2(std::move(ref));

            THEN("its symbol is B")
            {
                REQUIRE(ref2.symbol() == B);
            }

            THEN("it is equal to B")
            {
                REQUIRE(ref2 == B);
            }
        }
    }
}

SCENARIO("ref DSDs are tested for equality and inequality", "[Element][Ref][equality]")
{
    GIVEN("An ref DSD with \"foobar\" value")
    {
        Ref data("foobar");

        GIVEN("An ref element constructed equivalently")
        {
            Ref data2("foobar");

            THEN("they test positive for equality")
            {
                REQUIRE(data == data2);
            }

            THEN("they test negative for inequality")
            {
                REQUIRE(!(data != data2));
            }
        }

        GIVEN("An ref element with different value")
        {
            Ref data2("foobarz");

            THEN("they test negative for equality")
            {
                REQUIRE(!(data == data2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(data != data2);
            }
        }

        GIVEN("An empty ref element")
        {
            Ref data2;

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
