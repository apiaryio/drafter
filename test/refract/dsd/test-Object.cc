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

            THEN("the original Object is empty")
            {
                REQUIRE(object.empty());
            }

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
                REQUIRE(object.empty());
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

        WHEN("a StringElement is pushed back")
        {
            auto str = make_element<StringElement>();
            auto str1ptr = str.get();

            object.push_back(std::move(str));

            THEN("its size is 1")
            {
                REQUIRE(object.size() == 1);
            }

            THEN("it is not empty")
            {
                REQUIRE(!object.empty());
            }

            THEN("its begin points to the str")
            {
                REQUIRE((*object.begin()).get() == str1ptr);
            }

            WHEN("another three StringElements are inserted at begin two at end")
            {
                auto str2 = make_element<StringElement>();
                auto str3 = make_element<StringElement>();
                auto str4 = make_element<StringElement>();

                object.insert(object.begin(), std::move(str2));
                object.insert(object.begin(), std::move(str3));
                object.insert(object.begin(), std::move(str4));

                auto str5 = make_element<StringElement>();
                auto str6 = make_element<StringElement>();

                object.insert(object.begin(), std::move(str5));
                object.insert(object.begin(), std::move(str6));

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

            WHEN("another StringElement is pushed back")
            {
                auto str2 = make_element<StringElement>();
                auto str2ptr = str2.get();

                object.push_back(std::move(str2));

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

                THEN("its first two elements are the given strs")
                {
                    REQUIRE(object.begin()[0].get() == str1ptr);
                    REQUIRE(object.begin()[1].get() == str2ptr);
                }
            }
        }
    }
}

SCENARIO("`Object` is move-constructed from elements", "[ElementData][Object]")
{
    GIVEN("Three StringElement instances")
    {
        auto str1 = make_element<StringElement>("a");
        auto str2 = make_element<StringElement>("b");
        auto str3 = make_element<StringElement>("c");

        auto str1clone = make_element<StringElement>("a");
        auto str2clone = make_element<StringElement>("b");
        auto str3clone = make_element<StringElement>("c");

        const auto str1ptr = str1.get();
        const auto str2ptr = str2.get();
        const auto str3ptr = str3.get();

        WHEN("An Object is constructed from them")
        {
            Object object(std::move(str1), std::move(str2), std::move(str3));

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
            THEN("its members were obtained by moving in the original strs")
            {
                REQUIRE(object.begin()[0].get() == str1ptr);
                REQUIRE(object.begin()[1].get() == str2ptr);
                REQUIRE(object.begin()[2].get() == str3ptr);
            }
        }

        WHEN("an Object is created from them")
        {
            Object object{ std::move(str1), std::move(str2), std::move(str3) };

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
            THEN("its first three elements are the given strs")
            {
                REQUIRE(object.begin()[0].get() == str1ptr);
                REQUIRE(object.begin()[1].get() == str2ptr);
                REQUIRE(object.begin()[2].get() == str3ptr);
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
                THEN("their entries are equal")
                {
                    REQUIRE(std::equal(object2.begin(),
                        object2.end(),
                        object.begin(),
                        [](const std::unique_ptr<IElement>& e1, const std::unique_ptr<IElement>& e2) {
                            return *e1 == *e2;
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
                THEN("its members are the original strs")
                {
                    REQUIRE(object2.begin()[0].get() == str1ptr);
                    REQUIRE(object2.begin()[1].get() == str2ptr);
                    REQUIRE(object2.begin()[2].get() == str3ptr);
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
