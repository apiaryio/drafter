//
//  test/refract/dsd/test-Array.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include <catch2/catch.hpp>

#include <refract/dsd/Array.h>
#include <refract/Element.h>

#include <array>

using namespace refract;
using namespace dsd;

TEST_CASE("`Array`'s default element name is `array`", "[Element][Array]")
{
    REQUIRE(std::string(Array::name) == "array");
}

SCENARIO("`Array` is default constructed and both copy- and move constructed from", "[ElementData][Array]")
{
    GIVEN("A default initialized Array")
    {
        Array array;

        THEN("it is empty")
        {
            REQUIRE(array.empty());
        }

        THEN("its size is zero")
        {
            REQUIRE(array.size() == 0);
        }

        THEN("its begin is equal to its end")
        {
            REQUIRE(array.begin() == array.end());
        }

        WHEN("from it another Array is copy constructed")
        {
            Array array2(array);

            THEN("the latter Array is also empty")
            {
                REQUIRE(array2.empty());
            }
        }

        WHEN("from it another Array is copy constructed")
        {
            Array array2(std::move(array));

            THEN("the original Array is empty")
            {
                REQUIRE(array2.empty());
            }

            THEN("the latter Array is also empty")
            {
                REQUIRE(array2.empty());
            }
        }
    }
}

SCENARIO("`Array` is inserted to and erased from", "[ElementData][Array]")
{
    GIVEN("A default initialized `Array`")
    {
        Array array;

        WHEN("an StringElement is pushed back")
        {
            auto mock = from_primitive("abc");
            auto mock1ptr = mock.get();

            array.push_back(std::move(mock));

            THEN("its size is 1")
            {
                REQUIRE(array.size() == 1);
            }

            THEN("it is not empty")
            {
                REQUIRE(!array.empty());
            }

            THEN("its begin points to the mock")
            {
                REQUIRE((*array.begin()).get() == mock1ptr);
            }

            WHEN("another three elements are inserted at begin two at end")
            {
                auto mock2 = from_primitive(2);
                auto mock2ptr = mock2.get();
                auto mock3 = from_primitive(3);
                auto mock3ptr = mock3.get();
                auto mock4 = from_primitive(4);
                auto mock4ptr = mock4.get();

                array.insert(array.begin(), std::move(mock2));
                array.insert(array.begin(), std::move(mock3));
                array.insert(array.begin(), std::move(mock4));

                auto mock5 = from_primitive(5);
                auto mock5ptr = mock5.get();

                auto mock6 = from_primitive(6);
                auto mock6ptr = mock6.get();

                array.insert(array.end(), std::move(mock5));
                array.insert(array.end(), std::move(mock6));

                THEN("its size is 6")
                {
                    REQUIRE(array.size() == 6);
                }

                WHEN("the third Element is erased")
                {
                    auto it_ = array.begin();
                    std::advance(it_, 2);
                    auto it = array.erase(it_);

                    THEN("its size is 5")
                    {
                        REQUIRE(array.size() == 5);

                        THEN("other instances are still contained")
                        {
                            REQUIRE(array.begin()[0].get() == mock4ptr);
                            REQUIRE(array.begin()[1].get() == mock3ptr);
                            REQUIRE(array.begin()[2].get() == mock1ptr);
                            REQUIRE(array.begin()[3].get() == mock5ptr);
                            REQUIRE(array.begin()[4].get() == mock6ptr);
                        }

                        THEN("the resulting iterator points at the newly third element")
                        {
                            REQUIRE(it == std::next(array.begin(), 2));
                        }
                    }
                }

                WHEN("all but the first Element are erased")
                {
                    auto it = array.erase(std::next(array.begin()), array.end());

                    THEN("its size is 1")
                    {
                        REQUIRE(array.size() == 1);
                    }

                    THEN("the resulting iterator is end")
                    {
                        REQUIRE(it == array.end());
                    }
                }
            }

            WHEN("another element is pushed back")
            {
                auto mock2 = from_primitive("tralala");
                auto mock2ptr = mock2.get();

                array.push_back(std::move(mock2));

                THEN("its size is 2")
                {
                    REQUIRE(array.size() == 2);
                }

                THEN("the begin is 2 away from the end")
                {
                    REQUIRE(array.end() - array.begin() == 2);
                }

                THEN("it is not empty")
                {
                    REQUIRE(!array.empty());
                }

                THEN("its first two elements are the given mocks")
                {
                    REQUIRE(array.begin()[0].get() == mock1ptr);
                    REQUIRE(array.begin()[1].get() == mock2ptr);
                }

                THEN("it is iterable")
                {
                    auto mocks = std::array<IElement*, 2>{ mock1ptr, mock2ptr };
                    auto mocks_it = mocks.begin();
                    int ctx = 0;
                    for (const auto& el : array) {
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

SCENARIO("`Array` is move-constructed from elements", "[ElementData][Array]")
{
    GIVEN("Three elements")
    {
        auto mock1 = from_primitive("foo");
        auto mock2 = from_primitive("bar");
        auto mock3 = from_primitive("baz");

        const auto mock1ptr = mock1.get();
        const auto mock2ptr = mock2.get();
        const auto mock3ptr = mock3.get();

        WHEN("An Array is constructed from them")
        {
            Array array(std::move(mock1), std::move(mock2), std::move(mock3));

            THEN("its size is three")
            {
                REQUIRE(array.size() == 3);
            }
            THEN("its begin is 3 away from its end")
            {
                REQUIRE(array.end() - array.begin() == 3);
            }
            THEN("it is not empty")
            {
                REQUIRE(!array.empty());
            }
            THEN("it is iterable")
            {
                auto mock_ptrs = std::array<const IElement*, 3>{ mock1ptr, mock2ptr, mock3ptr };
                auto mock_ptrs_it = mock_ptrs.begin();
                int ctx = 0;
                for (const auto& el : array) {
                    REQUIRE(mock_ptrs_it < mock_ptrs.end()); // memory overflow
                    REQUIRE(*mock_ptrs_it == el.get());
                    ++mock_ptrs_it;
                    ++ctx;
                    REQUIRE(ctx != 0); // integer overflow
                }
                REQUIRE(ctx == 3);
            }
        }

        WHEN("an Array is created from them")
        {
            Array array{ std::move(mock1), std::move(mock2), std::move(mock3) };

            THEN("its size is three")
            {
                REQUIRE(array.size() == 3);
            }
            THEN("its begin is 3 away from its end")
            {
                REQUIRE(array.end() - array.begin() == 3);
            }
            THEN("it is not empty")
            {
                REQUIRE(!array.empty());
            }
            THEN("its first three elements are the given mocks")
            {
                REQUIRE(array.begin()[0].get() == mock1ptr);
                REQUIRE(array.begin()[1].get() == mock2ptr);
                REQUIRE(array.begin()[2].get() == mock3ptr);
            }
            THEN("it is iterable")
            {
                auto mocks = std::array<IElement*, 3>{ mock1ptr, mock2ptr, mock3ptr };
                auto mocks_it = mocks.begin();
                int ctx = 0;
                for (const auto& el : array) {
                    REQUIRE(mocks_it < mocks.end()); // memory overflow
                    REQUIRE(*mocks_it == el.get());
                    ++mocks_it;
                    ++ctx;
                    REQUIRE(ctx != 0); // integer overflow
                }
                REQUIRE(ctx == 3);
            }

            WHEN("another Array is copy constructed from it")
            {
                Array array2(array);

                THEN("its size is three")
                {
                    REQUIRE(array2.size() == 3);
                }
                THEN("its begin is 3 away from its end")
                {
                    REQUIRE(array2.end() - array2.begin() == 3);
                }
                THEN("the original's size is three")
                {
                    REQUIRE(array.size() == 3);
                }
                THEN("it is not empty")
                {
                    REQUIRE(!array2.empty());
                }
                THEN("the original is not empty")
                {
                    REQUIRE(!array.empty());
                }
                THEN("its size equals the original's size")
                {
                    REQUIRE(array.size() == array2.size());
                }
                THEN("it is iterable")
                {
                    int ctx = 0;
                    for (const auto& el : array2) {
                        REQUIRE(el);
                        ++ctx;
                        REQUIRE(ctx != 0); // integer overflow
                    }
                    REQUIRE(ctx == 3);
                }

                THEN("its members are equal")
                {
                    REQUIRE(array.size() == array2.size());
                    REQUIRE(std::equal(array.begin(), array.end(), array2.begin(), [](const auto& a, const auto& b) {
                        return *a == *b;
                    }));
                }
            }

            WHEN("another Array is move constructed from it")
            {
                Array array2(std::move(array));

                THEN("its size is three")
                {
                    REQUIRE(array2.size() == 3);
                }
                THEN("its begin is 3 away from its end")
                {
                    REQUIRE(array2.end() - array2.begin() == 3);
                }
                THEN("the original's begin is at its end")
                {
                    REQUIRE(array.end() == array.begin());
                }
                THEN("the original's size is zero")
                {
                    REQUIRE(array.size() == 0);
                }
                THEN("the original is empty")
                {
                    REQUIRE(array.empty());
                }
                THEN("it is not empty")
                {
                    REQUIRE(!array2.empty());
                }
                THEN("it is iterable")
                {
                    auto mocks = std::array<IElement*, 3>{ mock1ptr, mock2ptr, mock3ptr };
                    auto mocks_it = mocks.begin();
                    int ctx = 0;
                    for (const auto& el : array2) {
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
                    REQUIRE(array2.begin()[0].get() == mock1ptr);
                    REQUIRE(array2.begin()[1].get() == mock2ptr);
                    REQUIRE(array2.begin()[2].get() == mock3ptr);
                }
            }
        }
    }
}

SCENARIO("array DSDs are tested for equality and inequality", "[Element][Array][equality]")
{
    GIVEN("An array DSD with some members")
    {
        auto make_test_data = []() {
            return Array( //
                from_primitive(true),
                from_primitive(42));
        };

        Array data = make_test_data();

        GIVEN("An array element constructed equivalently")
        {
            Array data2 = make_test_data();

            THEN("they test positive for equality")
            {
                REQUIRE(data == data2);
            }

            THEN("they test negative for inequality")
            {
                REQUIRE(!(data != data2));
            }
        }

        GIVEN("An empty array element")
        {
            Array data2;

            THEN("they test negative for equality")
            {
                REQUIRE(!(data == data2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(data != data2);
            }
        }

        GIVEN("An array element containing additional elements")
        {
            Array data2 = make_test_data();
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

        GIVEN("An array element containing less elements")
        {
            Array data2( //
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
