//
//  test/refract/dsd/test-InfoElements.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 26/08/2016
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include <catch2/catch.hpp>

#include "refract/InfoElements.h"
#include "refract/ElementIfc.h"
#include "refract/ElementFwd.h"
#include "refract/Element.h"

using namespace refract;

SCENARIO("InfoElementss are initialy empty", "[InfoElements]")
{
    GIVEN("A default initialized InfoElements")
    {
        InfoElements collection;

        THEN("it is empty")
        {
            REQUIRE(collection.empty());
        }
        THEN("its size is zero")
        {
            REQUIRE(collection.size() == 0);
        }
        THEN("its begin is equal to its end")
        {
            REQUIRE(collection.begin() == collection.end());
        }
    }
}

SCENARIO("Elements can be added and erased from InfoElements", "[InfoElements]")
{
    GIVEN("An empty InfoElements")
    {
        InfoElements collection;

        WHEN("a StringElement is added")
        {
            const char* key = "id";

            auto el = make_element<StringElement>("foo");
            const auto* elPtr = el.get();
            collection.set(key, std::move(el));

            THEN("its size increments")
            {
                REQUIRE(collection.size() == 1);
            }
            THEN("the pointer obtained by find equals the original one")
            {
                REQUIRE(collection.find(key)->second.get() == elPtr);
            }

            WHEN("the member is deleted by erase(const std::string&)")
            {
                collection.erase(key);

                THEN("its size decrements")
                {
                    REQUIRE(collection.size() == 0);
                }
                THEN("it is empty")
                {
                    REQUIRE(collection.empty());
                }
            }
        }
    }
}

SCENARIO("InfoElementss can be copied and moved", "[InfoElements]")
{
    GIVEN("An empty InfoElements")
    {
        InfoElements collection;

        WHEN("from it another InfoElements is copy constructed")
        {
            InfoElements collection2(collection);

            THEN("the latter is also empty")
            {
                REQUIRE(collection.empty());
            }
        }

        WHEN("from it another InfoElements is move constructed")
        {
            InfoElements collection2(std::move(collection));

            THEN("the latter is also empty")
            {
                REQUIRE(collection.empty());
            }
        }
    }

    GIVEN("A InfoElements with three entries")
    {

        std::string key1 = "id";
        std::string key2 = "peterson";
        std::string key3 = "rabbit h\000le";

        auto str1 = make_element<StringElement>("a");
        const auto* str1ptr = str1.get();
        auto str1clone = make_element<StringElement>("a");
        auto str2 = make_element<StringElement>("b");
        const auto* str2ptr = str2.get();
        auto str2clone = make_element<StringElement>("b");
        auto str3 = make_element<StringElement>("c");
        const auto* str3ptr = str3.get();
        auto str3clone = make_element<StringElement>("c");

        InfoElements collection;

        collection.set(key1, std::move(str1));
        collection.set(key2, std::move(str2));
        collection.set(key3, std::move(str3));

        REQUIRE(collection.size() == 3);

        WHEN("from it another InfoElements is copy constructed")
        {
            InfoElements collection2(collection);

            THEN("their sizes equal")
            {
                REQUIRE(collection.size() == collection2.size());
            }
            THEN("the values of the latter are not nullptr")
            {
                for (const auto& m : collection2) {
                    REQUIRE(m.second);
                }
            }
            THEN("their keys equal")
            {
                REQUIRE(std::equal(collection.begin(),
                    collection.end(),
                    collection2.begin(),
                    [](const InfoElements::value_type& m1, const InfoElements::value_type& m2) {
                        return m1.first == m2.first;
                    }));
            }
            THEN("their values equal")
            {
                REQUIRE(std::equal(collection.begin(),
                    collection.end(),
                    collection2.begin(),
                    [](const InfoElements::value_type& m1, const InfoElements::value_type& m2) {
                        return *m1.second == *m2.second;
                    }));
            }
        }

        WHEN("from it another InfoElements is move constructed")
        {
            InfoElements collection2(std::move(collection));

            THEN("the original is empty")
            {
                REQUIRE(collection.empty());
            }
            THEN("the size of the latter is 3")
            {
                REQUIRE(collection2.size() == 3);
            }
            THEN("the values of the latter are not nullptr")
            {
                for (const auto& m : collection2) {
                    REQUIRE(m.second);
                }
            }
            THEN("their keys equal")
            {
                REQUIRE(std::equal(collection.begin(),
                    collection.end(),
                    collection2.begin(),
                    [](const InfoElements::value_type& m1, const InfoElements::value_type& m2) {
                        return m1.first == m2.first;
                    }));
            }
            THEN("the values of the latter are the same as the original values")
            {
                REQUIRE(collection2.find(key1)->second.get() == str1ptr);
                REQUIRE(collection2.find(key2)->second.get() == str2ptr);
                REQUIRE(collection2.find(key3)->second.get() == str3ptr);
            }
        }
    }
}
