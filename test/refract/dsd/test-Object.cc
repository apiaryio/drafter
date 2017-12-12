//
//  test/refract/dsd/test-Object.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "catch.hpp"

#include "refract/dsd/Object.h"

#include "refract/Element.h"
#include "ElementMock.h"

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

        WHEN("an ElementMock is pushed back")
        {
            auto mock = std::make_unique<test::ElementMock>();
            auto mock1ptr = mock.get();

            REQUIRE(test::ElementMock::instances().size() == 1);

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

                object.insert(object.begin(), std::move(mock2));
                object.insert(object.begin(), std::move(mock3));
                object.insert(object.begin(), std::move(mock4));

                auto mock5 = std::make_unique<test::ElementMock>();
                auto mock5ptr = mock.get();

                auto mock6 = std::make_unique<test::ElementMock>();
                auto mock6ptr = mock.get();

                object.insert(object.begin(), std::move(mock5));
                object.insert(object.begin(), std::move(mock6));

                REQUIRE(test::ElementMock::instances().size() == 6);

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
                    }

                    THEN("the resulting iterator points at the newly third element")
                    {
                        REQUIRE(it == std::next(object.begin(), 2));
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

            WHEN("another ElementMock is pushed back")
            {
                auto mock2 = std::make_unique<test::ElementMock>();
                auto mock2ptr = mock2.get();

                REQUIRE(test::ElementMock::instances().size() == 2);

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
                    auto mocks = std::array<IElement*, 2>{ { mock1ptr, mock2ptr } };
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
    GIVEN("Three ElementMock instances")
    {
        REQUIRE(test::ElementMock::instances().size() == 0);

        auto mock1 = std::make_unique<test::ElementMock>();
        auto mock2 = std::make_unique<test::ElementMock>();
        auto mock3 = std::make_unique<test::ElementMock>();

        const auto mock1ptr = mock1.get();
        const auto mock2ptr = mock2.get();
        const auto mock3ptr = mock3.get();

        REQUIRE(test::ElementMock::instances().size() == 3);

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
                auto mock_ptrs = std::array<const IElement*, 3>{ { mock1ptr, mock2ptr, mock3ptr } };
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
            THEN("non-temporary ElementMocks were not constructed")
            {
                REQUIRE(test::ElementMock::instances().size() == 3);
            }
            THEN("its members were obtained by moving in the original mocks")
            {
                REQUIRE(mock1ptr->_total_ctx == 0);
                REQUIRE(mock2ptr->_total_ctx == 0);
                REQUIRE(mock3ptr->_total_ctx == 0);

                REQUIRE(object.begin()[0].get() == mock1ptr);
                REQUIRE(object.begin()[1].get() == mock2ptr);
                REQUIRE(object.begin()[2].get() == mock3ptr);
            }
        }

        WHEN("an Object is created from them")
        {
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
                    auto mocks = std::array<IElement*, 3>{ { mock1ptr, mock2ptr, mock3ptr } };
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

                WHEN("another Object is copy constructed from it")
                {
                    auto mock1clone = new test::ElementMock{};
                    auto mock2clone = new test::ElementMock{};
                    auto mock3clone = new test::ElementMock{};
                    REQUIRE(test::ElementMock::instances().size() == 6);

                    mock1ptr->clone_out.reset(mock1clone);
                    mock2ptr->clone_out.reset(mock2clone);
                    mock3ptr->clone_out.reset(mock3clone);
                    REQUIRE(test::ElementMock::instances().size() == 6);

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
                        auto mocks = std::array<IElement*, 3>{ { mock1clone, mock2clone, mock3clone } };
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

                        REQUIRE(object2.begin()[0].get() == mock1clone);
                        REQUIRE(object2.begin()[1].get() == mock2clone);
                        REQUIRE(object2.begin()[2].get() == mock3clone);
                    }
                }

                WHEN("another Object is move constructed from it")
                {
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
                            auto mocks = std::array<IElement*, 3>{ { mock1ptr, mock2ptr, mock3ptr } };
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
                        THEN("non-temporary ElementMocks were not constructed")
                        {
                            REQUIRE(test::ElementMock::instances().size() == 3);
                        }
                        THEN("its members are the original mocks")
                        {
                            REQUIRE(object2.begin()[0].get() == mock1ptr);
                            REQUIRE(object2.begin()[1].get() == mock2ptr);
                            REQUIRE(object2.begin()[2].get() == mock3ptr);
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

SCENARIO("object DSDs are tested for equality and inequality", "[Element][Object][equality]")
{
    GIVEN("An object DSD with some members")
    {
        auto make_test_data = []() {
            return Object( //
                make_element<MemberElement>("foo", from_primitive("bar")),
                make_element<MemberElement>("baz", make_element<RefElement>("Boo")),
                make_element<MemberElement>("answer", from_primitive(42.0)));
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
