//
//  test/utils/test-Variant.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 17/01/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include <catch.hpp>

#include "../Tracker.h"
#include <string>

#include "utils/Variant.h"

struct Foo : tracked<Foo> {
    int data_ = {};

    Foo() = default;
    Foo(const Foo&) = default;
    Foo(Foo&&) = default;
    Foo& operator=(const Foo&) = default;
    Foo& operator=(Foo&&) = default;
    ~Foo() = default;

    explicit Foo(int data) : data_(data) {}
};

struct Bar : tracked<Bar> {
    bool data_ = {};

    Bar() = default;
    Bar(const Bar&) = default;
    Bar(Bar&&) = default;
    Bar& operator=(const Bar&) = default;
    Bar& operator=(Bar&&) = default;
    ~Bar() = default;

    explicit Bar(bool data) : data_(data) {}
};

struct Baz : tracked<Baz> {
    std::string data_ = {};

    Baz() = default;
    Baz(const Baz&) = default;
    Baz(Baz&&) = default;
    Baz& operator=(const Baz&) = default;
    Baz& operator=(Baz&&) = default;
    ~Baz() = default;

    explicit Baz(std::string data) : data_(std::move(data)) {}
};

using namespace drafter;
using namespace utils;

using tested = variant<Foo, Bar, Baz>;

SCENARIO("variant construction", "[variant][init]")
{
    GIVEN("A default intitialized variant<Foo, Bar, Baz>")
    {
        Foo::reset_record();
        Bar::reset_record();
        Baz::reset_record();

        tested v;

        THEN("only the first variant type exists")
        {
            REQUIRE(Foo::instances().size() == 1);
            REQUIRE(Bar::instances().size() == 0);
            REQUIRE(Baz::instances().size() == 0);

            THEN("it is allocated somewhere in the variant")
            {
                using address = const std::uint8_t*;

                address v_mem_start = reinterpret_cast<address>(&v);
                address v_mem_end = v_mem_start + sizeof(v);

                address foo_mem_start = reinterpret_cast<address>(&Foo::last_instance());

                REQUIRE(foo_mem_start >= v_mem_start);
                REQUIRE(foo_mem_start < v_mem_end);
            }

            THEN("it was default constructed")
            {
                REQUIRE(Foo::record().default_constructor == 1);
            }

            THEN("it was not copied")
            {
                REQUIRE(Foo::record().copy_constructor == 0);
                REQUIRE(Foo::record().copy_assignment == 0);
            }

            THEN("it was not moved")
            {
                REQUIRE(Foo::record().move_constructor == 0);
                REQUIRE(Foo::record().move_assignment == 0);
            }

            THEN("no other variants were constructed")
            {
                REQUIRE(Bar::record().default_constructor == 0);
                REQUIRE(Bar::record().copy_constructor == 0);
                REQUIRE(Bar::record().move_constructor == 0);

                REQUIRE(Baz::record().default_constructor == 0);
                REQUIRE(Baz::record().copy_constructor == 0);
                REQUIRE(Baz::record().move_constructor == 0);
            }
        }

        THEN("the index of the variant is zero")
        {
            REQUIRE(v.index() == 0);
        }

        THEN("get<I=0> on the variant returns the only instance of Foo")
        {
            REQUIRE(&get<0>(v) == &Foo::last_instance());
        }

        THEN("get<I=1,2> on the variant throws bad_variant_access")
        {
            REQUIRE_THROWS_AS(get<1>(v), bad_variant_access);
            REQUIRE_THROWS_AS(get<2>(v), bad_variant_access);
        }

        THEN("get_if<I=0> on the variant returns the only instance of Foo")
        {
            REQUIRE(get_if<0>(v) == &Foo::last_instance());
        }

        THEN("get_if<I=1,2> on the variant returns nullptr")
        {
            REQUIRE(get_if<1>(v) == nullptr);
            REQUIRE(get_if<2>(v) == nullptr);
        }

        THEN("holds_alternative<I=0> on the variant is true")
        {
            REQUIRE(holds_alternative<0>(v));
        }

        THEN("holds_alternative<I=1,2> on the variant is false")
        {
            REQUIRE(!holds_alternative<1>(v));
            REQUIRE(!holds_alternative<2>(v));
        }
    }

    GIVEN("An in place initialized variant<Foo, Bar, Baz> of Bar")
    {
        Foo::reset_record();
        Bar::reset_record();
        Baz::reset_record();

        tested v(in_place_type<Bar>{}, true);

        THEN("one Bar exists")
        {
            REQUIRE(Foo::instances().size() == 0);
            REQUIRE(Bar::instances().size() == 1);
            REQUIRE(Baz::instances().size() == 0);
        }

        THEN("the last Bar is allocated somewhere in the variant")
        {
            using address = const std::uint8_t*;

            address v_mem_start = reinterpret_cast<address>(&v);
            address v_mem_end = v_mem_start + sizeof(v);

            address bar_mem_start = reinterpret_cast<address>(&Bar::last_instance());

            REQUIRE(bar_mem_start >= v_mem_start);
            REQUIRE(bar_mem_start < v_mem_end);
        }

        THEN("a Bar was default constructed")
        {
            REQUIRE(Bar::record().default_constructor == 1);
        }

        THEN("it was not copied")
        {
            REQUIRE(Bar::record().copy_constructor == 0);
            REQUIRE(Bar::record().copy_assignment == 0);
        }

        THEN("it was not moved")
        {
            REQUIRE(Bar::record().move_constructor == 0);
            REQUIRE(Bar::record().move_assignment == 0);
        }

        THEN("no Foo, Baz were constructed")
        {
            REQUIRE(Foo::record().default_constructor == 0);
            REQUIRE(Foo::record().copy_constructor == 0);
            REQUIRE(Foo::record().move_constructor == 0);

            REQUIRE(Baz::record().default_constructor == 0);
            REQUIRE(Baz::record().copy_constructor == 0);
            REQUIRE(Baz::record().move_constructor == 0);
        }

        THEN("the index of the variant is one")
        {
            REQUIRE(v.index() == 1);
        }

        THEN("get<I=1> on the variant returns the last instance of Bar")
        {
            REQUIRE(&get<1>(v) == &Bar::last_instance());
        }

        THEN("get<I=0,2> on the variant throws bad_variant_access")
        {
            REQUIRE_THROWS_AS(get<0>(v), bad_variant_access);
            REQUIRE_THROWS_AS(get<2>(v), bad_variant_access);
        }

        THEN("get_if<I=1> on the variant returns the last instance of Bar")
        {
            REQUIRE(get_if<1>(v) == &Bar::last_instance());
        }

        THEN("get_if<I=0,2> on the variant returns nullptr")
        {
            REQUIRE(get_if<0>(v) == nullptr);
            REQUIRE(get_if<2>(v) == nullptr);
        }

        THEN("holds_alternative<I=1> on the variant is true")
        {
            REQUIRE(holds_alternative<1>(v));
        }

        THEN("holds_alternative<I=0,2> on the variant is false")
        {
            REQUIRE(!holds_alternative<0>(v));
            REQUIRE(!holds_alternative<2>(v));
        }
    }

    GIVEN("A variant<Foo, Bar, Baz> initialized from lvalue of Bar")
    {
        Bar bar;

        Foo::reset_record();
        Bar::reset_record();
        Baz::reset_record();

        tested v(bar);

        THEN("two Bar exist")
        {
            REQUIRE(Foo::instances().size() == 0);
            REQUIRE(Bar::instances().size() == 2);
            REQUIRE(Baz::instances().size() == 0);
        }

        THEN("the last Bar is allocated somewhere in the variant")
        {
            using address = const std::uint8_t*;

            address v_mem_start = reinterpret_cast<address>(&v);
            address v_mem_end = v_mem_start + sizeof(v);

            address bar_mem_start = reinterpret_cast<address>(&Bar::last_instance());

            REQUIRE(bar_mem_start >= v_mem_start);
            REQUIRE(bar_mem_start < v_mem_end);
        }

        THEN("a Bar was copy constructed")
        {
            REQUIRE(Bar::record().copy_constructor == 1);
        }

        THEN("it was not default constructed")
        {
            REQUIRE(Bar::record().default_constructor == 0);
        }

        THEN("it was not copy assigned")
        {
            REQUIRE(Bar::record().copy_assignment == 0);
        }

        THEN("it was not moved")
        {
            REQUIRE(Bar::record().move_constructor == 0);
            REQUIRE(Bar::record().move_assignment == 0);
        }


        THEN("no Foo, Baz were constructed")
        {
            REQUIRE(Foo::record().default_constructor == 0);
            REQUIRE(Foo::record().copy_constructor == 0);
            REQUIRE(Foo::record().move_constructor == 0);

            REQUIRE(Baz::record().default_constructor == 0);
            REQUIRE(Baz::record().copy_constructor == 0);
            REQUIRE(Baz::record().move_constructor == 0);
        }

        THEN("the index of the variant is one")
        {
            REQUIRE(v.index() == 1);
        }

        THEN("get<I=1> on the variant returns the last instance of Bar")
        {
            REQUIRE(&get<1>(v) == &Bar::last_instance());
        }

        THEN("get<I=0,2> on the variant throws bad_variant_access")
        {
            REQUIRE_THROWS_AS(get<0>(v), bad_variant_access);
            REQUIRE_THROWS_AS(get<2>(v), bad_variant_access);
        }

        THEN("get_if<I=1> on the variant returns the last instance of Bar")
        {
            REQUIRE(get_if<1>(v) == &Bar::last_instance());
        }

        THEN("get_if<I=0,2> on the variant returns nullptr")
        {
            REQUIRE(get_if<0>(v) == nullptr);
            REQUIRE(get_if<2>(v) == nullptr);
        }

        THEN("holds_alternative<I=1> on the variant is true")
        {
            REQUIRE(holds_alternative<1>(v));
        }

        THEN("holds_alternative<I=0,2> on the variant is false")
        {
            REQUIRE(!holds_alternative<0>(v));
            REQUIRE(!holds_alternative<2>(v));
        }
    }

    GIVEN("A variant<Foo, Bar, Baz> initialized from rvalue of Bar")
    {
        auto create_bar = []() -> Bar {
            Bar bar{};
            Foo::reset_record();
            Bar::reset_record();
            Baz::reset_record();
            return bar;
        };

        tested v(create_bar());

        THEN("one Bar exists")
        {
            REQUIRE(Foo::instances().size() == 0);
            REQUIRE(Bar::instances().size() == 1);
            REQUIRE(Baz::instances().size() == 0);
        }

        THEN("the last Bar is allocated somewhere in the variant")
        {
            using address = const std::uint8_t*;

            address v_mem_start = reinterpret_cast<address>(&v);
            address v_mem_end = v_mem_start + sizeof(v);

            address bar_mem_start = reinterpret_cast<address>(&Bar::last_instance());

            REQUIRE(bar_mem_start >= v_mem_start);
            REQUIRE(bar_mem_start < v_mem_end);
        }

        THEN("a Bar was move constructed")
        {
            // @tjanc@ we cannot guarantee move-ellision will work with the observed move constructor; there will be
            // either one or two invocations based on that
            REQUIRE((Bar::record().move_constructor == 1 || Bar::record().move_constructor == 2));
        }

        THEN("it was not default constructed")
        {
            REQUIRE(Bar::record().default_constructor == 0);
        }

        THEN("it was not copied")
        {
            REQUIRE(Bar::record().copy_constructor == 0);
            REQUIRE(Bar::record().copy_assignment == 0);
        }

        THEN("it was not move assigned")
        {
            REQUIRE(Bar::record().move_assignment == 0);
        }

        THEN("no Foo, Baz were constructed")
        {
            REQUIRE(Foo::record().default_constructor == 0);
            REQUIRE(Foo::record().copy_constructor == 0);
            REQUIRE(Foo::record().move_constructor == 0);

            REQUIRE(Baz::record().default_constructor == 0);
            REQUIRE(Baz::record().copy_constructor == 0);
            REQUIRE(Baz::record().move_constructor == 0);
        }

        THEN("the index of the variant is one")
        {
            REQUIRE(v.index() == 1);
        }

        THEN("get<I=1> on the variant returns the last instance of Bar")
        {
            REQUIRE(&get<1>(v) == &Bar::last_instance());
        }

        THEN("get<I=0,2> on the variant throws bad_variant_access")
        {
            REQUIRE_THROWS_AS(get<0>(v), bad_variant_access);
            REQUIRE_THROWS_AS(get<2>(v), bad_variant_access);
        }

        THEN("get_if<I=1> on the variant returns the last instance of Bar")
        {
            REQUIRE(get_if<1>(v) == &Bar::last_instance());
        }

        THEN("get_if<I=0,2> on the variant returns nullptr")
        {
            REQUIRE(get_if<0>(v) == nullptr);
            REQUIRE(get_if<2>(v) == nullptr);
        }

        THEN("holds_alternative<I=1> on the variant is true")
        {
            REQUIRE(holds_alternative<1>(v));
        }

        THEN("holds_alternative<I=0,2> on the variant is false")
        {
            REQUIRE(!holds_alternative<0>(v));
            REQUIRE(!holds_alternative<2>(v));
        }
    }

    GIVEN("A variant<Foo, Bar, Baz> initialized from rvalue of variant<Foo, Bar, Baz>(Bar)")
    {
        tested bar(in_place_type<Bar>{});

        Foo::reset_record();
        Bar::reset_record();
        Baz::reset_record();

        tested v(std::move(bar));

        THEN("two Bar exist")
        {
            REQUIRE(Foo::instances().size() == 0);
            REQUIRE(Bar::instances().size() == 2);
            REQUIRE(Baz::instances().size() == 0);
        }

        THEN("the index of the variant is one")
        {
            REQUIRE(v.index() == 1);
        }

        THEN("the last Bar was allocated somewhere in the variant")
        {
            using address = const std::uint8_t*;

            address v_mem_start = reinterpret_cast<address>(&v);
            address v_mem_end = v_mem_start + sizeof(v);

            address bar_mem_start = reinterpret_cast<address>(&Bar::last_instance());

            REQUIRE(bar_mem_start >= v_mem_start);
            REQUIRE(bar_mem_start < v_mem_end);
        }

        THEN("no assignments took place")
        {
            REQUIRE(Bar::record().copy_assignment == 0);
            REQUIRE(Bar::record().move_assignment == 0);
            REQUIRE(Baz::record().copy_assignment == 0);
            REQUIRE(Baz::record().move_assignment == 0);
            REQUIRE(Foo::record().copy_assignment == 0);
            REQUIRE(Foo::record().move_assignment == 0);
        }

        THEN("no Baz was constructed")
        {
            REQUIRE(Baz::record().default_constructor == 0);
            REQUIRE(Baz::record().copy_constructor == 0);
            REQUIRE(Baz::record().move_constructor == 0);
        }

        THEN("no Foo was constructed")
        {
            REQUIRE(Foo::record().default_constructor == 0);
            REQUIRE(Foo::record().copy_constructor == 0);
            REQUIRE(Foo::record().move_constructor == 0);
        }

        THEN("a Bar was move constructed")
        {
            REQUIRE(Bar::record().default_constructor == 0);
            REQUIRE(Bar::record().copy_constructor == 0);
            REQUIRE(Bar::record().move_constructor == 1);
        }

        THEN("get<I=1> on the variant returns the last instance of Bar")
        {
            REQUIRE(&get<1>(v) == &Bar::last_instance());
        }

        THEN("get<I=0,2> on the variant throws bad_variant_access")
        {
            REQUIRE_THROWS_AS(get<0>(v), bad_variant_access);
            REQUIRE_THROWS_AS(get<2>(v), bad_variant_access);
        }

        THEN("get_if<I=1> on the variant returns the last instance of Bar")
        {
            REQUIRE(get_if<1>(v) == &Bar::last_instance());
        }

        THEN("get_if<I=0,2> on the variant returns nullptr")
        {
            REQUIRE(get_if<0>(v) == nullptr);
            REQUIRE(get_if<2>(v) == nullptr);
        }

        THEN("holds_alternative<I=1> on the variant is true")
        {
            REQUIRE(holds_alternative<1>(v));
        }

        THEN("holds_alternative<I=0,2> on the variant is false")
        {
            REQUIRE(!holds_alternative<0>(v));
            REQUIRE(!holds_alternative<2>(v));
        }
    }
}

SCENARIO("variant destruction", "[variant][destructor]")
{
    GIVEN("A default intitialized variant<Foo, Bar, Baz>")
    {
        WHEN("it is destroyed")
        {
            {
                tested v;

                Foo::reset_record();
                Bar::reset_record();
                Baz::reset_record();
            }

            THEN("Foo is destroyed once")
            {
                REQUIRE(Foo::record().destructor == 1);
                REQUIRE(Bar::record().destructor == 0);
                REQUIRE(Baz::record().destructor == 0);
            }

            THEN("no Foo, Bar, Baz is around anymore")
            {
                REQUIRE(Foo::instances().size() == 0);
                REQUIRE(Bar::instances().size() == 0);
                REQUIRE(Baz::instances().size() == 0);
            }
        }
    }

    GIVEN("A variant<Foo, Bar, Baz> constructed by move from a Bar")
    {
        Bar bar{};

        WHEN("it is destroyed")
        {
            {
                tested v(std::move(bar));

                Foo::reset_record();
                Bar::reset_record();
                Baz::reset_record();
            }

            THEN("Bar is destroyed once")
            {
                REQUIRE(Foo::record().destructor == 0);
                REQUIRE(Bar::record().destructor == 1);
                REQUIRE(Baz::record().destructor == 0);
            }

            THEN("no Foo, Baz is around anymore")
            {
                REQUIRE(Foo::instances().size() == 0);
                REQUIRE(Baz::instances().size() == 0);
            }

            THEN("one Bar is around")
            {
                REQUIRE(Bar::instances().size() == 1);

                THEN("that Bar is the original one")
                {
                    REQUIRE(&Bar::last_instance() == &bar);
                }
            }
        }
    }

    GIVEN("A variant<Foo, Bar, Baz> constructed by copy from a Bar")
    {
        Bar bar{};

        WHEN("it is destroyed")
        {
            {
                tested v(bar);

                Foo::reset_record();
                Bar::reset_record();
                Baz::reset_record();
            }

            THEN("Bar is destroyed once")
            {
                REQUIRE(Foo::record().destructor == 0);
                REQUIRE(Bar::record().destructor == 1);
                REQUIRE(Baz::record().destructor == 0);
            }

            THEN("no Foo, Baz is around anymore")
            {
                REQUIRE(Foo::instances().size() == 0);
                REQUIRE(Baz::instances().size() == 0);
            }

            THEN("one Bar is around")
            {
                REQUIRE(Bar::instances().size() == 1);

                THEN("that Bar is the original one")
                {
                    REQUIRE(&Bar::last_instance() == &bar);
                }
            }
        }
    }
}

SCENARIO("variant assignment", "[variant][assignment]")
{
    GIVEN("A variant<Foo, Bar, Baz> `v` (resp. `u`) in place constructed as a Baz (resp. `Baz`)")
    {
        tested v(in_place_type<Baz>{});
        tested u(in_place_type<Baz>{});

        WHEN("u is copy assigned to v")
        {
            Foo::reset_record();
            Bar::reset_record();
            Baz::reset_record();

            v = u;

            THEN("`v` holds a Baz")
            {
                REQUIRE(get_if<Baz>(v));
            }

            THEN("`u` holds a Baz")
            {
                REQUIRE(get_if<Baz>(u));
            }

            THEN("no Foo, Bar, Baz has been constructed")
            {
                REQUIRE(Foo::record().default_constructor == 0);
                REQUIRE(Foo::record().copy_constructor == 0);
                REQUIRE(Foo::record().move_constructor == 0);

                REQUIRE(Bar::record().default_constructor == 0);
                REQUIRE(Bar::record().copy_constructor == 0);
                REQUIRE(Bar::record().move_constructor == 0);

                REQUIRE(Baz::record().default_constructor == 0);
                REQUIRE(Baz::record().copy_constructor == 0);
                REQUIRE(Baz::record().move_constructor == 0);
            }

            THEN("copy assignment to Baz took place")
            {
                REQUIRE(Foo::record().move_assignment == 0);
                REQUIRE(Foo::record().copy_assignment == 0);

                REQUIRE(Bar::record().move_assignment == 0);
                REQUIRE(Bar::record().copy_assignment == 0);

                REQUIRE(Baz::record().move_assignment == 0);
                REQUIRE(Baz::record().copy_assignment == 1);
            }

            THEN("no Foo, Bar, Baz was destroyed")
            {
                REQUIRE(Bar::record().destructor == 0);
                REQUIRE(Foo::record().destructor == 0);
                REQUIRE(Baz::record().destructor == 0);
            }

            THEN("there is no Foo nor Bar")
            {
                REQUIRE(Foo::instances().size() == 0);
                REQUIRE(Bar::instances().size() == 0);
            }

            THEN("there are two Baz")
            {
                REQUIRE(Baz::instances().size() == 2);
            }

            THEN("the first Baz is the one in `v`")
            {
                REQUIRE(get_if<Baz>(v) == Baz::instances()[0]);
            }

            THEN("the second Baz is the one in `u`")
            {
                REQUIRE(get_if<Baz>(u) == Baz::instances()[1]);
            }
        }

        WHEN("u is move assigned to v")
        {
            Foo::reset_record();
            Bar::reset_record();
            Baz::reset_record();

            v = std::move(u);

            THEN("`v` holds a Baz")
            {
                REQUIRE(get_if<Baz>(v));
            }

            THEN("`u` holds a Baz")
            {
                REQUIRE(get_if<Baz>(u));
            }

            THEN("no Foo, Bar, Baz was constructed")
            {
                REQUIRE(Foo::record().default_constructor == 0);
                REQUIRE(Foo::record().copy_constructor == 0);
                REQUIRE(Foo::record().move_constructor == 0);

                REQUIRE(Bar::record().default_constructor == 0);
                REQUIRE(Bar::record().copy_constructor == 0);
                REQUIRE(Bar::record().move_constructor == 0);

                REQUIRE(Baz::record().default_constructor == 0);
                REQUIRE(Baz::record().copy_constructor == 0);
                REQUIRE(Baz::record().move_constructor == 0);
            }

            THEN("copy assignment to Baz took place")
            {
                REQUIRE(Foo::record().move_assignment == 0);
                REQUIRE(Foo::record().copy_assignment == 0);

                REQUIRE(Bar::record().move_assignment == 0);
                REQUIRE(Bar::record().copy_assignment == 0);

                REQUIRE(Baz::record().move_assignment == 1);
                REQUIRE(Baz::record().copy_assignment == 0);
            }

            THEN("no Foo, Bar, Baz was destroyed")
            {
                REQUIRE(Bar::record().destructor == 0);
                REQUIRE(Foo::record().destructor == 0);
                REQUIRE(Baz::record().destructor == 0);
            }

            THEN("there is no Foo nor Bar")
            {
                REQUIRE(Foo::instances().size() == 0);
                REQUIRE(Bar::instances().size() == 0);
            }

            THEN("there are two Baz")
            {
                REQUIRE(Baz::instances().size() == 2);
            }

            THEN("the first Baz is the one in `v`")
            {
                REQUIRE(get_if<Baz>(v) == Baz::instances()[0]);
            }

            THEN("the second Baz is the one in `u`")
            {
                REQUIRE(get_if<Baz>(u) == Baz::instances()[1]);
            }
        }
    }

    GIVEN("A variant<Foo, Bar, Baz> `v` (resp. `u`) in place constructed as a Bar (resp. `Baz`)")
    {
        tested v(in_place_type<Bar>{});
        tested u(in_place_type<Baz>{});

        WHEN("u is copy assigned to v")
        {
            Foo::reset_record();
            Bar::reset_record();
            Baz::reset_record();

            v = u;

            THEN("`v` holds a Baz")
            {
                REQUIRE(get_if<Baz>(v));
            }

            THEN("`u` holds a Baz")
            {
                REQUIRE(get_if<Baz>(u));
            }

            THEN("a Baz has been copy constructed")
            {
                REQUIRE(Baz::record().copy_constructor == 1);
            }

            THEN("a Bar has been destroyed")
            {
                REQUIRE(Bar::record().destructor == 1);
                REQUIRE(Foo::record().destructor == 0);
                REQUIRE(Baz::record().destructor == 0);
            }

            THEN("there is no Foo nor Bar")
            {
                REQUIRE(Foo::instances().size() == 0);
                REQUIRE(Bar::instances().size() == 0);
            }

            THEN("there are two Baz")
            {
                REQUIRE(Baz::instances().size() == 2);
            }

            THEN("the first Baz is the one in `u`")
            {
                REQUIRE(get_if<Baz>(u) == Baz::instances()[0]);
            }

            THEN("the second Baz is the one in `v`")
            {
                REQUIRE(get_if<Baz>(v) == Baz::instances()[1]);
            }
        }

        WHEN("u is move assigned to v")
        {
            Foo::reset_record();
            Bar::reset_record();
            Baz::reset_record();

            v = std::move(u);

            THEN("`v` holds a Baz")
            {
                REQUIRE(get_if<Baz>(v));
            }

            THEN("`u` holds a Baz")
            {
                REQUIRE(get_if<Baz>(u));
            }

            THEN("a Baz has been move constructed")
            {
                REQUIRE(Baz::record().move_constructor == 1);
            }

            THEN("a Bar has been destroyed")
            {
                REQUIRE(Bar::record().destructor == 1);
                REQUIRE(Foo::record().destructor == 0);
                REQUIRE(Baz::record().destructor == 0);
            }

            THEN("there is no Foo nor Bar")
            {
                REQUIRE(Foo::instances().size() == 0);
                REQUIRE(Bar::instances().size() == 0);
            }

            THEN("there are two Baz")
            {
                REQUIRE(Baz::instances().size() == 2);
            }

            THEN("the first Baz is the one in `u`")
            {
                REQUIRE(get_if<Baz>(u) == Baz::instances()[0]);
            }

            THEN("the second Baz is the one in `v`")
            {
                REQUIRE(get_if<Baz>(v) == Baz::instances()[1]);
            }
        }
    }
}
