#include "refract/ElementSize.h"
#include "refract/Element.h"
#include <catch2/catch.hpp>

namespace refract
{
    std::ostream& operator<<(std::ostream& out, const cardinal& obj)
    {
        if (finite(obj))
            out << '<' << obj.data() << '>';
        else
            out << "<inf>";
        return out;
    }
}

using namespace refract;

SCENARIO("Cardinals of empty types", "[apie][sizeOf]")
{
    GIVEN("an empty String Element")
    {
        const auto tested = make_empty<StringElement>();

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is infinite")
            {
                REQUIRE(!finite(result));
            }
        }
    }

    GIVEN("an empty Number Element")
    {
        const auto tested = make_empty<NumberElement>();

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is infinite")
            {
                REQUIRE(!finite(result));
            }
        }
    }

    GIVEN("an empty Boolean Element")
    {
        const auto tested = make_empty<BooleanElement>();

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is equal to the 2nd cardinal")
            {
                REQUIRE(result == cardinal{ 2 });
            }
        }
    }

    GIVEN("a Null Element")
    {
        const auto tested = make_empty<NullElement>();

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is equal to the 1st cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }

    GIVEN("an empty Array Element")
    {
        const auto tested = make_empty<ArrayElement>();

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is infinite")
            {
                REQUIRE(!finite(result));
            }
        }
    }

    GIVEN("an empty Object Element")
    {
        const auto tested = make_empty<ObjectElement>();

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is infinite")
            {
                REQUIRE(!finite(result));
            }
        }
    }
}

SCENARIO("Cardinals of empty, nullable types", "[apie][sizeOf]")
{
    GIVEN("a nullable, empty String Element")
    {
        const auto tested = make_empty<StringElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("nullable")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is infinite")
            {
                REQUIRE(!finite(result));
            }
        }
    }

    GIVEN("a nullable, empty Number Element")
    {
        const auto tested = make_empty<NumberElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("nullable")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is infinite")
            {
                REQUIRE(!finite(result));
            }
        }
    }

    GIVEN("a nullable, empty Boolean Element")
    {
        const auto tested = make_empty<BooleanElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("nullable")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is equal to the 3rd cardinal")
            {
                REQUIRE(result == cardinal{ 3 });
            }
        }
    }

    GIVEN("a nullable, empty Array Element")
    {
        const auto tested = make_empty<ArrayElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("nullable")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is infinite")
            {
                REQUIRE(!finite(result));
            }
        }
    }

    GIVEN("a nullable, empty Object Element")
    {
        const auto tested = make_empty<ObjectElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("nullable")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is infinite")
            {
                REQUIRE(!finite(result));
            }
        }
    }
}

SCENARIO("Cardinals of nullable, fixed, empty types", "[apie][sizeOf]")
{
    GIVEN("a nullable, empty, fixed String Element")
    {
        const auto tested = make_empty<StringElement>();
        tested->attributes().set("typeAttributes",
            make_element<ArrayElement>( //
                from_primitive("fixed"),
                from_primitive("nullable")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is the 2nd cardinal")
            {
                REQUIRE(result == cardinal{ 2 });
            }
        }
    }

    GIVEN("a nullable, empty, fixed Number Element")
    {
        const auto tested = make_empty<NumberElement>();
        tested->attributes().set("typeAttributes",
            make_element<ArrayElement>( //
                from_primitive("fixed"),
                from_primitive("nullable")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is the 2nd cardinal")
            {
                REQUIRE(result == cardinal{ 2 });
            }
        }
    }

    GIVEN("a nullable, empty, fixed Boolean Element")
    {
        const auto tested = make_empty<BooleanElement>();
        tested->attributes().set("typeAttributes",
            make_element<ArrayElement>( //
                from_primitive("fixed"),
                from_primitive("nullable")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is the 2nd cardinal")
            {
                REQUIRE(result == cardinal{ 2 });
            }
        }
    }

    GIVEN("a nullable, empty, fixed Array Element")
    {
        const auto tested = make_empty<ArrayElement>();
        tested->attributes().set("typeAttributes",
            make_element<ArrayElement>( //
                from_primitive("fixed"),
                from_primitive("nullable")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is the 2nd cardinal")
            {
                REQUIRE(result == cardinal{ 2 });
            }
        }
    }

    GIVEN("a nullable, empty, fixed Object Element")
    {
        const auto tested = make_empty<ObjectElement>();
        tested->attributes().set("typeAttributes",
            make_element<ArrayElement>( //
                from_primitive("fixed"),
                from_primitive("nullable")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is the 2nd cardinal")
            {
                REQUIRE(result == cardinal{ 2 });
            }
        }
    }
}

SCENARIO("Cardinals of fixed, empty types", "[apie][sizeOf]")
{
    GIVEN("an empty, fixed String Element")
    {
        const auto tested = make_empty<StringElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is the 1st cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }

    GIVEN("an empty, fixed Number Element")
    {
        const auto tested = make_empty<NumberElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is the 1st cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }

    GIVEN("an empty, fixed Boolean Element")
    {
        const auto tested = make_empty<BooleanElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is equal to the 2nd cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }

    GIVEN("an empty, fixed Array Element")
    {
        const auto tested = make_empty<ArrayElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is the 1st cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }

    GIVEN("an empty, fixed Object Element")
    {
        const auto tested = make_empty<ObjectElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is the 1st cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }
}

SCENARIO("Cardinals of types with content", "[apie][sizeOf]")
{
    GIVEN("a String Element with some content")
    {
        const auto tested = make_element<StringElement>("Persuit");

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is infinite")
            {
                REQUIRE(!finite(result));
            }
        }
    }

    GIVEN("a Number Element with some content")
    {
        const auto tested = make_element<NumberElement>(101);

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is infinite")
            {
                REQUIRE(!finite(result));
            }
        }
    }

    GIVEN("a Boolean Element with some content")
    {
        const auto tested = make_element<BooleanElement>(true);

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is equal to the 2nd cardinal")
            {
                REQUIRE(result == cardinal{ 2 });
            }
        }
    }

    GIVEN("an Array Element with some content")
    {
        const auto tested = make_element<ArrayElement>(from_primitive(3), from_primitive("hey"));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is infinite")
            {
                REQUIRE(!finite(result));
            }
        }
    }

    GIVEN("an Object Element with some content")
    {
        const auto tested = make_element<ObjectElement>(             //
            make_element<MemberElement>("no", from_primitive(true)), //
            make_element<MemberElement>("*", from_primitive(42)));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is infinite")
            {
                REQUIRE(!finite(result));
            }
        }
    }
}

SCENARIO("Cardinals of fixed types with content", "[apie][sizeOf]")
{
    GIVEN("a fixed String Element with some content")
    {
        const auto tested = make_element<StringElement>("Persuit");
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is the 1st cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }

    GIVEN("a fixed Number Element with some content")
    {
        const auto tested = make_element<NumberElement>(101);
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is the 1st cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }

    GIVEN("a fixed Boolean Element with some content")
    {
        const auto tested = make_element<BooleanElement>(true);
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is equal to the 2nd cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }

    GIVEN("a fixed Array Element with some content")
    {
        // 2 values: does not inherit fixed; {true, false}
        auto entry1 = make_empty<BooleanElement>();

        // 1 value: fixed by itself; {42}
        auto entry2 = from_primitive(42);
        entry2->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        // 1 values: inherits fixed: {42}
        auto entry3 = from_primitive(42);

        // 3 values: does not inherit fixed and is nullable; {null, true, false}
        auto entry4 = make_empty<BooleanElement>();
        entry4->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("nullable")));

        // 2 values: inherits fixed and is nullable; {null, true}
        auto entry5 = from_primitive(true);
        entry5->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("nullable")));

        const auto tested = make_element<ArrayElement>( //
            std::move(entry1),                          //
            std::move(entry2),                          //
            std::move(entry3),                          //
            std::move(entry4),                          //
            std::move(entry5)                           //
        );
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is the 12th cardinal")
            {
                REQUIRE(result == cardinal{ 12 });
            }
        }
    }

    GIVEN("a fixed Object Element with some content")
    {
        const auto tested = make_element<ObjectElement>(             //
            make_element<MemberElement>("no", from_primitive(true)), //
            make_element<MemberElement>("*", from_primitive(42)));
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is the 1st cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }
}

SCENARIO("Cardinals of fixedType, empty types", "[apie][sizeOf]")
{
    GIVEN("an empty, fixedType Array Element")
    {
        const auto tested = make_empty<ArrayElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixedType")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is the empty cardinal")
            {
                REQUIRE(result == cardinal::empty());
            }
        }
    }

    GIVEN("an empty, fixedType Object Element")
    {
        const auto tested = make_empty<ObjectElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixedType")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is the 1st cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }
}

SCENARIO("Cardinals of fixedType types with content", "[apie][sizeOf]")
{
    GIVEN("a fixedType Array Element with some content")
    {
        const auto tested = make_element<ArrayElement>(make_element<NullElement>(), from_primitive(33));
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixedType")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is infinite")
            {
                REQUIRE(!finite(result));
            }
        }
    }

    GIVEN("a fixedType Object Element with some content")
    {
        const auto tested = make_element<ObjectElement>(                     //
            make_element<MemberElement>("no", make_empty<BooleanElement>()), // 2 values
            make_element<MemberElement>("yes", from_primitive(true)),        // 2 values
            make_element<MemberElement>("nah", from_primitive(false))        // 2 values
        );
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixedType")));

        WHEN("its cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is the 8th cardinal")
            {
                REQUIRE(result == cardinal{ 8 });
            }
        }
    }
}
