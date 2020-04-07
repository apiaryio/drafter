//
//  test/refract/dsd/test-Array.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include <catch2/catch.hpp>

#include "refract/dsd/Array.h"
#include "refract/Element.h"

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

        WHEN("a StringElement is pushed back")
        {
            auto str = make_element<StringElement>();
            auto str1ptr = str.get();

            array.push_back(std::move(str));

            THEN("its size is 1")
            {
                REQUIRE(array.size() == 1);
            }

            THEN("it is not empty")
            {
                REQUIRE(!array.empty());
            }

            THEN("its begin points to the str")
            {
                REQUIRE((*array.begin()).get() == str1ptr);
            }

            WHEN("another three StringElements are inserted at begin two at end")
            {
                auto str2 = make_element<StringElement>();

                auto str3 = make_element<StringElement>();

                auto str4 = make_element<StringElement>();

                array.insert(array.begin(), std::move(str2));
                array.insert(array.begin(), std::move(str3));
                array.insert(array.begin(), std::move(str4));

                auto str5 = make_element<StringElement>();
                auto str6 = make_element<StringElement>();

                array.insert(array.begin(), std::move(str5));
                array.insert(array.begin(), std::move(str6));

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
                    }

                    THEN("the resulting iterator points at the newly third element")
                    {
                        REQUIRE(it == std::next(array.begin(), 2));
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

            WHEN("another StringElement is pushed back")
            {
                auto str2 = make_element<StringElement>();
                auto str2ptr = str2.get();

                array.push_back(std::move(str2));

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

                THEN("its first two elements are the given strs")
                {
                    REQUIRE(array.begin()[0].get() == str1ptr);
                    REQUIRE(array.begin()[1].get() == str2ptr);
                }
            }
        }
    }
}

SCENARIO("`Array` is move-constructed from elements", "[ElementData][Array]")
{
    GIVEN("Three StringElement instances")
    {
        auto str1 = make_element<StringElement>();
        auto str2 = make_element<StringElement>();
        auto str3 = make_element<StringElement>();

        const auto str1ptr = str1.get();
        const auto str2ptr = str2.get();
        const auto str3ptr = str3.get();

        WHEN("An Array is constructed from them")
        {
            Array array(std::move(str1), std::move(str2), std::move(str3));

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
            THEN("its members were obtained by moving in the original strs")
            {
                REQUIRE(array.begin()[0].get() == str1ptr);
                REQUIRE(array.begin()[1].get() == str2ptr);
                REQUIRE(array.begin()[2].get() == str3ptr);
            }
        }

        WHEN("an Array is created from them")
        {
            Array array{ std::move(str1), std::move(str2), std::move(str3) };

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
            THEN("its first three elements are the given strs")
            {
                REQUIRE(array.begin()[0].get() == str1ptr);
                REQUIRE(array.begin()[1].get() == str2ptr);
                REQUIRE(array.begin()[2].get() == str3ptr);
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
                THEN("their entries are equal")
                {
                    REQUIRE(std::equal(array2.begin(),
                        array2.end(),
                        array.begin(),
                        [](const std::unique_ptr<IElement>& e1, const std::unique_ptr<IElement>& e2) {
                            return *e1 == *e2;
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
                THEN("its members are the original strs")
                {
                    REQUIRE(array2.begin()[0].get() == str1ptr);
                    REQUIRE(array2.begin()[1].get() == str2ptr);
                    REQUIRE(array2.begin()[2].get() == str3ptr);
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
