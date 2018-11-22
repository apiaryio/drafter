//
//  test/refract/dsd/test-Option.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include <catch2/catch.hpp>

#include "refract/dsd/Option.h"
#include "refract/Element.h"

#include <array>

using namespace refract;
using namespace dsd;

TEST_CASE("`Option`'s default element name is `option`", "[Element][Option]")
{
    REQUIRE(std::string(Option::name) == "option");
}

SCENARIO("`Option` is default constructed and both copy- and move constructed from", "[ElementData][Option]")
{
    GIVEN("A default initialized Option")
    {
        Option option;

        THEN("it is empty")
        {
            REQUIRE(option.empty());
        }

        THEN("its size is zero")
        {
            REQUIRE(option.size() == 0);
        }

        THEN("its begin is equal to its end")
        {
            REQUIRE(option.begin() == option.end());
        }

        WHEN("from it another Option is copy constructed")
        {
            Option option2(option);

            THEN("the latter Option is also empty")
            {
                REQUIRE(option2.empty());
            }
        }

        WHEN("from it another Option is copy constructed")
        {
            Option option2(std::move(option));

            THEN("the original Option is empty")
            {
                REQUIRE(option2.empty());
            }

            THEN("the latter Option is also empty")
            {
                REQUIRE(option2.empty());
            }
        }
    }
}

SCENARIO("`Option` is inserted to and erased from", "[ElementData][Option]")
{
    GIVEN("A default initialized `Option`")
    {
        Option option;

        WHEN("an StringElement is pushed back")
        {
            auto mock = from_primitive("abc");
            auto mock1ptr = mock.get();

            option.push_back(std::move(mock));

            THEN("its size is 1")
            {
                REQUIRE(option.size() == 1);
            }

            THEN("it is not empty")
            {
                REQUIRE(!option.empty());
            }

            THEN("its begin points to the mock")
            {
                REQUIRE((*option.begin()).get() == mock1ptr);
            }

            WHEN("another three elements are inserted at begin two at end")
            {
                auto mock2 = from_primitive(2);
                auto mock2ptr = mock2.get();
                auto mock3 = from_primitive(3);
                auto mock3ptr = mock3.get();
                auto mock4 = from_primitive(4);
                auto mock4ptr = mock4.get();

                option.insert(option.begin(), std::move(mock2));
                option.insert(option.begin(), std::move(mock3));
                option.insert(option.begin(), std::move(mock4));

                auto mock5 = from_primitive(5);
                auto mock5ptr = mock5.get();

                auto mock6 = from_primitive(6);
                auto mock6ptr = mock6.get();

                option.insert(option.end(), std::move(mock5));
                option.insert(option.end(), std::move(mock6));

                THEN("its size is 6")
                {
                    REQUIRE(option.size() == 6);
                }

                WHEN("the third Element is erased")
                {
                    auto it_ = option.begin();
                    std::advance(it_, 2);
                    auto it = option.erase(it_);

                    THEN("its size is 5")
                    {
                        REQUIRE(option.size() == 5);

                        THEN("other instances are still contained")
                        {
                            REQUIRE(option.begin()[0].get() == mock4ptr);
                            REQUIRE(option.begin()[1].get() == mock3ptr);
                            REQUIRE(option.begin()[2].get() == mock1ptr);
                            REQUIRE(option.begin()[3].get() == mock5ptr);
                            REQUIRE(option.begin()[4].get() == mock6ptr);
                        }

                        THEN("the resulting iterator points at the newly third element")
                        {
                            REQUIRE(it == std::next(option.begin(), 2));
                        }
                    }
                }

                WHEN("all but the first Element are erased")
                {
                    auto it = option.erase(std::next(option.begin()), option.end());

                    THEN("its size is 1")
                    {
                        REQUIRE(option.size() == 1);
                    }

                    THEN("the resulting iterator is end")
                    {
                        REQUIRE(it == option.end());
                    }
                }
            }

            WHEN("another element is pushed back")
            {
                auto mock2 = from_primitive("tralala");
                auto mock2ptr = mock2.get();

                option.push_back(std::move(mock2));

                THEN("its size is 2")
                {
                    REQUIRE(option.size() == 2);
                }

                THEN("the begin is 2 away from the end")
                {
                    REQUIRE(option.end() - option.begin() == 2);
                }

                THEN("it is not empty")
                {
                    REQUIRE(!option.empty());
                }

                THEN("its first two elements are the given mocks")
                {
                    REQUIRE(option.begin()[0].get() == mock1ptr);
                    REQUIRE(option.begin()[1].get() == mock2ptr);
                }

                THEN("it is iterable")
                {
                    auto mocks = std::array<IElement*, 2>{ mock1ptr, mock2ptr };
                    auto mocks_it = mocks.begin();
                    int ctx = 0;
                    for (const auto& el : option) {
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

SCENARIO("`Option` is move-constructed from elements", "[ElementData][Option]")
{
    GIVEN("Three elements")
    {
        auto mock1 = from_primitive("foo");
        auto mock2 = from_primitive("bar");
        auto mock3 = from_primitive("baz");

        const auto mock1ptr = mock1.get();
        const auto mock2ptr = mock2.get();
        const auto mock3ptr = mock3.get();

        WHEN("An Option is constructed from them")
        {
            Option option(std::move(mock1), std::move(mock2), std::move(mock3));

            THEN("its size is three")
            {
                REQUIRE(option.size() == 3);
            }
            THEN("its begin is 3 away from its end")
            {
                REQUIRE(option.end() - option.begin() == 3);
            }
            THEN("it is not empty")
            {
                REQUIRE(!option.empty());
            }
            THEN("it is iterable")
            {
                auto mock_ptrs = std::array<const IElement*, 3>{ mock1ptr, mock2ptr, mock3ptr };
                auto mock_ptrs_it = mock_ptrs.begin();
                int ctx = 0;
                for (const auto& el : option) {
                    REQUIRE(mock_ptrs_it < mock_ptrs.end()); // memory overflow
                    REQUIRE(*mock_ptrs_it == el.get());
                    ++mock_ptrs_it;
                    ++ctx;
                    REQUIRE(ctx != 0); // integer overflow
                }
                REQUIRE(ctx == 3);
            }
        }

        WHEN("an Option is created from them")
        {
            Option option{ std::move(mock1), std::move(mock2), std::move(mock3) };

            THEN("its size is three")
            {
                REQUIRE(option.size() == 3);
            }
            THEN("its begin is 3 away from its end")
            {
                REQUIRE(option.end() - option.begin() == 3);
            }
            THEN("it is not empty")
            {
                REQUIRE(!option.empty());
            }
            THEN("its first three elements are the given mocks")
            {
                REQUIRE(option.begin()[0].get() == mock1ptr);
                REQUIRE(option.begin()[1].get() == mock2ptr);
                REQUIRE(option.begin()[2].get() == mock3ptr);
            }
            THEN("it is iterable")
            {
                auto mocks = std::array<IElement*, 3>{ mock1ptr, mock2ptr, mock3ptr };
                auto mocks_it = mocks.begin();
                int ctx = 0;
                for (const auto& el : option) {
                    REQUIRE(mocks_it < mocks.end()); // memory overflow
                    REQUIRE(*mocks_it == el.get());
                    ++mocks_it;
                    ++ctx;
                    REQUIRE(ctx != 0); // integer overflow
                }
                REQUIRE(ctx == 3);
            }

            WHEN("another Option is copy constructed from it")
            {
                Option option2(option);

                THEN("its size is three")
                {
                    REQUIRE(option2.size() == 3);
                }
                THEN("its begin is 3 away from its end")
                {
                    REQUIRE(option2.end() - option2.begin() == 3);
                }
                THEN("the original's size is three")
                {
                    REQUIRE(option.size() == 3);
                }
                THEN("it is not empty")
                {
                    REQUIRE(!option2.empty());
                }
                THEN("the original is not empty")
                {
                    REQUIRE(!option.empty());
                }
                THEN("its size equals the original's size")
                {
                    REQUIRE(option.size() == option2.size());
                }
                THEN("it is iterable")
                {
                    int ctx = 0;
                    for (const auto& el : option2) {
                        REQUIRE(el);
                        ++ctx;
                        REQUIRE(ctx != 0); // integer overflow
                    }
                    REQUIRE(ctx == 3);
                }

                THEN("its members are equal")
                {
                    REQUIRE(option.size() == option2.size());
                    REQUIRE(std::equal(option.begin(), option.end(), option2.begin(), [](const auto& a, const auto& b) {
                        return *a == *b;
                    }));
                }
            }

            WHEN("another Option is move constructed from it")
            {
                Option option2(std::move(option));

                THEN("its size is three")
                {
                    REQUIRE(option2.size() == 3);
                }
                THEN("its begin is 3 away from its end")
                {
                    REQUIRE(option2.end() - option2.begin() == 3);
                }
                THEN("the original's begin is at its end")
                {
                    REQUIRE(option.end() == option.begin());
                }
                THEN("the original's size is zero")
                {
                    REQUIRE(option.size() == 0);
                }
                THEN("the original is empty")
                {
                    REQUIRE(option.empty());
                }
                THEN("it is not empty")
                {
                    REQUIRE(!option2.empty());
                }
                THEN("it is iterable")
                {
                    auto mocks = std::array<IElement*, 3>{ mock1ptr, mock2ptr, mock3ptr };
                    auto mocks_it = mocks.begin();
                    int ctx = 0;
                    for (const auto& el : option2) {
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
                    REQUIRE(option2.begin()[0].get() == mock1ptr);
                    REQUIRE(option2.begin()[1].get() == mock2ptr);
                    REQUIRE(option2.begin()[2].get() == mock3ptr);
                }
            }
        }
    }
}

SCENARIO("option DSDs are tested for equality and inequality", "[Element][Option][equality]")
{
    GIVEN("An option DSD with some members")
    {
        auto make_test_data = []() {
            return Option( //
                from_primitive(true),
                from_primitive(42));
        };

        Option data = make_test_data();

        GIVEN("An option element constructed equivalently")
        {
            Option data2 = make_test_data();

            THEN("they test positive for equality")
            {
                REQUIRE(data == data2);
            }

            THEN("they test negative for inequality")
            {
                REQUIRE(!(data != data2));
            }
        }

        GIVEN("An empty option element")
        {
            Option data2;

            THEN("they test negative for equality")
            {
                REQUIRE(!(data == data2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(data != data2);
            }
        }

        GIVEN("An option element containing additional elements")
        {
            Option data2 = make_test_data();
            data2.push_back(from_primitive("foo"));

            THEN("they test negative for equality")
            {
                REQUIRE(!(data == data2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(data != data2);
            }
        }

        GIVEN("An option element containing less elements")
        {
            Option data2( //
                from_primitive(true));

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
