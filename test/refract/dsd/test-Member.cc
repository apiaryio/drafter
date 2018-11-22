//
//  test/refract/dsd/test-Member.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include <catch2/catch.hpp>

#include "refract/dsd/Member.h"

#include "refract/Element.h"

using namespace refract;
using namespace dsd;

TEST_CASE("`Member`'s default element name is `member`", "[Element][Member]")
{
    REQUIRE(std::string(Member::name) == "member");
}

SCENARIO("`Member` is default constructed and both copy- and move constructed from", "[ElementData][Member]")
{
    GIVEN("A default initialized Member")
    {
        Member member;

        THEN("its key and value are nullptr")
        {
            REQUIRE(member.key() == nullptr);
            REQUIRE(member.value() == nullptr);
        }

        WHEN("from it another Member is copy constructed")
        {
            Member member2(member);

            THEN("the latter Member's key and value are also nullptr")
            {
                REQUIRE(member2.key() == nullptr);
                REQUIRE(member2.value() == nullptr);
            }
        }

        WHEN("from it another Member is move constructed")
        {
            Member member2(std::move(member));

            THEN("the latter Member's key and value are also nullptr")
            {
                REQUIRE(member2.key() == nullptr);
                REQUIRE(member2.value() == nullptr);
            }
        }
    }
}

SCENARIO("Member is constructed from values, both copy- and move constructed from and all its copies are destroyed",
    "[ElementData][Member]")
{
    GIVEN("A std::string key and a number element value")
    {
        std::string key = "eigenvalue";
        auto mock = from_primitive(42);
        const auto* mockPtr = mock.get();

        WHEN("a Member is constructed using them")
        {
            Member member(key, std::move(mock));

            THEN("its key is not nullptr")
            {
                REQUIRE(member.key());
            }
            THEN("its element obtained by key() holds a StringElement with the key")
            {
                auto strValue = dynamic_cast<const StringElement*>(member.key());
                REQUIRE(strValue);
                REQUIRE(key == strValue->get());
            }
            THEN("its element obtained by value() is the mock")
            {
                auto value = dynamic_cast<const NumberElement*>(member.value());
                REQUIRE(value == mockPtr);
            }
        }
    }

    GIVEN("A Member with a StringElement key and an NumberElement value")
    {
        std::string key = "eigenvalue";
        auto mock = from_primitive(42);
        const auto* mockPtr = mock.get();

        Member member(key, std::move(mock));

        WHEN("another Member is copy constructed from it")
        {
            Member member2(member);

            THEN("its key and value differ from the original")
            {
                REQUIRE(member.key() != member2.key());
                REQUIRE(member.value() != member2.value());
            }

            THEN("its key is a StringElement containing a copy of the original key string")
            {
                auto strValue = dynamic_cast<const StringElement*>(member2.key());
                REQUIRE(strValue);
                REQUIRE(key == strValue->get());
            }
        }

        WHEN("another Member is move constructed from it")
        {
            const auto* memberKey = member.key();
            const auto* memberValue = member.value();

            Member member2(std::move(member));

            THEN("the original's key and value are now nullptr")
            {
                REQUIRE(member.key() == nullptr);
                REQUIRE(member.value() == nullptr);
            }

            THEN("its key and value are equal to the original key and value")
            {
                REQUIRE(member2.key() == memberKey);
                REQUIRE(member2.value() == memberValue);
            }
        }
    }
}

SCENARIO("`Member` is modified", "[ElementData][Member]")
{
    GIVEN("A Member with a StringElement key and a number element value")
    {
        std::string key = "eigenvalue";
        auto mock = from_primitive(42);
        const auto* mockPtr = mock.get();

        Member member(key, std::move(mock));

        WHEN("its value is modified through its setter")
        {
            const auto* keyBefore = member.key();

            auto mock2 = from_primitive(42);
            const auto* mock2ptr = mock2.get();
            member.value(std::move(mock2));

            THEN("its value changes to the new one")
            {
                REQUIRE(member.value() == mock2ptr);
            }

            THEN("its key does not change")
            {
                REQUIRE(member.key() == keyBefore);
            }
        }
    }
}

SCENARIO("member DSDs are tested for equality and inequality", "[Element][Member][equality]")
{
    GIVEN("An member DSD with (\"foo\", ObjectElement) value")
    {
        auto make_test_data = []() {
            return Member{ "foo",
                make_element<ObjectElement>( //
                    make_element<MemberElement>("bar", from_primitive("baz")),
                    make_element<MemberElement>("boo", make_empty<NumberElement>())) };
        };

        auto data = make_test_data();

        GIVEN("An member element constructed equivalently")
        {
            auto data2 = make_test_data();

            THEN("they test positive for equality")
            {
                REQUIRE(data == data2);
            }

            THEN("they test negative for inequality")
            {
                REQUIRE(!(data != data2));
            }
        }

        GIVEN("An member element with different key")
        {
            Member data2{ "fooz",
                make_element<ObjectElement>( //
                    make_element<MemberElement>("bar", from_primitive("baz")),
                    make_element<MemberElement>("boo", make_empty<NumberElement>())) };

            THEN("they test negative for equality")
            {
                REQUIRE(!(data == data2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(data != data2);
            }
        }

        GIVEN("An member element with different value")
        {
            Member data2{ "foo", from_primitive("bar") };

            THEN("they test negative for equality")
            {
                REQUIRE(!(data == data2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(data != data2);
            }
        }

        GIVEN("An empty member element")
        {
            Member data2;

            THEN("they test negative for equality")
            {
                REQUIRE(!(data == data2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(data != data2);
            }
        }
    }
}
