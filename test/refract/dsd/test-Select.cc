//
//  test/refract/dsd/test-Select.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "catch.hpp"

#include "ElementMock.h"

#include "refract/dsd/Select.h"

#include "refract/Element.h"

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

        WHEN("an ElementMock is pushed back")
        {
            auto mock = std::make_unique<test::ElementMock>();
            auto mock1ptr = mock.get();

            REQUIRE(test::ElementMock::instances().size() == 1);

            auto mock1Option = make_element<OptionElement>(std::move(mock));
            auto mock1OptionPtr = mock1Option.get();
            select.push_back(std::move(mock1Option));

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
                REQUIRE((*select.begin()).get() == mock1OptionPtr);
            }

            THEN("there were no method calls to the mock")
            {
                REQUIRE(mock1ptr->_total_ctx == 0);
            }

            THEN("there still is just one mock instance")
            {
                REQUIRE(test::ElementMock::instances().size() == 1);
            }

            WHEN("another three ElementMocks are inserted at begin two at end")
            {
                auto mock2 = std::make_unique<test::ElementMock>();
                auto mock2ptr = mock.get();

                auto mock3 = std::make_unique<test::ElementMock>();
                auto mock3ptr = mock.get();

                auto mock4 = std::make_unique<test::ElementMock>();
                auto mock4ptr = mock.get();

                select.insert(select.begin(), make_element<OptionElement>(std::move(mock2)));
                select.insert(select.begin(), make_element<OptionElement>(std::move(mock3)));
                select.insert(select.begin(), make_element<OptionElement>(std::move(mock4)));

                auto mock5 = std::make_unique<test::ElementMock>();
                auto mock5ptr = mock.get();

                auto mock6 = std::make_unique<test::ElementMock>();
                auto mock6ptr = mock.get();

                select.insert(select.begin(), make_element<OptionElement>(std::move(mock5)));
                select.insert(select.begin(), make_element<OptionElement>(std::move(mock6)));

                REQUIRE(test::ElementMock::instances().size() == 6);

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

                    THEN("only 5 instances of ElementMock exist")
                    {
                        REQUIRE(test::ElementMock::instances().size() == 5);
                    }

                    THEN("none of the existing instances of ElementMock is the erased Element")
                    {
                        const auto& instances = test::ElementMock::instances();
                        REQUIRE(std::find_if(instances.begin(),
                                    instances.end(),
                                    [mock5ptr](const test::ElementMock* el) {
                                        assert(el);
                                        return el == mock5ptr;
                                    })
                            == instances.end());
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

            WHEN("another ElementMock is pushed back")
            {
                auto mock2 = std::make_unique<test::ElementMock>();
                auto mock2ptr = mock2.get();

                auto mock2Option = make_element<OptionElement>(std::move(mock2));
                auto mock2OptionPtr = mock2Option.get();

                REQUIRE(test::ElementMock::instances().size() == 2);

                select.push_back(std::move(mock2Option));

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
                    REQUIRE(select.begin()[0].get() == mock1OptionPtr);
                    REQUIRE(select.begin()[1].get() == mock2OptionPtr);
                }

                THEN("there were no method calls to the mocks")
                {
                    REQUIRE(mock1ptr->_total_ctx == 0);
                    REQUIRE(mock2ptr->_total_ctx == 0);
                }

                THEN("there still are just two mock instances")
                {
                    REQUIRE(test::ElementMock::instances().size() == 2);
                }

                THEN("it is iterable")
                {
                    auto mocks = std::array<const OptionElement*, 2>{ {
                        mock1OptionPtr, //
                        mock2OptionPtr  //
                    } };

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
    GIVEN("Three ElementMock instances")
    {
        REQUIRE(test::ElementMock::instances().size() == 0);

        auto mock1 = std::make_unique<test::ElementMock>();
        const auto mock1ptr = mock1.get();
        auto mock1Option = make_element<OptionElement>(std::move(mock1));
        const auto mock1OptionPtr = mock1Option.get();

        auto mock2 = std::make_unique<test::ElementMock>();
        const auto mock2ptr = mock2.get();
        auto mock2Option = make_element<OptionElement>(std::move(mock2));
        const auto mock2OptionPtr = mock2Option.get();

        auto mock3 = std::make_unique<test::ElementMock>();
        const auto mock3ptr = mock3.get();
        auto mock3Option = make_element<OptionElement>(std::move(mock3));
        const auto mock3OptionPtr = mock3Option.get();

        REQUIRE(test::ElementMock::instances().size() == 3);

        WHEN("An Select is constructed from them")
        {
            Select select( //
                std::move(mock1Option),
                std::move(mock2Option),
                std::move(mock3Option));

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
                auto mocks = std::array<const OptionElement*, 3>{ { //
                    mock1OptionPtr,
                    mock2OptionPtr,
                    mock3OptionPtr } };
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
            THEN("non-temporary ElementMocks were not constructed")
            {
                REQUIRE(test::ElementMock::instances().size() == 3);
            }
            THEN("its members were obtained by moving in the original mocks")
            {
                REQUIRE(mock1ptr->_total_ctx == 0);
                REQUIRE(mock2ptr->_total_ctx == 0);
                REQUIRE(mock3ptr->_total_ctx == 0);

                REQUIRE(select.begin()[0].get() == mock1OptionPtr);
                REQUIRE(select.begin()[1].get() == mock2OptionPtr);
                REQUIRE(select.begin()[2].get() == mock3OptionPtr);
            }
        }

        WHEN("an Select is created from them")
        {
            {
                Select select{ std::move(mock1Option), std::move(mock2Option), std::move(mock3Option) };

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
                    REQUIRE(select.begin()[0].get() == mock1OptionPtr);
                    REQUIRE(select.begin()[1].get() == mock2OptionPtr);
                    REQUIRE(select.begin()[2].get() == mock3OptionPtr);
                }
                THEN("it is iterable")
                {
                    auto mocks = std::array<const OptionElement*, 3>{ { //
                        mock1OptionPtr,
                        mock2OptionPtr,
                        mock3OptionPtr } };
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
                THEN("non-temporary ElementMocks were not constructed")
                {
                    REQUIRE(test::ElementMock::instances().size() == 3);
                }
                THEN("nothing was called on the mocks")
                {
                    REQUIRE(mock1ptr->_total_ctx == 0);
                    REQUIRE(mock2ptr->_total_ctx == 0);
                    REQUIRE(mock3ptr->_total_ctx == 0);
                }

                WHEN("another Select is copy constructed from it")
                {
                    auto mock1clone = new test::ElementMock{};
                    auto mock2clone = new test::ElementMock{};
                    auto mock3clone = new test::ElementMock{};
                    REQUIRE(test::ElementMock::instances().size() == 6);

                    mock1ptr->clone_out.reset(mock1clone);
                    mock2ptr->clone_out.reset(mock2clone);
                    mock3ptr->clone_out.reset(mock3clone);
                    REQUIRE(test::ElementMock::instances().size() == 6);

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
                        auto mocks = std::array<IElement*, 3>{ { mock1clone, mock2clone, mock3clone } };
                        auto mocks_it = mocks.begin();
                        int ctx = 0;
                        for (const auto& el : select2) {
                            REQUIRE(mocks_it < mocks.end()); // memory overflow
                            REQUIRE(*mocks_it == el->get().begin()[0].get());
                            ++mocks_it;
                            ++ctx;
                            REQUIRE(ctx != 0); // integer overflow
                        }
                        REQUIRE(ctx == 3);
                    }

                    THEN("non-temporary ElementMocks were not constructed")
                    {
                        REQUIRE(test::ElementMock::instances().size() == 6);
                    }

                    THEN("its members were obtained calling `IElement::clone(cAll)` on original mocks")
                    {
                        REQUIRE(mock1ptr->_total_ctx == 1);
                        REQUIRE(mock2ptr->_total_ctx == 1);
                        REQUIRE(mock3ptr->_total_ctx == 1);

                        REQUIRE(mock1ptr->clone_ctx == 1);
                        REQUIRE(mock2ptr->clone_ctx == 1);
                        REQUIRE(mock3ptr->clone_ctx == 1);

                        REQUIRE(mock1ptr->clone_in == IElement::cAll);
                        REQUIRE(mock2ptr->clone_in == IElement::cAll);
                        REQUIRE(mock3ptr->clone_in == IElement::cAll);

                        REQUIRE(select2.begin()[0]->get().begin()[0].get() == mock1clone);
                        REQUIRE(select2.begin()[1]->get().begin()[0].get() == mock2clone);
                        REQUIRE(select2.begin()[2]->get().begin()[0].get() == mock3clone);
                    }
                }

                WHEN("another Select is move constructed from it")
                {
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
                            auto mocks = std::array<const OptionElement*, 3>{ { //
                                mock1OptionPtr,
                                mock2OptionPtr,
                                mock3OptionPtr } };
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
                        THEN("non-temporary ElementMocks were not constructed")
                        {
                            REQUIRE(test::ElementMock::instances().size() == 3);
                        }
                        THEN("its members are the original mocks")
                        {
                            REQUIRE(select2.begin()[0]->get().begin()[0].get() == mock1ptr);
                            REQUIRE(select2.begin()[1]->get().begin()[0].get() == mock2ptr);
                            REQUIRE(select2.begin()[2]->get().begin()[0].get() == mock3ptr);
                        }
                    }
                    WHEN("it goes out of scope")
                    {
                        THEN("no mocks are left")
                        {
                            REQUIRE(test::ElementMock::instances().empty());
                        }
                    }
                }
            }
            WHEN("it goes out of scope")
            {
                THEN("no mocks are left")
                {
                    REQUIRE(test::ElementMock::instances().empty());
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
