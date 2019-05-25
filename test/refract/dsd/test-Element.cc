//
//  test/refract/dsd/test-Element.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include <catch2/catch.hpp>

#include "refract/Element.h"

using namespace refract;
using namespace dsd;

SCENARIO("`isReserved` identifies reserved type names", "[Element]")
{
    WHEN("it is invoked with an `array` literal")
    {
        bool result = isReserved("array");
        THEN("the result is `true`")
        {
            REQUIRE(result);
        }
    }
    WHEN("it is invoked with an `boolean` literal")
    {
        bool result = isReserved("boolean");
        THEN("the result is `true`")
        {
            REQUIRE(result);
        }
    }
    WHEN("it is invoked with an `enum` literal")
    {
        bool result = isReserved("enum");
        THEN("the result is `true`")
        {
            REQUIRE(result);
        }
    }
    WHEN("it is invoked with an `extend` literal")
    {
        bool result = isReserved("extend");
        THEN("the result is `true`")
        {
            REQUIRE(result);
        }
    }
    WHEN("it is invoked with an `generic` literal")
    {
        bool result = isReserved("generic");
        THEN("the result is `true`")
        {
            REQUIRE(result);
        }
    }
    WHEN("it is invoked with an `member` literal")
    {
        bool result = isReserved("member");
        THEN("the result is `true`")
        {
            REQUIRE(result);
        }
    }
    WHEN("it is invoked with an `null` literal")
    {
        bool result = isReserved("null");
        THEN("the result is `true`")
        {
            REQUIRE(result);
        }
    }
    WHEN("it is invoked with an `number` literal")
    {
        bool result = isReserved("number");
        THEN("the result is `true`")
        {
            REQUIRE(result);
        }
    }
    WHEN("it is invoked with an `object` literal")
    {
        bool result = isReserved("object");
        THEN("the result is `true`")
        {
            REQUIRE(result);
        }
    }
    WHEN("it is invoked with an `option` literal")
    {
        bool result = isReserved("option");
        THEN("the result is `true`")
        {
            REQUIRE(result);
        }
    }
    WHEN("it is invoked with an `ref` literal")
    {
        bool result = isReserved("ref");
        THEN("the result is `true`")
        {
            REQUIRE(result);
        }
    }
    WHEN("it is invoked with an `select` literal")
    {
        bool result = isReserved("select");
        THEN("the result is `true`")
        {
            REQUIRE(result);
        }
    }
    WHEN("it is invoked with an `string` literal")
    {
        bool result = isReserved("string");
        THEN("the result is `true`")
        {
            REQUIRE(result);
        }
    }
}

SCENARIO("Elements can be cloned with refract::clone(const IElement&)", "[Element]")
{
    GIVEN("An empty BooleanElement")
    {
        auto element = make_empty<BooleanElement>();

        WHEN("it is cloned")
        {
            auto c = element->clone();

            THEN("the clone is equal to the original")
            {
                REQUIRE(*element == *c);
            }
        }
    }

    GIVEN("A BooleanElement with value")
    {
        auto element = make_element<BooleanElement>(true);

        WHEN("it is cloned")
        {
            auto c = element->clone();

            THEN("the clone is equal to the original")
            {
                REQUIRE(*element == *c);
            }
        }
    }

    GIVEN("An empty BooleanElement with attributes and meta")
    {
        auto element = make_empty<BooleanElement>();
        element->attributes().set("foo", from_primitive("bar"));
        element->attributes().set("answer", from_primitive(42));

        element->meta().set("id", from_primitive(1));
        element->meta().set("a",
            make_element<ObjectElement>( //
                make_element<MemberElement>("b", from_primitive(3))));
        element->meta().set("msg", from_primitive("Hello world!"));

        WHEN("it is cloned")
        {
            auto c = element->clone();

            THEN("the clone is equal to the original")
            {
                REQUIRE(*element == *c);
            }
        }
    }
}
