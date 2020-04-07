//
//  test/refract/dsd/test-Holder.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include <catch2/catch.hpp>

#include "refract/dsd/Holder.h"

#include "refract/Element.h"
#include "refract/ElementFwd.h"

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
    GIVEN("An `Holder` with a StringElement value")
    {
        auto value = make_element<StringElement>("foo");
        const auto* valuePtr = value.get();
        Holder holder(std::move(value));

        WHEN("it is claimed")
        {
            auto result = holder.claim();

            THEN("it holds nullptr")
            {
                REQUIRE(holder.data() == nullptr);
            }

            THEN("the held element is the one passed into its constructor")
            {
                REQUIRE(result.get() == valuePtr);
            }
        }
    }
}
