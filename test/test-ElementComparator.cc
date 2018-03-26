#include "ElementComparator.h"
#include "catch.hpp"
#include "refract/Element.h"

using namespace drafter;
using S = refract::StringElement;
using N = refract::NumberElement;

SCENARIO("Compare equality of elements", "[Element][comparator][equal]")
{

    GIVEN("Two empty primitive elements of same type")
    {

        auto first = make_empty<S>();
        auto second = make_empty<S>();

        WHEN("it is compared")
        {
            THEN("it is recognized as equal")
            {
                REQUIRE(true == visit(*first, ElementComparator{ *second }));
            }
        }
    }

    GIVEN("Two empty primitive elements of diferent type")
    {

        auto first = make_empty<S>();
        auto second = make_empty<N>();

        WHEN("it is compared")
        {
            THEN("it is recognized as non-equal")
            {
                REQUIRE(false == visit(*first, ElementComparator{ *second }));
            }
        }
    }

    GIVEN("Two primitive elements of same type with same value")
    {

        auto first = make_element<S>("a");
        auto second = make_element<S>("a");

        WHEN("it is compared")
        {
            THEN("it is recognized as equal")
            {
                REQUIRE(true == visit(*first, ElementComparator{ *second }));
            }
        }
    }

    GIVEN("Two primitive elements of same type with different value")
    {

        auto first = make_element<S>("a");
        auto second = make_element<S>("b");

        WHEN("it is compared")
        {
            THEN("it is recognized as non-equal")
            {
                REQUIRE(false == visit(*first, ElementComparator{ *second }));
            }
        }
    }

    GIVEN("Two empty primitive elements of same type with different element name")
    {

        auto first = make_element<S>();
        first->element("A");
        auto second = make_element<S>();

        WHEN("it is compared")
        {
            THEN("it is recognized as non-equal")
            {
                REQUIRE(false == visit(*first, ElementComparator{ *second }));
            }
        }
    }

    GIVEN("Two empty primitive different InfoElements")
    {

        auto first = make_element<S>();
        first->attributes().set("key", make_element<S>("value"));

        auto second = make_element<S>();

        WHEN("it is compared")
        {
            THEN("it is recognized as non-equal")
            {
                REQUIRE(false == visit(*first, ElementComparator{ *second }));
            }
        }
    }

    GIVEN("Two empty primitive elements with same InfoElements")
    {

        auto first = make_element<S>();
        first->attributes().set("key", make_element<S>("value"));

        auto second = make_element<S>();
        second->attributes().set("key", make_element<S>("value"));

        WHEN("it is compared")
        {
            THEN("it is recognized as equal")
            {
                REQUIRE(true == visit(*first, ElementComparator{ *second }));
            }
        }
    }

    GIVEN("Two empty primitive elements with different InfoElements value")
    {

        auto first = make_element<S>();
        first->attributes().set("key", make_element<S>("value"));

        auto second = make_element<S>();
        second->attributes().set("key", make_element<S>("another value"));

        WHEN("it is compared")
        {
            THEN("it is recognized as non-equal")
            {
                REQUIRE(false == visit(*first, ElementComparator{ *second }));
            }
        }
    }
}

SCENARIO("Compare equality of elements with sourceMaps", "[Element][comparator][equal]")
{
    GIVEN("Two empty primitive elements with different sourceMap value")
    {

        auto first = make_element<S>();
        first->attributes().set("sourceMap", make_element<S>("value"));

        auto second = make_element<S>();
        second->attributes().set("sourceMap", make_element<S>("another value"));

        WHEN("it is compared")
        {
            THEN("it is recognized as equal")
            {
                REQUIRE(true == visit(*first, ElementComparator{ *second }));
            }
        }
    }

    GIVEN("Two empty primitive elements first with sourceMap")
    {

        auto first = make_element<S>();
        first->attributes().set("sourceMap", make_element<S>("value"));

        auto second = make_element<S>();

        WHEN("it is compared")
        {
            THEN("it is recognized as equal")
            {
                REQUIRE(true == visit(*first, ElementComparator{ *second }));
            }
        }
    }

    GIVEN("Two empty primitive elements second with sourceMap")
    {

        auto first = make_element<S>();

        auto second = make_element<S>();
        second->attributes().set("sourceMap", make_element<S>("value"));

        WHEN("it is compared")
        {
            THEN("it is recognized as equal")
            {
                REQUIRE(true == visit(*first, ElementComparator{ *second }));
            }
        }
    }
}
