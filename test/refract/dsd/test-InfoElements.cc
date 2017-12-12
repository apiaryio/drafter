//
//  test/refract/dsd/test-InfoElements.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 26/08/2016
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "catch.hpp"

#include "refract/InfoElements.h"
#include "refract/ElementIfc.h"

#include "ElementMock.h"

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

            auto el = new test::ElementMock();
            el->element_out = "\0 42";
            collection.set(key, std::unique_ptr<IElement>(el));

            THEN("its size increments")
            {
                REQUIRE(collection.size() == 1);
            }
            THEN("nothing gets called on the mock")
            {
                REQUIRE(el->_total_ctx == 0);
            }
            THEN("the original mock pointer is still dereferencable")
            {
                REQUIRE(el->element() == "\0 42");
            }
            THEN("the pointer obtained by find equals the original one")
            {
                REQUIRE(collection.find(key)->second.get() == el);
            }

            WHEN("the member is deleted by erase(const std::string&)")
            {
                REQUIRE(test::ElementMock::instances().size() == 1);
                collection.erase(key);

                THEN("its size decrements")
                {
                    REQUIRE(collection.size() == 0);
                }
                THEN("it is empty")
                {
                    REQUIRE(collection.empty());
                }
                THEN("the mock is destructed")
                {
                    REQUIRE(test::ElementMock::instances().size() == 0);
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

        auto mock1 = new test::ElementMock{};
        auto mock2 = new test::ElementMock{};
        auto mock3 = new test::ElementMock{};

        auto mock1clone = new test::ElementMock{};
        auto mock2clone = new test::ElementMock{};
        auto mock3clone = new test::ElementMock{};

        mock1->clone_out.reset(mock1clone);
        mock2->clone_out.reset(mock2clone);
        mock3->clone_out.reset(mock3clone);

        InfoElements collection;

        collection.set(key1, std::unique_ptr<IElement>(mock1));
        collection.set(key2, std::unique_ptr<IElement>(mock2));
        collection.set(key3, std::unique_ptr<IElement>(mock3));

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
            THEN("the values of the latter were obtained by calling clone() on the original")
            {
                REQUIRE(mock1->_total_ctx == 1);
                REQUIRE(mock1->clone_ctx == 1);
                REQUIRE(mock1->clone_in == IElement::cAll);
                REQUIRE(collection2.find(key1)->second.get() == mock1clone);

                REQUIRE(mock2->_total_ctx == 1);
                REQUIRE(mock2->clone_ctx == 1);
                REQUIRE(mock2->clone_in == IElement::cAll);
                REQUIRE(collection2.find(key2)->second.get() == mock2clone);

                REQUIRE(mock3->_total_ctx == 1);
                REQUIRE(mock3->clone_ctx == 1);
                REQUIRE(mock3->clone_in == IElement::cAll);
                REQUIRE(collection2.find(key3)->second.get() == mock3clone);
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
            THEN("the values of the latter are the same as the original values")
            {
                REQUIRE(collection2.find(key1)->second.get() == mock1);
                REQUIRE(collection2.find(key2)->second.get() == mock2);
                REQUIRE(collection2.find(key3)->second.get() == mock3);
            }
        }
    }
}
