//
//  test/refract/dsd/test-InfoElements.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 26/08/2016
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include <catch2/catch.hpp>

#include <refract/InfoElements.h>
#include <refract/ElementIfc.h>
#include <refract/Element.h>

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

        WHEN("an ElementMock pointer is added")
        {
            const char* key = "id";

            auto el = from_primitive(42);
            auto elPtr = el.get();
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
        const char* key3 = "rabbit h\000le";

        auto mock1 = from_primitive(1);
        auto mock2 = from_primitive(2);
        auto mock3 = from_primitive(3);

        const auto* mock1ptr = mock1.get();
        const auto* mock2ptr = mock2.get();
        const auto* mock3ptr = mock3.get();

        InfoElements collection;

        collection.set(key1, std::move(mock1));
        collection.set(key2, std::move(mock2));
        collection.set(key3, std::move(mock3));

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
                REQUIRE(std::equal(
                    collection.begin(), collection.end(), collection2.begin(), [](const auto& m1, const auto& m2) {
                        return m1.first == m2.first;
                    }));
            }

            THEN("their values equal")
            {
                REQUIRE(std::equal(
                    collection.begin(), collection.end(), collection2.begin(), [](const auto& m1, const auto& m2) {
                        return *m1.second == *m2.second;
                    }));
            }

            THEN("their value's memory locations differ")
            {
                REQUIRE(std::equal(
                    collection.begin(), collection.end(), collection2.begin(), [](const auto& m1, const auto& m2) {
                        return m1.second != m2.second;
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
                REQUIRE(std::equal(
                    collection.begin(), collection.end(), collection2.begin(), [](const auto& m1, const auto& m2) {
                        return m1.first == m2.first;
                    }));
            }

            THEN("their value's memory locations are the same")
            {
                REQUIRE(std::equal(
                    collection.begin(), collection.end(), collection2.begin(), [](const auto& m1, const auto& m2) {
                        return m1.second == m2.second;
                    }));
            }
        }
    }
}
