//
//  test/refract/dsd/test-Select.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include <catch2/catch.hpp>

#include "refract/dsd/Select.h"
#include "refract/Element.h"
#include "refract/ElementFwd.h"

#include <array>

using namespace refract;
using namespace dsd;

TEST_CASE("`Select`'s default element name is `select`", "[Element][Select]")
{
    REQUIRE(std::string(Select::name) == "select");
}

SCENARIO("`Select` is default constructed and both copy- and move constructed from", "[ElementData][Select]")
{
    GIVEN("A default initialized Select")
    {
        Select select;

        THEN("it is empty")
        {
            REQUIRE(select.empty());
        }

        THEN("its size is zero")
        {
            REQUIRE(select.size() == 0);
        }

        THEN("its begin is equal to its end")
        {
            REQUIRE(select.begin() == select.end());
        }

        WHEN("from it another Select is copy constructed")
        {
            Select select2(select);

            THEN("the latter Select is also empty")
            {
                REQUIRE(select2.empty());
            }
        }

        WHEN("from it another Select is copy constructed")
        {
            Select select2(std::move(select));

            THEN("the original Select is empty")
            {
                REQUIRE(select2.empty());
            }

            THEN("the latter Select is also empty")
            {
                REQUIRE(select2.empty());
            }
        }
    }
}

SCENARIO("`Select` is inserted to and erased from", "[ElementData][Select]")
{
    GIVEN("A default initialized `Select`")
    {
        Select select;

        WHEN("a StringElement is pushed back")
        {
            auto str1 = make_element<OptionElement>(make_element<StringElement>("a"));
            auto str1ptr = str1.get();
            select.push_back(std::move(str1));

            THEN("its size is 1")
            {
                REQUIRE(select.size() == 1);
            }

            THEN("it is not empty")
            {
                REQUIRE(!select.empty());
            }

            THEN("its begin points to the str")
            {
                REQUIRE((*select.begin()).get() == str1ptr);
            }

            WHEN("another three OptionElements are inserted at begin two at end")
            {
                auto str2 = make_element<OptionElement>(make_element<StringElement>("b"));
                auto str3 = make_element<OptionElement>(make_element<StringElement>("c"));
                auto str4 = make_element<OptionElement>(make_element<StringElement>("d"));

                select.insert(select.begin(), std::move(str2));
                select.insert(select.begin(), std::move(str3));
                select.insert(select.begin(), std::move(str4));

                auto str5 = make_element<OptionElement>(make_element<StringElement>("e"));
                auto str6 = make_element<OptionElement>(make_element<StringElement>("f"));

                select.insert(select.begin(), std::move(str5));
                select.insert(select.begin(), std::move(str6));

                THEN("its size is 6")
                {
                    REQUIRE(select.size() == 6);
                }

                WHEN("the third Element is erased")
                {
                    auto it_ = select.begin();
                    std::advance(it_, 2);
                    auto it = select.erase(it_);

                    THEN("its size is 5")
                    {
                        REQUIRE(select.size() == 5);
                    }

                    THEN("the resulting iterator points at the newly third element")
                    {
                        REQUIRE(it == std::next(select.begin(), 2));
                    }
                }

                WHEN("all but the first Element are erased")
                {
                    auto it = select.erase(std::next(select.begin()), select.end());

                    THEN("its size is 1")
                    {
                        REQUIRE(select.size() == 1);
                    }

                    THEN("the resulting iterator is end")
                    {
                        REQUIRE(it == select.end());
                    }
                }
            }

            WHEN("another OptionElement is pushed back")
            {
                auto str2 = make_element<OptionElement>(make_element<StringElement>("bar"));
                auto str2ptr = str2.get();

                select.push_back(std::move(str2));

                THEN("its size is 2")
                {
                    REQUIRE(select.size() == 2);
                }

                THEN("the begin is 2 away from the end")
                {
                    REQUIRE(select.end() - select.begin() == 2);
                }

                THEN("it is not empty")
                {
                    REQUIRE(!select.empty());
                }

                THEN("its first two elements are the given strs")
                {
                    REQUIRE(select.begin()[0].get() == str1ptr);
                    REQUIRE(select.begin()[1].get() == str2ptr);
                }
            }
        }
    }
}

SCENARIO("`Select` is move-constructed from elements", "[ElementData][Select]")
{
    GIVEN("Three StringElement instances")
    {
        auto str1 = make_element<OptionElement>(make_element<StringElement>("a"));
        auto str1Clone = make_element<OptionElement>(make_element<StringElement>("a"));
        const auto str1ptr = str1.get();

        auto str2 = make_element<OptionElement>(make_element<StringElement>("b"));
        auto str2Clone = make_element<OptionElement>(make_element<StringElement>("b"));
        const auto str2ptr = str2.get();

        auto str3 = make_element<OptionElement>(make_element<StringElement>("c"));
        auto str3Clone = make_element<OptionElement>(make_element<StringElement>("c"));
        const auto str3ptr = str3.get();

        WHEN("An Select is constructed from them")
        {
            Select select{ std::move(str1), std::move(str2), std::move(str3) };

            THEN("its size is three")
            {
                REQUIRE(select.size() == 3);
            }
            THEN("its begin is 3 away from its end")
            {
                REQUIRE(select.end() - select.begin() == 3);
            }
            THEN("it is not empty")
            {
                REQUIRE(!select.empty());
            }
            THEN("its members were obtained by moving in the original strs")
            {
                REQUIRE(select.begin()[0].get() == str1ptr);
                REQUIRE(select.begin()[1].get() == str2ptr);
                REQUIRE(select.begin()[2].get() == str3ptr);
            }
        }

        WHEN("an Select is created from them")
        {
            Select select{ std::move(str1), std::move(str2), std::move(str3) };

            THEN("its size is three")
            {
                REQUIRE(select.size() == 3);
            }
            THEN("its begin is 3 away from its end")
            {
                REQUIRE(select.end() - select.begin() == 3);
            }
            THEN("it is not empty")
            {
                REQUIRE(!select.empty());
            }
            THEN("its first three elements are the given strs")
            {
                REQUIRE(select.begin()[0].get() == str1ptr);
                REQUIRE(select.begin()[1].get() == str2ptr);
                REQUIRE(select.begin()[2].get() == str3ptr);
            }

            WHEN("another Select is copy constructed from it")
            {
                Select select2(select);

                THEN("its size is three")
                {
                    REQUIRE(select2.size() == 3);
                }
                THEN("its begin is 3 away from its end")
                {
                    REQUIRE(select2.end() - select2.begin() == 3);
                }
                THEN("the original's size is three")
                {
                    REQUIRE(select.size() == 3);
                }
                THEN("it is not empty")
                {
                    REQUIRE(!select2.empty());
                }
                THEN("the original is not empty")
                {
                    REQUIRE(!select.empty());
                }
                THEN("its size equals the original's size")
                {
                    REQUIRE(select.size() == select2.size());
                }
                THEN("their entries are equal")
                {
                    REQUIRE(std::equal(select2.begin(),
                        select2.end(),
                        select.begin(),
                        [](const std::unique_ptr<OptionElement>& e1, const std::unique_ptr<OptionElement>& e2) {
                            return *e1 == *e2;
                        }));
                }
            }

            WHEN("another Select is move constructed from it")
            {
                Select select2(std::move(select));

                THEN("its size is three")
                {
                    REQUIRE(select2.size() == 3);
                }
                THEN("its begin is 3 away from its end")
                {
                    REQUIRE(select2.end() - select2.begin() == 3);
                }
                THEN("the original's begin is at its end")
                {
                    REQUIRE(select.end() == select.begin());
                }
                THEN("the original's size is zero")
                {
                    REQUIRE(select.size() == 0);
                }
                THEN("the original is empty")
                {
                    REQUIRE(select.empty());
                }
                THEN("it is not empty")
                {
                    REQUIRE(!select2.empty());
                }
                THEN("its entries are the original strs")
                {
                    REQUIRE(select2.begin()[0].get() == str1ptr);
                    REQUIRE(select2.begin()[1].get() == str2ptr);
                    REQUIRE(select2.begin()[2].get() == str3ptr);
                }
            }
        }
    }
}

SCENARIO("select DSDs are tested for equality and inequality", "[Element][Select][equality]")
{
    GIVEN("An select DSD with some members")
    {
        auto make_test_data = []() {
            return Select(                                 //
                make_element<OptionElement>(               //
                    make_element<MemberElement>("theist",  //
                        make_empty<StringElement>())       //
                    ),                                     //
                make_element<OptionElement>(               //
                    make_element<MemberElement>("atheist", //
                        make_empty<StringElement>())       //
                    )                                      //
            );
        };

        Select data = make_test_data();

        GIVEN("An select element constructed equivalently")
        {
            Select data2 = make_test_data();

            THEN("they test positive for equality")
            {
                REQUIRE(data == data2);
            }

            THEN("they test negative for inequality")
            {
                REQUIRE(!(data != data2));
            }
        }

        GIVEN("An empty select element")
        {
            Select data2;

            THEN("they test negative for equality")
            {
                REQUIRE(!(data == data2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(data != data2);
            }
        }

        GIVEN("An select element containing additional elements")
        {
            Select data2 = make_test_data();
            data2.push_back(make_element<OptionElement>( //
                make_element<MemberElement>("catalyst",  //
                    make_empty<StringElement>())         //
                ));

            THEN("they test negative for equality")
            {
                REQUIRE(!(data == data2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(data != data2);
            }
        }

        GIVEN("An select element containing less elements")
        {
            Select data2(                                 //
                make_element<OptionElement>(              //
                    make_element<MemberElement>("theist", //
                        make_empty<StringElement>())      //
                    )                                     //
            );

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
