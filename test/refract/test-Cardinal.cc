#include "refract/Cardinal.h"
#include <catch2/catch.hpp>

using namespace refract;

SCENARIO("Representations of selected cardinals", "[utils][sizeOf]")
{
    GIVEN("the empty cardinal")
    {
        const auto tested = cardinal::empty();

        WHEN("it is tested to be finite")
        {
            THEN("the result is true")
            {
                REQUIRE(finite(tested));
            }
        }

        WHEN("it is tested for equality with another empty cardinal")
        {
            const auto other = cardinal::empty();
            THEN("they are equal")
            {
                REQUIRE(tested == other);
                REQUIRE(other == tested);
            }
        }

        WHEN("it is tested for equality with a different finite cardinal")
        {
            const cardinal other{ 3 };
            THEN("they are not equal")
            {
                REQUIRE(!(tested == other));
                REQUIRE(!(other == tested));
            }
        }

        WHEN("it is tested for equality with an infinite cardinal")
        {
            const auto other = cardinal::open();
            THEN("they are not equal")
            {
                REQUIRE(!(tested == other));
                REQUIRE(!(other == tested));
            }
        }

        WHEN("it is tested for inequality with another empty cardinal")
        {
            const auto other = cardinal::empty();
            THEN("they are equal")
            {
                REQUIRE(!(tested != other));
                REQUIRE(!(other != tested));
            }
        }

        WHEN("it is tested for inequality with a finite cardinal")
        {
            const cardinal other{ 3 };
            THEN("they are not equal")
            {
                REQUIRE(tested != other);
                REQUIRE(other != tested);
            }
        }

        WHEN("it is tested for inequality with an infinite cardinal")
        {
            const auto other = cardinal::open();
            THEN("they are not equal")
            {
                REQUIRE(tested != other);
                REQUIRE(other != tested);
            }
        }

        WHEN("it is added to another empty cardinal")
        {
            const auto summand = cardinal::empty();
            const auto result = summand + tested;

            THEN("the result is equal to the other summand")
            {
                REQUIRE(result == summand);
            }
        }

        WHEN("it is added to a finite cardinal")
        {
            const auto summand = cardinal{ 3 };
            const auto result = summand + tested;

            THEN("the result is equal to the other summand")
            {
                REQUIRE(result == summand);
            }
        }

        WHEN("it is added to an infinite cardinal")
        {
            const auto summand = cardinal::open();
            const auto result = summand + tested;

            THEN("the result is equal to the other summand")
            {
                REQUIRE(result == summand);
            }
        }

        WHEN("it is multiplied by another empty cardinal")
        {
            const auto summand = cardinal::empty();
            const auto result = summand * tested;

            THEN("the result is equal to the empty cardinal")
            {
                REQUIRE(result == cardinal::empty());
            }
        }

        WHEN("it is multiplied by a finite cardinal")
        {
            const auto summand = cardinal{ 3 };
            const auto result = summand * tested;

            THEN("the result is equal to the empty cardinal")
            {
                REQUIRE(result == cardinal::empty());
            }
        }

        WHEN("it is multiplied by an infinite cardinal")
        {
            const auto summand = cardinal::open();
            const auto result = summand * tested;

            THEN("the result is equal to the empty cardinal")
            {
                REQUIRE(result == cardinal::empty());
            }
        }

        WHEN("its representation is queried")
        {
            const auto result = tested.data();

            THEN("it is equal to 0")
            {
                REQUIRE(result == 0);
            }
        }
    }

    GIVEN("the finite cardinal 42")
    {
        const auto tested = cardinal{ 42 };

        WHEN("it is tested to be finite")
        {
            THEN("the result is true")
            {
                REQUIRE(finite(tested));
            }
        }

        WHEN("it is tested for equality with the empty cardinal")
        {
            const auto other = cardinal::empty();
            THEN("they are not equal")
            {
                REQUIRE(!(tested == other));
                REQUIRE(!(other == tested));
            }
        }

        WHEN("it is tested for equality with a different finite cardinal")
        {
            const cardinal other{ 3 };
            THEN("they are not equal")
            {
                REQUIRE(!(tested == other));
                REQUIRE(!(other == tested));
            }
        }

        WHEN("it is tested for equality with the same finite cardinal")
        {
            const cardinal other{ 42 };
            THEN("they are equal")
            {
                REQUIRE(tested == other);
                REQUIRE(other == tested);
            }
        }

        WHEN("it is tested for equality with an infinite cardinal")
        {
            const auto other = cardinal::open();
            THEN("they are not equal")
            {
                REQUIRE(!(tested == other));
                REQUIRE(!(other == tested));
            }
        }

        WHEN("it is tested for inequality with an empty cardinal")
        {
            const auto other = cardinal::empty();
            THEN("they are not equal")
            {
                REQUIRE(tested != other);
                REQUIRE(other != tested);
            }
        }

        WHEN("it is tested for inequality with a different finite cardinal")
        {
            const cardinal other{ 3 };
            THEN("they are not equal")
            {
                REQUIRE(tested != other);
                REQUIRE(other != tested);
            }
        }

        WHEN("it is tested for inequality with the same finite cardinal")
        {
            const cardinal other{ 42 };
            THEN("they are equal")
            {
                REQUIRE(!(tested != other));
                REQUIRE(!(other != tested));
            }
        }

        WHEN("it is tested for inequality with an infinite cardinal")
        {
            const auto other = cardinal::open();
            THEN("they are not equal")
            {
                REQUIRE(tested != other);
                REQUIRE(other != tested);
            }
        }

        WHEN("it is added to another empty cardinal")
        {
            const auto summand = cardinal::empty();
            const auto result = summand + tested;

            THEN("the result is equal to the original")
            {
                REQUIRE(result == tested);
            }
        }

        WHEN("it is added to a different finite cardinal")
        {
            const auto summand = cardinal{ 3 };
            const auto result = summand + tested;

            THEN("the result is equal to the other summand")
            {
                REQUIRE(result == cardinal{ 45 });
            }
        }

        WHEN("it is added to an infinite cardinal")
        {
            const auto summand = cardinal::open();
            const auto result = summand + tested;

            THEN("the result is also infinite")
            {
                REQUIRE(result == cardinal::open());
            }
        }

        WHEN("it is multiplied by an empty cardinal")
        {
            const auto summand = cardinal::empty();
            const auto result = summand * tested;

            THEN("the result is equal to the empty cardinal")
            {
                REQUIRE(result == cardinal::empty());
            }
        }

        WHEN("it is multiplied by another finite cardinal")
        {
            const auto summand = cardinal{ 3 };
            const auto result = summand * tested;

            THEN("the result is equal to the product")
            {
                REQUIRE(result == cardinal{ 3 * 42 });
            }
        }

        WHEN("it is multiplied by an infinite cardinal")
        {
            const auto summand = cardinal::open();
            const auto result = summand * tested;

            THEN("the result is equal to an infinite cardinal")
            {
                REQUIRE(result == cardinal::open());
            }
        }

        WHEN("its representation is queried")
        {
            const auto result = tested.data();

            THEN("it is equal to 42")
            {
                REQUIRE(result == 42);
            }
        }
    }

    GIVEN("the open cardinal")
    {
        const auto tested = cardinal::open();

        WHEN("it is tested to be finite")
        {
            THEN("the result is false")
            {
                REQUIRE(!finite(tested));
            }
        }

        WHEN("it is tested for equality with the empty cardinal")
        {
            const auto other = cardinal::empty();
            THEN("they are not equal")
            {
                REQUIRE(!(tested == other));
                REQUIRE(!(other == tested));
            }
        }

        WHEN("it is tested for equality with a finite cardinal")
        {
            const cardinal other{ 3 };
            THEN("they are not equal")
            {
                REQUIRE(!(tested == other));
                REQUIRE(!(other == tested));
            }
        }

        WHEN("it is tested for equality with the open cardinal")
        {
            const auto other = cardinal::open();
            THEN("they are equal")
            {
                REQUIRE(tested == other);
                REQUIRE(other == tested);
            }
        }

        WHEN("it is tested for inequality with an empty cardinal")
        {
            const auto other = cardinal::empty();
            THEN("they are not equal")
            {
                REQUIRE(tested != other);
                REQUIRE(other != tested);
            }
        }

        WHEN("it is tested for inequality with a finite cardinal")
        {
            const cardinal other{ 3 };
            THEN("they are not equal")
            {
                REQUIRE(tested != other);
                REQUIRE(other != tested);
            }
        }

        WHEN("it is tested for inequality with the open cardinal")
        {
            const auto other = cardinal::open();
            THEN("they are not equal")
            {
                REQUIRE(!(tested != other));
                REQUIRE(!(other != tested));
            }
        }

        WHEN("it is added to another empty cardinal")
        {
            const auto summand = cardinal::empty();
            const auto result = summand + tested;

            THEN("the result is equal to the original")
            {
                REQUIRE(result == tested);
            }
        }

        WHEN("it is added to a finite cardinal")
        {
            const auto summand = cardinal{ 3 };
            const auto result = summand + tested;

            THEN("the result is equal to the original")
            {
                REQUIRE(result == tested);
            }
        }

        WHEN("it is added to an infinite cardinal")
        {
            const auto summand = cardinal::open();
            const auto result = summand + tested;

            THEN("the result is also infinite")
            {
                REQUIRE(result == cardinal::open());
            }
        }

        WHEN("it is multiplied by an empty cardinal")
        {
            const auto summand = cardinal::empty();
            const auto result = summand * tested;

            THEN("the result is equal to the empty cardinal")
            {
                REQUIRE(result == cardinal::empty());
            }
        }

        WHEN("it is multiplied by a finite cardinal")
        {
            const auto summand = cardinal{ 3 };
            const auto result = summand * tested;

            THEN("the result is equal to the open cardinal")
            {
                REQUIRE(result == tested);
            }
        }

        WHEN("it is multiplied by an infinite cardinal")
        {
            const auto summand = cardinal::open();
            const auto result = summand * tested;

            THEN("the result is equal to the open cardinal")
            {
                REQUIRE(result == cardinal::open());
            }
        }

        WHEN("its representation is queried")
        {
            const auto result = tested.data();

            THEN("it is equal to the largest unsigned integer available")
            {
                REQUIRE(result == std::numeric_limits<unsigned>::max());
            }
        }
    }
}

SCENARIO("Cardinal construction", "[apie][sizeOf]")
{
    GIVEN("a default constructed cardinal")
    {
        const cardinal tested;

        THEN("it is equal to the empty cardinal")
        {
            REQUIRE(tested == cardinal::empty());
        }
    }

    GIVEN("a cardinal constructed from an integer")
    {
        const cardinal tested{ 42 };

        THEN("it's data is equal to the passed integer")
        {
            REQUIRE(tested.data() == 42);
        }
    }

    GIVEN("a cardinal constructed from the integer zero")
    {
        const cardinal tested{ 0 };

        THEN("it is equal to the empty cardinal")
        {
            REQUIRE(tested == cardinal::empty());
        }
    }

    GIVEN("a finite cardinal")
    {
        const cardinal original{ 3 };

        WHEN("from it another cardinal is copy constructed")
        {
            const cardinal tested{ original };

            THEN("they are equal")
            {
                REQUIRE(tested == original);
            }
        }
    }
}
