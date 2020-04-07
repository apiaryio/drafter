#include <catch2/catch.hpp>

#include "refract/VisitorUtils.h"

using namespace refract;

SCENARIO("IsLiteral identifies valid 'literal' elements", "[utils]")
{

    WHEN("have empty string")
    {
        std::unique_ptr<IElement> e = make_empty<StringElement>();

        THEN("It is not identified as literal")
        {
            REQUIRE(!IsLiteral(*e.get()));
        }
    }

    WHEN("have string with value")
    {
        std::unique_ptr<IElement> e = from_primitive("abc");

        THEN("It is not identified as literal")
        {
            REQUIRE(IsLiteral(*e.get()));
        }
    }

    WHEN("have number with value")
    {
        std::unique_ptr<IElement> e = from_primitive(3);

        THEN("It is not identified as literal")
        {
            REQUIRE(IsLiteral(*e.get()));
        }
    }

    WHEN("have empty number")
    {
        std::unique_ptr<IElement> e = make_empty<NumberElement>();

        THEN("It is not identified as literal")
        {
            REQUIRE(!IsLiteral(*e.get()));
        }
    }

    WHEN("have empty array")
    {
        std::unique_ptr<IElement> e = make_empty<ArrayElement>();

        THEN("It is not identified as literal")
        {
            REQUIRE(!IsLiteral(*e.get()));
        }
    }

    WHEN("have array with value")
    {
        std::unique_ptr<IElement> e = make_element<ArrayElement>(from_primitive("abc"));

        THEN("It is not identified as literal")
        {
            REQUIRE(!IsLiteral(*e.get()));
        }
    }

    WHEN("have empty array value fixed typeAttribute")
    {
        std::unique_ptr<IElement> e = make_element<ArrayElement>();
        e->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        THEN("It is identified as literal")
        {
            REQUIRE(IsLiteral(*e.get()));
        }
    }

    WHEN("have array with value and fixed typeAttribute")
    {
        std::unique_ptr<IElement> e = make_element<ArrayElement>(from_primitive("abc"), from_primitive(3));
        e->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        THEN("It is identified as literal")
        {
            REQUIRE(IsLiteral(*e.get()));
        }
    }

    WHEN("have NullElement")
    {
        std::unique_ptr<IElement> e = make_empty<NullElement>();

        THEN("It is not identified as literal")
        {
            REQUIRE(!IsLiteral(*e.get()));
        }
    }

    WHEN("have NullElement with fixed typeAttribute")
    {
        std::unique_ptr<IElement> e = make_empty<NullElement>();
        e->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        THEN("It is not identified as literal")
        {
            REQUIRE(!IsLiteral(*e.get()));
        }
    }
}
