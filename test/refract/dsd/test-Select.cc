//
//  test/refract/dsd/test-Select.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include <catch2/catch.hpp>

#include <refract/dsd/Select.h>
#include <refract/dsd/Option.h>
#include <refract/Element.h>

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

        WHEN("an option element is pushed back")
        {
            auto mock = make_element<OptionElement>(from_primitive("abc"));
            auto mock1ptr = mock.get();

            select.push_back(std::move(mock));

            THEN("its size is 1")
            {
                REQUIRE(select.size() == 1);
            }

            THEN("it is not empty")
            {
                REQUIRE(!select.empty());
            }

            THEN("its begin points to the mock")
            {
                REQUIRE((*select.begin()).get() == mock1ptr);
            }

            WHEN("another three elements are inserted at begin two at end")
            {
                auto mock2 = make_element<OptionElement>(from_primitive(2));
                auto mock2ptr = mock2.get();
                auto mock3 = make_element<OptionElement>(from_primitive(3));
                auto mock3ptr = mock3.get();
                auto mock4 = make_element<OptionElement>(from_primitive(4));
                auto mock4ptr = mock4.get();

                select.insert(select.begin(), std::move(mock2));
                select.insert(select.begin(), std::move(mock3));
                select.insert(select.begin(), std::move(mock4));

                auto mock5 = make_element<OptionElement>(from_primitive(5));
                auto mock5ptr = mock5.get();

                auto mock6 = make_element<OptionElement>(from_primitive(6));
                auto mock6ptr = mock6.get();

                select.insert(select.end(), std::move(mock5));
                select.insert(select.end(), std::move(mock6));

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

                        THEN("other instances are still contained")
                        {
                            REQUIRE(select.begin()[0].get() == mock4ptr);
                            REQUIRE(select.begin()[1].get() == mock3ptr);
                            REQUIRE(select.begin()[2].get() == mock1ptr);
                            REQUIRE(select.begin()[3].get() == mock5ptr);
                            REQUIRE(select.begin()[4].get() == mock6ptr);
                        }

                        THEN("the resulting iterator points at the newly third element")
                        {
                            REQUIRE(it == std::next(select.begin(), 2));
                        }
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

            WHEN("another element is pushed back")
            {
                auto mock2 = make_element<OptionElement>(from_primitive("tralala"));
                auto mock2ptr = mock2.get();

                select.push_back(std::move(mock2));

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

                THEN("its first two elements are the given mocks")
                {
                    REQUIRE(select.begin()[0].get() == mock1ptr);
                    REQUIRE(select.begin()[1].get() == mock2ptr);
                }

                THEN("it is iterable")
                {
                    auto mocks = std::array<IElement*, 2>{ mock1ptr, mock2ptr };
                    auto mocks_it = mocks.begin();
                    int ctx = 0;
                    for (const auto& el : select) {
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

SCENARIO("`Select` is move-constructed from elements", "[ElementData][Select]")
{
    GIVEN("Three elements")
    {
        auto mock1 = make_element<OptionElement>(from_primitive("foo"));
        auto mock2 = make_element<OptionElement>(from_primitive("bar"));
        auto mock3 = make_element<OptionElement>(from_primitive("baz"));

        const auto mock1ptr = mock1.get();
        const auto mock2ptr = mock2.get();
        const auto mock3ptr = mock3.get();

        WHEN("An Select is constructed from them")
        {
            Select select(std::move(mock1), std::move(mock2), std::move(mock3));

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
            THEN("it is iterable")
            {
                auto mock_ptrs = std::array<const IElement*, 3>{ mock1ptr, mock2ptr, mock3ptr };
                auto mock_ptrs_it = mock_ptrs.begin();
                int ctx = 0;
                for (const auto& el : select) {
                    REQUIRE(mock_ptrs_it < mock_ptrs.end()); // memory overflow
                    REQUIRE(*mock_ptrs_it == el.get());
                    ++mock_ptrs_it;
                    ++ctx;
                    REQUIRE(ctx != 0); // integer overflow
                }
                REQUIRE(ctx == 3);
            }
        }

        WHEN("an Select is created from them")
        {
            Select select{ std::move(mock1), std::move(mock2), std::move(mock3) };

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
            THEN("its first three elements are the given mocks")
            {
                REQUIRE(select.begin()[0].get() == mock1ptr);
                REQUIRE(select.begin()[1].get() == mock2ptr);
                REQUIRE(select.begin()[2].get() == mock3ptr);
            }
            THEN("it is iterable")
            {
                auto mocks = std::array<IElement*, 3>{ mock1ptr, mock2ptr, mock3ptr };
                auto mocks_it = mocks.begin();
                int ctx = 0;
                for (const auto& el : select) {
                    REQUIRE(mocks_it < mocks.end()); // memory overflow
                    REQUIRE(*mocks_it == el.get());
                    ++mocks_it;
                    ++ctx;
                    REQUIRE(ctx != 0); // integer overflow
                }
                REQUIRE(ctx == 3);
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
                THEN("it is iterable")
                {
                    int ctx = 0;
                    for (const auto& el : select2) {
                        REQUIRE(el);
                        ++ctx;
                        REQUIRE(ctx != 0); // integer overflow
                    }
                    REQUIRE(ctx == 3);
                }

                THEN("its members are equal")
                {
                    REQUIRE(select.size() == select2.size());
                    REQUIRE(std::equal(select.begin(), select.end(), select2.begin(), [](const auto& a, const auto& b) {
                        return *a == *b;
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
                THEN("it is iterable")
                {
                    auto mocks = std::array<IElement*, 3>{ mock1ptr, mock2ptr, mock3ptr };
                    auto mocks_it = mocks.begin();
                    int ctx = 0;
                    for (const auto& el : select2) {
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
                    REQUIRE(select2.begin()[0].get() == mock1ptr);
                    REQUIRE(select2.begin()[1].get() == mock2ptr);
                    REQUIRE(select2.begin()[2].get() == mock3ptr);
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
