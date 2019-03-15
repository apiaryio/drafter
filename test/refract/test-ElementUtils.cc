#include "refract/ElementUtils.h"
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
    GIVEN("Empty String Element")
    {
        const auto tested = make_empty<StringElement>();

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is an infinite cardinal")
            {
                REQUIRE(!finite(result));
            }

            THEN("it is the open cardinal")
            {
                REQUIRE(result == cardinal::open());
            }
        }
    }

    GIVEN("Empty Number Element")
    {
        const auto tested = make_empty<NumberElement>();

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is an infinite cardinal")
            {
                REQUIRE(!finite(result));
            }

            THEN("it is the open cardinal")
            {
                REQUIRE(result == cardinal::open());
            }
        }
    }

    GIVEN("Empty Boolean Element")
    {
        const auto tested = make_empty<BooleanElement>();

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is equal to the 2nd cardinal")
            {
                REQUIRE(result == cardinal{ 2 });
            }
        }
    }

    GIVEN("Null Element")
    {
        const auto tested = make_empty<NullElement>();

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is equal to the 1st cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }

    GIVEN("Empty Array Element")
    {
        const auto tested = make_empty<ArrayElement>();

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is an infinite cardinal")
            {
                REQUIRE(!finite(result));
            }

            THEN("it is the open cardinal")
            {
                REQUIRE(result == cardinal::open());
            }
        }
    }

    GIVEN("Empty Object Element")
    {
        const auto tested = make_empty<ObjectElement>();

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is an infinite cardinal")
            {
                REQUIRE(!finite(result));
            }

            THEN("it is the open cardinal")
            {
                REQUIRE(result == cardinal::open());
            }
        }
    }
}

SCENARIO("Cardinals of empty, nullable types", "[apie][sizeOf]")
{
    GIVEN("Nullable, empty String Element")
    {
        const auto tested = make_empty<StringElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("nullable")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is an infinite cardinal")
            {
                REQUIRE(!finite(result));
            }

            THEN("it is the open cardinal")
            {
                REQUIRE(result == cardinal::open());
            }
        }
    }

    GIVEN("Nullable, empty Number Element")
    {
        const auto tested = make_empty<NumberElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("nullable")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is an infinite cardinal")
            {
                REQUIRE(!finite(result));
            }

            THEN("it is the open cardinal")
            {
                REQUIRE(result == cardinal::open());
            }
        }
    }

    GIVEN("Nullable, empty Boolean Element")
    {
        const auto tested = make_empty<BooleanElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("nullable")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is equal to the 3rd cardinal")
            {
                REQUIRE(result == cardinal{ 3 });
            }
        }
    }

    GIVEN("Nullable, empty Array Element")
    {
        const auto tested = make_empty<ArrayElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("nullable")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is an infinite cardinal")
            {
                REQUIRE(!finite(result));
            }

            THEN("it is the open cardinal")
            {
                REQUIRE(result == cardinal::open());
            }
        }
    }

    GIVEN("Nullable, empty Object Element")
    {
        const auto tested = make_empty<ObjectElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("nullable")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is an infinite cardinal")
            {
                REQUIRE(!finite(result));
            }

            THEN("it is the open cardinal")
            {
                REQUIRE(result == cardinal::open());
            }
        }
    }
}

SCENARIO("Cardinals of nullable, fixed, empty types", "[apie][sizeOf]")
{
    GIVEN("Nullable, empty, fixed String Element")
    {
        const auto tested = make_empty<StringElement>();
        tested->attributes().set("typeAttributes",
            make_element<ArrayElement>( //
                from_primitive("fixed"),
                from_primitive("nullable")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is the 2nd cardinal")
            {
                REQUIRE(result == cardinal{ 2 });
            }
        }
    }

    GIVEN("Nullable, empty, fixed Number Element")
    {
        const auto tested = make_empty<NumberElement>();
        tested->attributes().set("typeAttributes",
            make_element<ArrayElement>( //
                from_primitive("fixed"),
                from_primitive("nullable")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is the 2nd cardinal")
            {
                REQUIRE(result == cardinal{ 2 });
            }
        }
    }

    GIVEN("Nullable, empty, fixed Boolean Element")
    {
        const auto tested = make_empty<BooleanElement>();
        tested->attributes().set("typeAttributes",
            make_element<ArrayElement>( //
                from_primitive("fixed"),
                from_primitive("nullable")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is the 2nd cardinal")
            {
                REQUIRE(result == cardinal{ 2 });
            }
        }
    }

    GIVEN("Nullable, empty, fixed Array Element")
    {
        const auto tested = make_empty<ArrayElement>();
        tested->attributes().set("typeAttributes",
            make_element<ArrayElement>( //
                from_primitive("fixed"),
                from_primitive("nullable")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is the 2nd cardinal")
            {
                REQUIRE(result == cardinal{ 2 });
            }
        }
    }

    GIVEN("Nullable, empty, fixed Object Element")
    {
        const auto tested = make_empty<ObjectElement>();
        tested->attributes().set("typeAttributes",
            make_element<ArrayElement>( //
                from_primitive("fixed"),
                from_primitive("nullable")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is the 2nd cardinal")
            {
                REQUIRE(result == cardinal{ 2 });
            }
        }
    }
}

SCENARIO("Cardinals of fixed, empty types", "[apie][sizeOf]")
{
    GIVEN("Empty, fixed String Element")
    {
        const auto tested = make_empty<StringElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is the 1st cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }

    GIVEN("Empty, fixed Number Element")
    {
        const auto tested = make_empty<NumberElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is the 1st cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }

    GIVEN("Empty, fixed Boolean Element")
    {
        const auto tested = make_empty<BooleanElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is equal to the 2nd cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }

    GIVEN("Empty, fixed Array Element")
    {
        const auto tested = make_empty<ArrayElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is the 1st cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }

    GIVEN("Empty, fixed Object Element")
    {
        const auto tested = make_empty<ObjectElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is the 1st cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }
}

SCENARIO("Cardinals of types with content", "[apie][sizeOf]")
{
    GIVEN("String Element with some content")
    {
        const auto tested = make_element<StringElement>("Persuit");

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is an infinite cardinal")
            {
                REQUIRE(!finite(result));
            }

            THEN("it is the open cardinal")
            {
                REQUIRE(result == cardinal::open());
            }
        }
    }

    GIVEN("Number Element with some content")
    {
        const auto tested = make_element<NumberElement>(101);

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is an infinite cardinal")
            {
                REQUIRE(!finite(result));
            }

            THEN("it is the open cardinal")
            {
                REQUIRE(result == cardinal::open());
            }
        }
    }

    GIVEN("Boolean Element with some content")
    {
        const auto tested = make_element<BooleanElement>(true);

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is equal to the 2nd cardinal")
            {
                REQUIRE(result == cardinal{ 2 });
            }
        }
    }

    GIVEN("Array Element with some content")
    {
        const auto tested = make_element<ArrayElement>(from_primitive(3), from_primitive("hey"));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is an infinite cardinal")
            {
                REQUIRE(!finite(result));
            }

            THEN("it is the open cardinal")
            {
                REQUIRE(result == cardinal::open());
            }
        }
    }

    GIVEN("Object Element with some content")
    {
        const auto tested = make_element<ObjectElement>(             //
            make_element<MemberElement>("no", from_primitive(true)), //
            make_element<MemberElement>("*", from_primitive(42)));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is an infinite cardinal")
            {
                REQUIRE(!finite(result));
            }

            THEN("it is the open cardinal")
            {
                REQUIRE(result == cardinal::open());
            }
        }
    }
}

SCENARIO("Cardinals of fixed types with content", "[apie][sizeOf]")
{
    GIVEN("Fixed String Element with some content")
    {
        const auto tested = make_element<StringElement>("Persuit");
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is the 1st cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }

    GIVEN("Fixed Number Element with some content")
    {
        const auto tested = make_element<NumberElement>(101);
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is the 1st cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }

    GIVEN("Fixed Boolean Element with some content")
    {
        const auto tested = make_element<BooleanElement>(true);
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is equal to the 2nd cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }

    GIVEN("Fixed Array Element with some content")
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

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is the 12th cardinal")
            {
                REQUIRE(result == cardinal{ 12 });
            }
        }
    }

    GIVEN("Fixed Object Element with some content")
    {
        const auto tested = make_element<ObjectElement>(             //
            make_element<MemberElement>("no", from_primitive(true)), //
            make_element<MemberElement>("*", from_primitive(42)));
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is the 1st cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }
}

SCENARIO("Cardinals of fixedType, empty types", "[apie][sizeOf]")
{
    GIVEN("Empty, fixedType Array Element")
    {
        const auto tested = make_empty<ArrayElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixedType")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is the empty cardinal")
            {
                REQUIRE(result == cardinal::empty());
            }
        }
    }

    GIVEN("Empty, fixedType Object Element")
    {
        const auto tested = make_empty<ObjectElement>();
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixedType")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is the 1st cardinal")
            {
                REQUIRE(result == cardinal{ 1 });
            }
        }
    }
}

SCENARIO("Cardinals of fixedType types with content", "[apie][sizeOf]")
{
    GIVEN("FixedType Array Element with some content")
    {
        const auto tested = make_element<ArrayElement>(make_element<NullElement>(), from_primitive(33));
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixedType")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is an infinite cardinal")
            {
                REQUIRE(!finite(result));
            }

            THEN("it is the open cardinal")
            {
                REQUIRE(result == cardinal::open());
            }
        }
    }

    GIVEN("FixedType Object Element with some content")
    {
        const auto tested = make_element<ObjectElement>(                     //
            make_element<MemberElement>("no", make_empty<BooleanElement>()), // 2 values
            make_element<MemberElement>("yes", from_primitive(true)),        // 2 values
            make_element<MemberElement>("nah", from_primitive(false))        // 2 values
        );
        tested->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixedType")));

        WHEN("it's cardinality is computed")
        {
            const cardinal result = sizeOf(*tested);

            THEN("it is a finite cardinal")
            {
                REQUIRE(finite(result));
            }

            THEN("it is the 8th cardinal")
            {
                REQUIRE(result == cardinal{ 8 });
            }
        }
    }
}
