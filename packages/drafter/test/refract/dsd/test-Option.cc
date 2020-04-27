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

            THEN("the original Option is empty")
            {
                REQUIRE(option.empty());
            }

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
                REQUIRE(option.empty());
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

        WHEN("a StringElement is pushed back")
        {
            auto str = make_element<StringElement>("a");
            auto str1ptr = str.get();

            option.push_back(std::move(str));

            THEN("its size is 1")
            {
                REQUIRE(option.size() == 1);
            }

            THEN("it is not empty")
            {
                REQUIRE(!option.empty());
            }

            THEN("its begin points to the str")
            {
                REQUIRE((*option.begin()).get() == str1ptr);
            }

            WHEN("another three StringElements are inserted at begin two at end")
            {
                auto str2 = make_element<StringElement>("b");
                auto str3 = make_element<StringElement>("c");
                auto str4 = make_element<StringElement>("d");

                option.insert(option.begin(), std::move(str2));
                option.insert(option.begin(), std::move(str3));
                option.insert(option.begin(), std::move(str4));

                auto str5 = make_element<StringElement>("e");
                auto str6 = make_element<StringElement>("f");

                option.insert(option.begin(), std::move(str5));
                option.insert(option.begin(), std::move(str6));

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
                    }

                    THEN("the resulting iterator points at the newly third element")
                    {
                        REQUIRE(it == std::next(option.begin(), 2));
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

            WHEN("another StringElement is pushed back")
            {
                auto str2 = make_element<StringElement>();
                auto str2ptr = str2.get();

                option.push_back(std::move(str2));

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

                THEN("its first two elements are the given strs")
                {
                    REQUIRE(option.begin()[0].get() == str1ptr);
                    REQUIRE(option.begin()[1].get() == str2ptr);
                }
            }
        }
    }
}

SCENARIO("`Option` is move-constructed from elements", "[ElementData][Option]")
{
    GIVEN("Three StringElement instances")
    {
        auto str1 = make_element<StringElement>();
        auto str2 = make_element<StringElement>();
        auto str3 = make_element<StringElement>();

        const auto str1ptr = str1.get();
        const auto str2ptr = str2.get();
        const auto str3ptr = str3.get();

        WHEN("An Option is constructed from them")
        {
            Option option(std::move(str1), std::move(str2), std::move(str3));

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
                auto str_ptrs = std::array<const IElement*, 3>{ str1ptr, str2ptr, str3ptr };
                auto str_ptrs_it = str_ptrs.begin();
                int ctx = 0;
                for (const auto& el : option) {
                    REQUIRE(str_ptrs_it < str_ptrs.end()); // memory overflow
                    REQUIRE(*str_ptrs_it == el.get());
                    ++str_ptrs_it;
                    ++ctx;
                    REQUIRE(ctx != 0); // integer overflow
                }
                REQUIRE(ctx == 3);
            }

            THEN("its members were obtained by moving in the original strs")
            {
                REQUIRE(option.begin()[0].get() == str1ptr);
                REQUIRE(option.begin()[1].get() == str2ptr);
                REQUIRE(option.begin()[2].get() == str3ptr);
            }
        }

        WHEN("an Option is created from them")
        {
            {
                Option option{ std::move(str1), std::move(str2), std::move(str3) };

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
                THEN("its first three elements are the given strs")
                {
                    REQUIRE(option.begin()[0].get() == str1ptr);
                    REQUIRE(option.begin()[1].get() == str2ptr);
                    REQUIRE(option.begin()[2].get() == str3ptr);
                }
                THEN("it is iterable")
                {
                    auto strs = std::array<IElement*, 3>{ str1ptr, str2ptr, str3ptr };
                    auto strs_it = strs.begin();
                    int ctx = 0;
                    for (const auto& el : option) {
                        REQUIRE(strs_it < strs.end()); // memory overflow
                        REQUIRE(*strs_it == el.get());
                        ++strs_it;
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
                    THEN("they are equal")
                    {
                        std::equal(option.begin(),
                            option.end(),
                            option2.begin(),
                            [](const std::unique_ptr<IElement>& e1, const std::unique_ptr<IElement>& e2) {
                                return *e1 == *e2;
                            });
                    }
                }

                WHEN("another Option is move constructed from it")
                {
                    {
                        Option option1(option);
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
                        THEN("they are equal")
                        {
                            std::equal(option1.begin(),
                                option1.end(),
                                option2.begin(),
                                [](const std::unique_ptr<IElement>& e1, const std::unique_ptr<IElement>& e2) {
                                    return *e1 == *e2;
                                });
                        }
                    }
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
