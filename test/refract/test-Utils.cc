//
//  test/refract/test-Utils.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 02/12/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include <cassert>
#include <catch2/catch.hpp>
#include "refract/Element.h"

#include "refract/Utils.h"

using namespace refract;

SCENARIO("Elements are visited by a void() visitor", "[Element][utils][visitor]")
{
    GIVEN("an empty boolean element")
    {
        BooleanElement b;

        WHEN("it is visited by a void visitor")
        {
            const IElement* arg = nullptr;
            visit(b, [&arg](const auto& el) { arg = &el; });

            THEN("the element is passed to the visitor by argument")
            {
                REQUIRE(arg == &b);
            }
        }
    }
}

SCENARIO("Elements are visited by a int() visitor", "[Element][utils][visitor]")
{
    GIVEN("an empty boolean element")
    {
        BooleanElement b;

        WHEN("it is visited by a int() visitor returning 42")
        {
            const IElement* arg = nullptr;
            const auto result = visit(b, [&arg](const auto& el) -> int {
                arg = &el;
                return 42;
            });

            THEN("the element is passed to the visitor by argument")
            {
                REQUIRE(arg == &b);
            }

            THEN("the result of the visit is an int")
            {
                REQUIRE(typeid(result) == typeid(int));
            }

            THEN("the result of the visit is 42")
            {
                REQUIRE(result == 42);
            }
        }
    }
}

SCENARIO("Elements are visited by a foo&() visitor", "[Element][utils][visitor]") {}

SCENARIO("Elements are visited by a foo() visitor", "[Element][utils][visitor]") {}

SCENARIO("Elements are tested for equality", "[Element][utils]")
{
    GIVEN("An empty boolean element")
    {
        auto el = make_empty<BooleanElement>();

        GIVEN("Another empty boolean element")
        {
            auto el2 = make_empty<BooleanElement>();

            THEN("they test positive for equality")
            {
                REQUIRE(*el == *el2);
            }

            THEN("they test negative for inequality")
            {
                REQUIRE(!(*el != *el2));
            }
        }

        GIVEN("Another empty array element")
        {
            auto el2 = make_empty<ArrayElement>();

            THEN("they test negative for equality")
            {
                REQUIRE(!(*el == *el2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(*el != *el2);
            }
        }

        GIVEN("Another empty string element")
        {
            auto el2 = make_empty<StringElement>();

            THEN("they test negative for equality")
            {
                REQUIRE(!(*el == *el2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(*el != *el2);
            }
        }
    }

    GIVEN("An empty boolean element with some meta & attribute entries")
    {
        auto make_test_element = []() {
            auto result = make_empty<BooleanElement>();
            result->attributes().set("id", from_primitive(42));
            result->attributes().set("ref", from_primitive("Foo"));

            result->meta().set("bar", make_element<ArrayElement>(from_primitive("abc")));
            result->meta().set("pi", from_primitive(3));
            return result;
        };

        auto el = make_test_element();

        GIVEN("An element constructed equivalently")
        {
            auto el2 = make_test_element();

            THEN("they test positive for equality")
            {
                REQUIRE(*el == *el2);
            }

            THEN("they test negative for inequality")
            {
                REQUIRE(!(*el != *el2));
            }

            WHEN("the second element is reassigned an equivalent meta entry")
            {
                el2->meta().set("pi", from_primitive(3));

                THEN("they test positive for equality")
                {
                    REQUIRE(*el == *el2);
                }

                THEN("they test negative for inequality")
                {
                    REQUIRE(!(*el != *el2));
                }
            }

            WHEN("the second element is reassigned an equivalent attributes entry")
            {
                el2->attributes().set("ref", from_primitive("Foo"));

                THEN("they test positive for equality")
                {
                    REQUIRE(*el == *el2);
                }

                THEN("they test negative for inequality")
                {
                    REQUIRE(!(*el != *el2));
                }
            }
        }

        GIVEN("An element constructed equivalently with additional attributes entries")
        {
            auto el2 = make_test_element();
            el2->attributes().set("a", from_primitive(1));

            THEN("they test negative for equality")
            {
                REQUIRE(!(*el == *el2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(*el != *el2);
            }
        }

        GIVEN("An element constructed equivalently with a attributes entry modified")
        {
            auto el2 = make_test_element();
            el2->attributes().set("id", from_primitive(41));

            THEN("they test negative for equality")
            {
                REQUIRE(!(*el == *el2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(*el != *el2);
            }
        }

        GIVEN("An element constructed equivalently with attributes entries missing")
        {
            auto make_test_element_missing = []() {
                auto result = make_empty<BooleanElement>();
                result->attributes().set("id", from_primitive(42));
                // result->attributes().set("ref", from_primitive("Foo"));

                result->meta().set("bar", make_element<ArrayElement>(from_primitive("abc")));
                result->meta().set("pi", from_primitive(3));
                return result;
            };

            auto el2 = make_test_element_missing();

            THEN("they test negative for equality")
            {
                REQUIRE(!(*el == *el2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(*el != *el2);
            }
        }

        GIVEN("An element constructed equivalently with additional meta entries")
        {
            auto el2 = make_test_element();
            el2->meta().set("a", from_primitive(1));

            THEN("they test negative for equality")
            {
                REQUIRE(!(*el == *el2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(*el != *el2);
            }
        }

        GIVEN("An element constructed equivalently with a meta entry modified")
        {
            auto el2 = make_test_element();
            el2->meta().set("bar", make_element<ArrayElement>(from_primitive("ab")));

            THEN("they test negative for equality")
            {
                REQUIRE(!(*el == *el2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(*el != *el2);
            }
        }

        GIVEN("An element constructed equivalently with meta entries missing")
        {
            auto make_test_element_missing = []() {
                auto result = make_empty<BooleanElement>();
                result->attributes().set("id", from_primitive(42));
                result->attributes().set("ref", from_primitive("Foo"));

                result->meta().set("bar", make_element<ArrayElement>(from_primitive("abc")));
                // result->meta().set("pi", from_primitive(3));
                return result;
            };

            auto el2 = make_test_element_missing();

            THEN("they test negative for equality")
            {
                REQUIRE(!(*el == *el2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(*el != *el2);
            }
        }
    }
}
