//
//  test/refract/dsd/test-Object.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include <catch2/catch.hpp>

#include "refract/dsd/Object.h"
#include "refract/Element.h"

#include <array>

using namespace refract;
using namespace dsd;

TEST_CASE("`Object`'s default element name is `object`", "[Element][Object]")
{
    REQUIRE(std::string(Object::name) == "object");
}

SCENARIO("`Object` is default constructed and both copy- and move constructed from", "[ElementData][Object]")
{
    GIVEN("A default initialized Object")
    {
        Object object;

        THEN("it is empty")
        {
            REQUIRE(object.empty());
        }

        THEN("its size is zero")
        {
            REQUIRE(object.size() == 0);
        }

        THEN("its begin is equal to its end")
        {
            REQUIRE(object.begin() == object.end());
        }

        WHEN("from it another Object is copy constructed")
        {
            Object object2(object);

            THEN("the latter Object is also empty")
            {
                REQUIRE(object2.empty());
            }
        }

        WHEN("from it another Object is copy constructed")
        {
            Object object2(std::move(object));

            THEN("the original Object is empty")
            {
                REQUIRE(object2.empty());
            }

            THEN("the latter Object is also empty")
            {
                REQUIRE(object2.empty());
            }
        }
    }
}

SCENARIO("`Object` is inserted to and erased from", "[ElementData][Object]")
{
    GIVEN("A default initialized `Object`")
    {
        Object object;

        WHEN("an StringElement is pushed back")
        {
            auto mock = from_primitive("abc");
            auto mock1ptr = mock.get();

            object.push_back(std::move(mock));

            THEN("its size is 1")
            {
                REQUIRE(object.size() == 1);
            }

            THEN("it is not empty")
            {
                REQUIRE(!object.empty());
            }

            THEN("its begin points to the mock")
            {
                REQUIRE((*object.begin()).get() == mock1ptr);
            }

            WHEN("another three elements are inserted at begin two at end")
            {
                auto mock2 = from_primitive(2);
                auto mock2ptr = mock2.get();
                auto mock3 = from_primitive(3);
                auto mock3ptr = mock3.get();
                auto mock4 = from_primitive(4);
                auto mock4ptr = mock4.get();

                object.insert(object.begin(), std::move(mock2));
                object.insert(object.begin(), std::move(mock3));
                object.insert(object.begin(), std::move(mock4));

                auto mock5 = from_primitive(5);
                auto mock5ptr = mock5.get();

                auto mock6 = from_primitive(6);
                auto mock6ptr = mock6.get();

                object.insert(object.end(), std::move(mock5));
                object.insert(object.end(), std::move(mock6));

                THEN("its size is 6")
                {
                    REQUIRE(object.size() == 6);
                }

                WHEN("the third Element is erased")
                {
                    auto it_ = object.begin();
                    std::advance(it_, 2);
                    auto it = object.erase(it_);

                    THEN("its size is 5")
                    {
                        REQUIRE(object.size() == 5);

                        THEN("other instances are still contained")
                        {
                            REQUIRE(object.begin()[0].get() == mock4ptr);
                            REQUIRE(object.begin()[1].get() == mock3ptr);
                            REQUIRE(object.begin()[2].get() == mock1ptr);
                            REQUIRE(object.begin()[3].get() == mock5ptr);
                            REQUIRE(object.begin()[4].get() == mock6ptr);
                        }

                        THEN("the resulting iterator points at the newly third element")
                        {
                            REQUIRE(it == std::next(object.begin(), 2));
                        }
                    }
                }

                WHEN("all but the first Element are erased")
                {
                    auto it = object.erase(std::next(object.begin()), object.end());

                    THEN("its size is 1")
                    {
                        REQUIRE(object.size() == 1);
                    }

                    THEN("the resulting iterator is end")
                    {
                        REQUIRE(it == object.end());
                    }
                }
            }

            WHEN("another element is pushed back")
            {
                auto mock2 = from_primitive("tralala");
                auto mock2ptr = mock2.get();

                object.push_back(std::move(mock2));

                THEN("its size is 2")
                {
                    REQUIRE(object.size() == 2);
                }

                THEN("the begin is 2 away from the end")
                {
                    REQUIRE(object.end() - object.begin() == 2);
                }

                THEN("it is not empty")
                {
                    REQUIRE(!object.empty());
                }

                THEN("its first two elements are the given mocks")
                {
                    REQUIRE(object.begin()[0].get() == mock1ptr);
                    REQUIRE(object.begin()[1].get() == mock2ptr);
                }

                THEN("it is iterable")
                {
                    auto mocks = std::array<IElement*, 2>{ mock1ptr, mock2ptr };
                    auto mocks_it = mocks.begin();
                    int ctx = 0;
                    for (const auto& el : object) {
                        REQUIRE(mocks_it < mocks.end()); // memory overflow
                        REQUIRE(*mocks_it == el.get());
                        ++mocks_it;
                        ++ctx;
                        REQUIRE(ctx != 0); // integer overflow
                    }
                    REQUIRE(ctx == 2);
                }
            }
        }
    }
}

SCENARIO("`Object` is move-constructed from elements", "[ElementData][Object]")
{
    GIVEN("Three elements")
    {
        auto mock1 = from_primitive("foo");
        auto mock2 = from_primitive("bar");
        auto mock3 = from_primitive("baz");

        const auto mock1ptr = mock1.get();
        const auto mock2ptr = mock2.get();
        const auto mock3ptr = mock3.get();

        WHEN("An Object is constructed from them")
        {
            Object object(std::move(mock1), std::move(mock2), std::move(mock3));

            THEN("its size is three")
            {
                REQUIRE(object.size() == 3);
            }
            THEN("its begin is 3 away from its end")
            {
                REQUIRE(object.end() - object.begin() == 3);
            }
            THEN("it is not empty")
            {
                REQUIRE(!object.empty());
            }
            THEN("it is iterable")
            {
                auto mock_ptrs = std::array<const IElement*, 3>{ mock1ptr, mock2ptr, mock3ptr };
                auto mock_ptrs_it = mock_ptrs.begin();
                int ctx = 0;
                for (const auto& el : object) {
                    REQUIRE(mock_ptrs_it < mock_ptrs.end()); // memory overflow
                    REQUIRE(*mock_ptrs_it == el.get());
                    ++mock_ptrs_it;
                    ++ctx;
                    REQUIRE(ctx != 0); // integer overflow
                }
                REQUIRE(ctx == 3);
            }
        }

        WHEN("an Object is created from them")
        {
            Object object{ std::move(mock1), std::move(mock2), std::move(mock3) };

            THEN("its size is three")
            {
                REQUIRE(object.size() == 3);
            }
            THEN("its begin is 3 away from its end")
            {
                REQUIRE(object.end() - object.begin() == 3);
            }
            THEN("it is not empty")
            {
                REQUIRE(!object.empty());
            }
            THEN("its first three elements are the given mocks")
            {
                REQUIRE(object.begin()[0].get() == mock1ptr);
                REQUIRE(object.begin()[1].get() == mock2ptr);
                REQUIRE(object.begin()[2].get() == mock3ptr);
            }
            THEN("it is iterable")
            {
                auto mocks = std::array<IElement*, 3>{ mock1ptr, mock2ptr, mock3ptr };
                auto mocks_it = mocks.begin();
                int ctx = 0;
                for (const auto& el : object) {
                    REQUIRE(mocks_it < mocks.end()); // memory overflow
                    REQUIRE(*mocks_it == el.get());
                    ++mocks_it;
                    ++ctx;
                    REQUIRE(ctx != 0); // integer overflow
                }
                REQUIRE(ctx == 3);
            }

            WHEN("another Object is copy constructed from it")
            {
                Object object2(object);

                THEN("its size is three")
                {
                    REQUIRE(object2.size() == 3);
                }
                THEN("its begin is 3 away from its end")
                {
                    REQUIRE(object2.end() - object2.begin() == 3);
                }
                THEN("the original's size is three")
                {
                    REQUIRE(object.size() == 3);
                }
                THEN("it is not empty")
                {
                    REQUIRE(!object2.empty());
                }
                THEN("the original is not empty")
                {
                    REQUIRE(!object.empty());
                }
                THEN("its size equals the original's size")
                {
                    REQUIRE(object.size() == object2.size());
                }
                THEN("it is iterable")
                {
                    int ctx = 0;
                    for (const auto& el : object2) {
                        REQUIRE(el);
                        ++ctx;
                        REQUIRE(ctx != 0); // integer overflow
                    }
                    REQUIRE(ctx == 3);
                }

                THEN("its members are equal")
                {
                    REQUIRE(object.size() == object2.size());
                    REQUIRE(std::equal(object.begin(), object.end(), object2.begin(), [](const auto& a, const auto& b) {
                        return *a == *b;
                    }));
                }
            }

            WHEN("another Object is move constructed from it")
            {
                Object object2(std::move(object));

                THEN("its size is three")
                {
                    REQUIRE(object2.size() == 3);
                }
                THEN("its begin is 3 away from its end")
                {
                    REQUIRE(object2.end() - object2.begin() == 3);
                }
                THEN("the original's begin is at its end")
                {
                    REQUIRE(object.end() == object.begin());
                }
                THEN("the original's size is zero")
                {
                    REQUIRE(object.size() == 0);
                }
                THEN("the original is empty")
                {
                    REQUIRE(object.empty());
                }
                THEN("it is not empty")
                {
                    REQUIRE(!object2.empty());
                }
                THEN("it is iterable")
                {
                    auto mocks = std::array<IElement*, 3>{ mock1ptr, mock2ptr, mock3ptr };
                    auto mocks_it = mocks.begin();
                    int ctx = 0;
                    for (const auto& el : object2) {
                        REQUIRE(mocks_it < mocks.end()); // memory overflow
                        REQUIRE(*mocks_it == el.get());
                        ++mocks_it;
                        ++ctx;
                        REQUIRE(ctx != 0); // integer overflow
                    }
                    REQUIRE(ctx == 3);
                }

                THEN("its members are the original mocks")
                {
                    REQUIRE(object2.begin()[0].get() == mock1ptr);
                    REQUIRE(object2.begin()[1].get() == mock2ptr);
                    REQUIRE(object2.begin()[2].get() == mock3ptr);
                }
            }
        }
    }
}

SCENARIO("object DSDs are tested for equality and inequality", "[Element][Object][equality]")
{
    GIVEN("An object DSD with some members")
    {
        auto make_test_data = []() {
            return Object( //
                make_element<MemberElement>("foo", from_primitive("bar")),
                make_element<MemberElement>("baz", make_element<RefElement>("Boo")),
                make_element<MemberElement>("answer", from_primitive(42)));
        };

        Object data = make_test_data();

        GIVEN("An object element constructed equivalently")
        {
            Object data2 = make_test_data();

            THEN("they test positive for equality")
            {
                REQUIRE(data == data2);
            }

            THEN("they test negative for inequality")
            {
                REQUIRE(!(data != data2));
            }
        }

        GIVEN("An empty object element")
        {
            Object data2;

            THEN("they test negative for equality")
            {
                REQUIRE(!(data == data2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(data != data2);
            }
        }

        GIVEN("An object element containing additional elements")
        {
            Object data2 = make_test_data();
            data2.push_back(make_element<MemberElement>("bla", from_primitive("blu")));

            THEN("they test negative for equality")
            {
                REQUIRE(!(data == data2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(data != data2);
            }
        }

        GIVEN("An object element containing less elements")
        {
            Object data2( //
                make_element<MemberElement>("foo", from_primitive("bar")),
                make_element<MemberElement>("baz", make_element<RefElement>("Boo")));

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
