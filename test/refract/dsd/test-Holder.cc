//
//  test/refract/dsd/test-Holder.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include <catch2/catch.hpp>

#include <refract/Element.h>
#include <refract/dsd/Holder.h>

using namespace refract;
using namespace dsd;

TEST_CASE("`Holder`'s default element name is `holder`", "[Element][Holder]")
{
    REQUIRE(std::string(Holder::name) == "");
}

SCENARIO("`Holder` is default constructed and both copy- and move constructed from", "[ElementData][Holder]")
{
    GIVEN("A default initialized Holder")
    {
        Holder holder;

        THEN("it holds a nullptr")
        {
            REQUIRE(holder.data() == nullptr);
        }

        WHEN("from it another Holder is copy constructed")
        {
            Holder holder2(holder);

            THEN("the latter Holder also holds nullptr")
            {
                REQUIRE(holder2.data() == nullptr);
            }
        }

        WHEN("from it another Holder is copy constructed")
        {
            Holder holder2(std::move(holder));

            THEN("the original Holder holds nullptr")
            {
                REQUIRE(holder.data() == nullptr);
            }

            THEN("the latter Holder also holds nullptr")
            {
                REQUIRE(holder2.data() == nullptr);
            }
        }
    }
}

SCENARIO("`Holder` is constructed from value and is claimed", "[ElementData][Holder]")
{
    GIVEN("An `Holder` with a string element value")
    {
        auto inner = from_primitive("abc");
        const auto* innerPtr = inner.get();
        Holder holder(std::move(inner));

        WHEN("it is claimed from")
        {
            auto result = holder.claim();

            THEN("it holds nullptr")
            {
                REQUIRE(holder.data() == nullptr);
            }

            THEN("the result is the original element")
            {
                REQUIRE(innerPtr == result.get());
            }
        }
    }
}
