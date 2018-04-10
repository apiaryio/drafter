#include "refract/InfoElementsUtils.h"
#include "catch.hpp"

SCENARIO("Append InfoElement", "[InfoElements][append]")
{
    using namespace refract;

    const std::string typeAttributes = "typeAttributes";
    const std::string fixed = "fixed";

     auto shouldBe = make_empty<StringElement>();
     shouldBe->attributes().set(typeAttributes, make_element<ArrayElement>(from_primitive(fixed)));

    GIVEN("Empty Element w/o typeAttributes")
    {
        auto e = make_empty<StringElement>();

        WHEN("I append to 'typeAttributes' 'fixed'")
        {
            AppendInfoElement<ArrayElement>(e->attributes(), typeAttributes, dsd::String{ fixed });

            THEN("then it should 'typeAttributes.fixed'")
            {
                REQUIRE(*e.get() == *shouldBe.get());
            }

        }
    }

    GIVEN("Empty Element w/ empty typeAttributes")
    {
        auto e = make_empty<StringElement>();
        e->attributes().set(typeAttributes, make_empty<ArrayElement>());

        WHEN("I append to 'typeAttributes' 'fixed'")
        {
            AppendInfoElement<ArrayElement>(e->attributes(), typeAttributes, dsd::String{ fixed });

            THEN("then it should 'typeAttributes.fixed'")
            {
                REQUIRE(*e.get() == *shouldBe.get());
            }

        }
    }

    GIVEN("Empty Element w/ set 'typeAttributes.fixed'")
    {
        auto e = make_empty<StringElement>();
        e->attributes().set(typeAttributes, make_element<ArrayElement>(from_primitive(fixed)));

        WHEN("I append to 'typeAttributes' 'fixed'")
        {
            AppendInfoElement<ArrayElement>(e->attributes(), typeAttributes, dsd::String{ fixed });

            THEN("then it should 'typeAttributes.fixed'")
            {
                REQUIRE(*e.get() == *shouldBe.get());
            }

        }

    GIVEN("Empty Element w/ set 'typeAttributes.required'")
    {
        auto e = make_empty<StringElement>();
        e->attributes().set(typeAttributes, make_element<ArrayElement>(from_primitive("required")));

        WHEN("I append to 'typeAttributes' 'fixed'")
        {
            AppendInfoElement<ArrayElement>(e->attributes(), typeAttributes, dsd::String{ fixed });

            THEN("then it should 'typeAttributes.fixed' and typeAttributes.required")
            {
                auto s = make_empty<StringElement>();
                s->attributes().set(typeAttributes, make_element<ArrayElement>(from_primitive("required"), from_primitive(fixed)));
                REQUIRE(*e.get() == *s.get());
            }

        }
    }
    }

}
