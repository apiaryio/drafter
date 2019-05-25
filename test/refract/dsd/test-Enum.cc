//
//  test/refract/dsd/test-Enum.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include <catch2/catch.hpp>

#include "refract/dsd/Enum.h"
#include "refract/Element.h"

using namespace refract;
using namespace dsd;

TEST_CASE("`Enum`'s default element name is `enum`", "[Element][Enum]")
{
    REQUIRE(std::string(Enum::name) == "enum");
}

SCENARIO("`Enum` is default constructed and both copy- and move constructed from", "[ElementData][Enum]")
{
    GIVEN("A default initialized Enum")
    {
        Enum enm;

        THEN("it holds a nullptr")
        {
            REQUIRE(enm.value() == nullptr);
        }

        WHEN("from it another Enum is copy constructed")
        {
            Enum enm2(enm);

            THEN("the latter Enum also holds nullptr")
            {
                REQUIRE(enm2.value() == nullptr);
            }
        }

        WHEN("from it another Enum is copy constructed")
        {
            Enum enm2(std::move(enm));

            THEN("the original Enum holds nullptr")
            {
                REQUIRE(enm.value() == nullptr);
            }

            THEN("the latter Enum also holds nullptr")
            {
                REQUIRE(enm2.value() == nullptr);
            }
        }
    }
}

SCENARIO("`Enum` is constructed from value and is claimed", "[ElementData][Enum]")
{
    GIVEN("An `Enum` with a StringElement value")
    {
        auto el = make_element<StringElement>();
        const auto* elPtr = el.get();
        Enum enm(std::move(el));

        WHEN("it is claimed")
        {
            auto result = enm.claim();

            THEN("it holds nullptr")
            {
                REQUIRE(enm.value() == nullptr);
            }

            THEN("the str is the one passed into its constructor")
            {
                REQUIRE(result.get() == elPtr);
            }
        }
    }
}
