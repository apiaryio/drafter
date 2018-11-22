//
//  test/refract/dsd/test-Extend.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include <catch2/catch.hpp>

#include "refract/dsd/Extend.h"
#include "refract/Element.h"
#include "refract/PrintVisitor.h"

#include <array>

using namespace refract;
using namespace dsd;

TEST_CASE("`Extend`'s default element name is `extend`", "[Element][Extend]")
{
    REQUIRE(std::string(Extend::name) == "extend");
}

SCENARIO("`Extend` is default constructed and both copy- and move constructed from", "[ElementData][Extend]")
{
    GIVEN("A default initialized Extend")
    {
        Extend extend;

        THEN("it is empty")
        {
            REQUIRE(extend.empty());
        }

        THEN("its size is zero")
        {
            REQUIRE(extend.size() == 0);
        }

        THEN("its begin is equal to its end")
        {
            REQUIRE(extend.begin() == extend.end());
        }

        WHEN("from it another Extend is copy constructed")
        {
            Extend extend2(extend);

            THEN("the latter Extend is also empty")
            {
                REQUIRE(extend2.empty());
            }
        }

        WHEN("from it another Extend is copy constructed")
        {
            Extend extend2(std::move(extend));

            THEN("the original Extend is empty")
            {
                REQUIRE(extend2.empty());
            }

            THEN("the latter Extend is also empty")
            {
                REQUIRE(extend2.empty());
            }
        }
    }
}

SCENARIO("`Extend` is inserted to and erased from", "[ElementData][Extend]")
{
    GIVEN("A default initialized `Extend`")
    {
        Extend extend;

        WHEN("an StringElement is pushed back")
        {
            auto mock = from_primitive("abc");
            auto mock1ptr = mock.get();

            extend.push_back(std::move(mock));

            THEN("its size is 1")
            {
                REQUIRE(extend.size() == 1);
            }

            THEN("it is not empty")
            {
                REQUIRE(!extend.empty());
            }

            THEN("its begin points to the mock")
            {
                REQUIRE((*extend.begin()).get() == mock1ptr);
            }

            WHEN("another three elements are inserted at begin two at end")
            {
                auto mock2 = from_primitive("2");
                auto mock2ptr = mock2.get();
                auto mock3 = from_primitive("3");
                auto mock3ptr = mock3.get();
                auto mock4 = from_primitive("4");
                auto mock4ptr = mock4.get();

                extend.insert(extend.begin(), std::move(mock2));
                extend.insert(extend.begin(), std::move(mock3));
                extend.insert(extend.begin(), std::move(mock4));

                auto mock5 = from_primitive("5");
                auto mock5ptr = mock5.get();

                auto mock6 = from_primitive("6");
                auto mock6ptr = mock6.get();

                extend.insert(extend.end(), std::move(mock5));
                extend.insert(extend.end(), std::move(mock6));

                THEN("its size is 6")
                {
                    REQUIRE(extend.size() == 6);
                }

                WHEN("the third Element is erased")
                {
                    auto it_ = extend.begin();
                    std::advance(it_, 2);
                    auto it = extend.erase(it_);

                    THEN("its size is 5")
                    {
                        REQUIRE(extend.size() == 5);

                        THEN("other instances are still contained")
                        {
                            REQUIRE(extend.begin()[0].get() == mock4ptr);
                            REQUIRE(extend.begin()[1].get() == mock3ptr);
                            REQUIRE(extend.begin()[2].get() == mock1ptr);
                            REQUIRE(extend.begin()[3].get() == mock5ptr);
                            REQUIRE(extend.begin()[4].get() == mock6ptr);
                        }

                        THEN("the resulting iterator points at the newly third element")
                        {
                            REQUIRE(it == std::next(extend.begin(), 2));
                        }
                    }
                }

                WHEN("all but the first Element are erased")
                {
                    auto it = extend.erase(std::next(extend.begin()), extend.end());

                    THEN("its size is 1")
                    {
                        REQUIRE(extend.size() == 1);
                    }

                    THEN("the resulting iterator is end")
                    {
                        REQUIRE(it == extend.end());
                    }
                }
            }

            WHEN("another element is pushed back")
            {
                auto mock2 = from_primitive("tralala");
                auto mock2ptr = mock2.get();

                extend.push_back(std::move(mock2));

                THEN("its size is 2")
                {
                    REQUIRE(extend.size() == 2);
                }

                THEN("the begin is 2 away from the end")
                {
                    REQUIRE(extend.end() - extend.begin() == 2);
                }

                THEN("it is not empty")
                {
                    REQUIRE(!extend.empty());
                }

                THEN("its first two elements are the given mocks")
                {
                    REQUIRE(extend.begin()[0].get() == mock1ptr);
                    REQUIRE(extend.begin()[1].get() == mock2ptr);
                }

                THEN("it is iterable")
                {
                    auto mocks = std::array<IElement*, 2>{ mock1ptr, mock2ptr };
                    auto mocks_it = mocks.begin();
                    int ctx = 0;
                    for (const auto& el : extend) {
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

SCENARIO("`Extend` is move-constructed from elements", "[ElementData][Extend]")
{
    GIVEN("Three elements")
    {
        auto mock1 = from_primitive("foo");
        auto mock2 = from_primitive("bar");
        auto mock3 = from_primitive("baz");

        const auto mock1ptr = mock1.get();
        const auto mock2ptr = mock2.get();
        const auto mock3ptr = mock3.get();

        WHEN("An Extend is constructed from them")
        {
            Extend extend(std::move(mock1), std::move(mock2), std::move(mock3));

            THEN("its size is three")
            {
                REQUIRE(extend.size() == 3);
            }
            THEN("its begin is 3 away from its end")
            {
                REQUIRE(extend.end() - extend.begin() == 3);
            }
            THEN("it is not empty")
            {
                REQUIRE(!extend.empty());
            }
            THEN("it is iterable")
            {
                auto mock_ptrs = std::array<const IElement*, 3>{ mock1ptr, mock2ptr, mock3ptr };
                auto mock_ptrs_it = mock_ptrs.begin();
                int ctx = 0;
                for (const auto& el : extend) {
                    REQUIRE(mock_ptrs_it < mock_ptrs.end()); // memory overflow
                    REQUIRE(*mock_ptrs_it == el.get());
                    ++mock_ptrs_it;
                    ++ctx;
                    REQUIRE(ctx != 0); // integer overflow
                }
                REQUIRE(ctx == 3);
            }
        }

        WHEN("an Extend is created from them")
        {
            Extend extend{ std::move(mock1), std::move(mock2), std::move(mock3) };

            THEN("its size is three")
            {
                REQUIRE(extend.size() == 3);
            }
            THEN("its begin is 3 away from its end")
            {
                REQUIRE(extend.end() - extend.begin() == 3);
            }
            THEN("it is not empty")
            {
                REQUIRE(!extend.empty());
            }
            THEN("its first three elements are the given mocks")
            {
                REQUIRE(extend.begin()[0].get() == mock1ptr);
                REQUIRE(extend.begin()[1].get() == mock2ptr);
                REQUIRE(extend.begin()[2].get() == mock3ptr);
            }
            THEN("it is iterable")
            {
                auto mocks = std::array<IElement*, 3>{ mock1ptr, mock2ptr, mock3ptr };
                auto mocks_it = mocks.begin();
                int ctx = 0;
                for (const auto& el : extend) {
                    REQUIRE(mocks_it < mocks.end()); // memory overflow
                    REQUIRE(*mocks_it == el.get());
                    ++mocks_it;
                    ++ctx;
                    REQUIRE(ctx != 0); // integer overflow
                }
                REQUIRE(ctx == 3);
            }

            WHEN("another Extend is copy constructed from it")
            {
                Extend extend2(extend);

                THEN("its size is three")
                {
                    REQUIRE(extend2.size() == 3);
                }
                THEN("its begin is 3 away from its end")
                {
                    REQUIRE(extend2.end() - extend2.begin() == 3);
                }
                THEN("the original's size is three")
                {
                    REQUIRE(extend.size() == 3);
                }
                THEN("it is not empty")
                {
                    REQUIRE(!extend2.empty());
                }
                THEN("the original is not empty")
                {
                    REQUIRE(!extend.empty());
                }
                THEN("its size equals the original's size")
                {
                    REQUIRE(extend.size() == extend2.size());
                }
                THEN("it is iterable")
                {
                    int ctx = 0;
                    for (const auto& el : extend2) {
                        REQUIRE(el);
                        ++ctx;
                        REQUIRE(ctx != 0); // integer overflow
                    }
                    REQUIRE(ctx == 3);
                }

                THEN("its members are equal")
                {
                    REQUIRE(extend.size() == extend2.size());
                    REQUIRE(std::equal(extend.begin(), extend.end(), extend2.begin(), [](const auto& a, const auto& b) {
                        return *a == *b;
                    }));
                }
            }

            WHEN("another Extend is move constructed from it")
            {
                Extend extend2(std::move(extend));

                THEN("its size is three")
                {
                    REQUIRE(extend2.size() == 3);
                }
                THEN("its begin is 3 away from its end")
                {
                    REQUIRE(extend2.end() - extend2.begin() == 3);
                }
                THEN("the original's begin is at its end")
                {
                    REQUIRE(extend.end() == extend.begin());
                }
                THEN("the original's size is zero")
                {
                    REQUIRE(extend.size() == 0);
                }
                THEN("the original is empty")
                {
                    REQUIRE(extend.empty());
                }
                THEN("it is not empty")
                {
                    REQUIRE(!extend2.empty());
                }
                THEN("it is iterable")
                {
                    auto mocks = std::array<IElement*, 3>{ mock1ptr, mock2ptr, mock3ptr };
                    auto mocks_it = mocks.begin();
                    int ctx = 0;
                    for (const auto& el : extend2) {
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
                    REQUIRE(extend2.begin()[0].get() == mock1ptr);
                    REQUIRE(extend2.begin()[1].get() == mock2ptr);
                    REQUIRE(extend2.begin()[2].get() == mock3ptr);
                }
            }
        }
    }
}

SCENARIO("extend DSDs are tested for equality and inequality", "[Element][Extend][equality]")
{
    GIVEN("An extend DSD with some members")
    {
        auto make_test_data = []() {
            return Extend(                   //
                make_element<ObjectElement>( //
                    make_element<MemberElement>("foo", from_primitive("bar"))),
                make_element<ObjectElement>( //
                    make_element<MemberElement>("answer", from_primitive(42))));
        };

        Extend data = make_test_data();

        GIVEN("An extend element constructed equivalently")
        {
            Extend data2 = make_test_data();

            THEN("they test positive for equality")
            {
                REQUIRE(data == data2);
            }

            THEN("they test negative for inequality")
            {
                REQUIRE(!(data != data2));
            }
        }

        GIVEN("An empty extend element")
        {
            Extend data2;

            THEN("they test negative for equality")
            {
                REQUIRE(!(data == data2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(data != data2);
            }
        }

        GIVEN("An extend element containing additional elements")
        {
            Extend data2 = make_test_data();
            data2.push_back(make_element<ObjectElement>());

            THEN("they test negative for equality")
            {
                REQUIRE(!(data == data2));
            }

            THEN("they test positive for inequality")
            {
                REQUIRE(data != data2);
            }
        }

        GIVEN("An extend element containing less elements")
        {
            Extend data2(                    //
                make_element<ObjectElement>( //
                    make_element<MemberElement>("foo", from_primitive("bar"))));

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

SCENARIO("Extend::merge behaves as expected", "[Element][Extend][merge]")
{
    GIVEN("A default initialized Extend")
    {
        Extend extend;

        THEN("merging it yields nullptr")
        {
            REQUIRE(extend.merge() == nullptr);
            REQUIRE(!extend.merge());
        }
    }

    GIVEN("A Extend with two empty string elements")
    {
        Extend extend;
        extend.push_back(make_empty<StringElement>());
        extend.push_back(make_empty<StringElement>());

        WHEN("it is merged")
        {
            auto result = extend.merge();
            THEN("the result is an empty element")
            {
                REQUIRE(result->empty());
            }
            THEN("the result is a string element")
            {
                REQUIRE(dynamic_cast<StringElement*>(result.get()));
            }
        }
    }

    GIVEN("A Extend with two string elements with test values")
    {
        Extend extend;

        auto first = make_element<StringElement>("foo");
        extend.insert(extend.end(), std::move(first));

        auto last = make_element<StringElement>("bar");
        const auto& lastRef = *last;
        extend.insert(extend.end(), std::move(last));

        WHEN("it is merged")
        {
            auto result = extend.merge();
            THEN("the result is not an empty element")
            {
                REQUIRE(!result->empty());
            }
            THEN("the result is a string element")
            {
                const auto resultEl = dynamic_cast<StringElement*>(result.get());
                REQUIRE(resultEl);

                THEN("the result element is a clone of the last in the original")
                {
                    REQUIRE(resultEl->meta().size() == 0);
                    REQUIRE(resultEl->attributes().size() == 0);
                    REQUIRE(resultEl->get() == lastRef.get());
                }
            }
        }
    }

    GIVEN("A Extend with two array elements with three and two test values")
    {
        Extend extend;

        auto first = make_element<ArrayElement>();
        first->get().push_back(make_element<StringElement>("foo"));
        first->get().push_back(make_element<StringElement>("bar"));
        first->get().push_back(make_element<StringElement>("baz"));
        extend.insert(extend.end(), std::move(first));

        auto last = make_element<ArrayElement>();
        last->get().push_back(make_element<StringElement>("zur"));
        last->get().push_back(make_element<NumberElement>(42));
        extend.insert(extend.end(), std::move(last));

        WHEN("it is merged")
        {
            auto result = extend.merge();
            THEN("the result is not an empty element")
            {
                REQUIRE(!result->empty());
            }
            THEN("the result is an array element")
            {
                const auto resultEl = dynamic_cast<ArrayElement*>(result.get());
                REQUIRE(resultEl);

                THEN("the result element is a concatenation of both arrays")
                {
                    REQUIRE(resultEl->meta().size() == 0);
                    REQUIRE(resultEl->attributes().size() == 0);

                    REQUIRE(resultEl->get().size() == 5);
                }
            }
        }
    }

    GIVEN("A Extend with three empty string elements with some attributes")
    {
        auto first = make_empty<StringElement>();
        first->attributes().set("samples",
            make_element<ArrayElement>( //
                make_element<StringElement>("foo"),
                make_element<StringElement>("far")));

        auto second = make_empty<StringElement>();
        first->attributes().set("samples",
            make_element<ArrayElement>( //
                make_element<StringElement>("bar"),
                make_element<StringElement>("baz")));

        auto third = make_empty<StringElement>();
        first->attributes().set("samples",
            make_element<ArrayElement>( //
                make_element<StringElement>("zur"),
                make_element<StringElement>("zul")));

        auto extend = make_element<ExtendElement>();
        extend->get().push_back(std::move(first));
        extend->get().push_back(std::move(second));
        extend->get().push_back(std::move(third));

        WHEN("it is merged")
        {
            auto merged = extend->get().merge();

            THEN("the result is not null")
            {
                REQUIRE(merged);

                THEN("the result is a string element")
                {
                    REQUIRE(dynamic_cast<const StringElement*>(merged.get()));
                }

                THEN("the result is an empty element")
                {
                    REQUIRE(merged->empty());
                }

                THEN("the result element has one attribute")
                {
                    REQUIRE(merged->attributes().size() == 1);
                }

                THEN("the result element contains an attribute with key `samples`")
                {
                    auto attr = merged->attributes().find("samples");
                    REQUIRE(attr != merged->attributes().end());

                    const IElement* samples = attr->second.get();

                    THEN("that attribute is not nullptr")
                    {
                        REQUIRE(samples);

                        THEN("that attribute is an array element")
                        {
                            auto samplesArray = dynamic_cast<const ArrayElement*>(samples);
                            REQUIRE(samplesArray);

                            THEN("the array is not empty")
                            {
                                REQUIRE(!samplesArray->empty());
                            }

                            THEN("the array contains two elements")
                            {
                                REQUIRE(samplesArray->get().size() == 2);
                            }

                            THEN("the array contains the string elements `zur` & `zul`]")
                            {
                                const IElement* el1 = samplesArray->get().begin()[0].get();
                                REQUIRE(dynamic_cast<const StringElement*>(el1)->get() == "zur");
                                const IElement* el2 = samplesArray->get().begin()[1].get();
                                REQUIRE(dynamic_cast<const StringElement*>(el2)->get() == "zul");
                            }
                        }
                    }
                }
            }
        }
    }

    GIVEN("A Extend with three number elements")
    {
        auto extend = make_element<ExtendElement>( //
            make_element<NumberElement>(3),
            make_empty<NumberElement>(),
            make_element<NumberElement>(42));

        WHEN("it is merged")
        {
            auto merged = extend->get().merge();

            THEN("the result is not null")
            {
                REQUIRE(merged);

                THEN("the result is not empty")
                {
                    REQUIRE(!merged->empty());
                }

                THEN("the result element has no attributes")
                {
                    REQUIRE(merged->attributes().size() == 0);
                }

                THEN("the result element has no meta entries")
                {
                    REQUIRE(merged->meta().size() == 0);
                }

                THEN("the result is a number element")
                {
                    auto result = dynamic_cast<const NumberElement*>(merged.get());
                    REQUIRE(result);

                    THEN("the result element's value is the last element's")
                    {
                        REQUIRE(result->get().get() == "42");
                    }
                }
            }
        }
    }

    GIVEN("A Extend with three string elements")
    {
        auto extend = make_element<ExtendElement>( //
            make_element<StringElement>("foo"),
            make_empty<StringElement>(),
            make_element<StringElement>("42"));

        WHEN("it is merged")
        {
            auto merged = extend->get().merge();

            THEN("the result is not null")
            {
                REQUIRE(merged);

                THEN("the result is not empty")
                {
                    REQUIRE(!merged->empty());
                }

                THEN("the result element has no attributes")
                {
                    REQUIRE(merged->attributes().size() == 0);
                }

                THEN("the result element has no meta entries")
                {
                    REQUIRE(merged->meta().size() == 0);
                }

                THEN("the result is a string element")
                {
                    auto result = dynamic_cast<const StringElement*>(merged.get());
                    REQUIRE(result);

                    THEN("the result element's value is the last element's")
                    {
                        REQUIRE(result->get() == "42");
                    }
                }
            }
        }
    }

    GIVEN("A Extend with three bool elements")
    {
        auto extend = make_element<ExtendElement>( //
            make_element<BooleanElement>(false),
            make_empty<BooleanElement>(),
            make_element<BooleanElement>(true));

        WHEN("it is merged")
        {
            auto merged = extend->get().merge();

            THEN("the result is not null")
            {
                REQUIRE(merged);

                THEN("the result is not empty")
                {
                    REQUIRE(!merged->empty());
                }

                THEN("the result element has no attributes")
                {
                    REQUIRE(merged->attributes().size() == 0);
                }

                THEN("the result element has no meta entries")
                {
                    REQUIRE(merged->meta().size() == 0);
                }

                THEN("the result is a boolean element")
                {
                    auto result = dynamic_cast<const BooleanElement*>(merged.get());
                    REQUIRE(result);

                    THEN("the result element's value is the last pushed element's")
                    {
                        REQUIRE(result->get());
                    }
                }
            }
        }
    }

    GIVEN("A Extend with three ref elements")
    {
        auto extend = make_element<ExtendElement>( //
            make_element<RefElement>("Foo"),
            make_empty<RefElement>(),
            make_element<RefElement>("Bar"));

        WHEN("it is merged")
        {
            auto merged = extend->get().merge();

            THEN("the result is not null")
            {
                REQUIRE(merged);

                THEN("the result is not empty")
                {
                    REQUIRE(!merged->empty());
                }

                THEN("the result element has no attributes")
                {
                    REQUIRE(merged->attributes().size() == 0);
                }

                THEN("the result element has no meta entries")
                {
                    REQUIRE(merged->meta().size() == 0);
                }

                THEN("the result is a ref element")
                {
                    auto result = dynamic_cast<const RefElement*>(merged.get());
                    REQUIRE(result);

                    THEN("the result element's value is the last element's")
                    {
                        REQUIRE(result->get() == Ref{ "Bar" });
                    }
                }
            }
        }
    }

    GIVEN("A Extend with three array elements")
    {
        auto extend = make_element<ExtendElement>(               //
            make_element<ArrayElement>(                          //
                from_primitive("abc"),                           //
                from_primitive(5)),                              //
            make_empty<ArrayElement>(),                          //
            make_element<ArrayElement>(from_primitive("hijklm")) //
        );

        WHEN("it is merged")
        {
            auto merged = extend->get().merge();

            THEN("the result is not null")
            {
                REQUIRE(merged);

                THEN("the result is not empty")
                {
                    REQUIRE(!merged->empty());
                }

                THEN("the result element has no attributes")
                {
                    REQUIRE(merged->attributes().size() == 0);
                }

                THEN("the result element has no meta entries")
                {
                    REQUIRE(merged->meta().size() == 0);
                }

                THEN("the result is a an array element")
                {
                    auto result = dynamic_cast<const ArrayElement*>(merged.get());
                    REQUIRE(result);
                }

                THEN("the result element's value is the concatenation of all contained array elements")
                {
                    auto expected = make_element<ArrayElement>( //
                        from_primitive("abc"),
                        from_primitive(5),
                        from_primitive("hijklm"));

                    REQUIRE(*merged == *expected);
                }
            }
        }
    }

    GIVEN("A Extend with three enum elements")
    {
        auto first = make_element<EnumElement>(from_primitive("Idle"));
        first->attributes().set("enumerations",                                      //
            make_element<ArrayElement>(                                              //
                make_element<StringElement>("Idle"),                                 //
                make_element<ObjectElement>(                                         //
                    make_element<MemberElement>("id", make_empty<NumberElement>()),  //
                    make_element<MemberElement>("name", make_empty<StringElement>()) //
                    ),                                                               //
                make_empty<NumberElement>()                                          //
                ));

        auto second = make_element<EnumElement>(make_element<ObjectElement>( //
            make_element<MemberElement>("id", from_primitive(42)),           //
            make_element<MemberElement>("name", from_primitive("Thomas"))    //
            ));
        second->attributes().set("enumerations",                                     //
            make_element<ArrayElement>(                                              //
                make_element<ObjectElement>(                                         //
                    make_element<MemberElement>("id", make_empty<NumberElement>()),  //
                    make_element<MemberElement>("name", make_empty<StringElement>()) //
                    ),                                                               //
                make_element<NumberElement>(42)                                      //
                ));

        auto third = make_element<EnumElement>(make_element<NumberElement>(3));

        auto extend = make_element<ExtendElement>( //
            std::move(first),                      //
            std::move(second),                     //
            std::move(third)                       //
        );

        WHEN("it is merged")
        {
            auto merged = extend->get().merge();

            THEN("the result is not null")
            {
                REQUIRE(merged);

                THEN("the result is not empty")
                {
                    REQUIRE(!merged->empty());
                }

                THEN("the result element has one attribute")
                {
                    REQUIRE(merged->attributes().size() == 1);
                }

                THEN("the result element has no meta entries")
                {
                    REQUIRE(merged->meta().size() == 0);
                }

                THEN("the result is a an enum element")
                {
                    auto result = dynamic_cast<const EnumElement*>(merged.get());
                    REQUIRE(result);

                    THEN("the result element's value is the last element's")
                    {
                        REQUIRE(result->get() == Enum{ make_element<NumberElement>(3) });
                    }
                }

                THEN("the result has an enumerations attribute")
                {
                    auto it = merged->attributes().find("enumerations");
                    REQUIRE(it != merged->attributes().end());

                    THEN("that attribute is a set of the original element's enumerations")
                    {
                        auto expected = make_element<ArrayElement>(                              //
                            make_element<StringElement>("Idle"),                                 //
                            make_empty<NumberElement>(),                                         //
                            make_element<ObjectElement>(                                         //
                                make_element<MemberElement>("id", make_empty<NumberElement>()),  //
                                make_element<MemberElement>("name", make_empty<StringElement>()) //
                                ),                                                               //
                            make_element<NumberElement>(42)                                      //
                        );

                        REQUIRE(*it->second == *expected);
                    }
                }
            }
        }
    }

    GIVEN("A Extend with three object elements")
    {
        auto extend = make_element<ExtendElement>(

            // FIRST
            make_element<ObjectElement>(

                // Include
                make_element<RefElement>("Ipsum"), //

                // One of
                make_element<SelectElement>(            //
                    make_element<OptionElement>(        //
                        make_element<MemberElement>(    //
                            from_primitive("state"),    //
                            nullptr)                    //
                        ),                              //
                    make_element<OptionElement>(        //
                        make_element<MemberElement>(    //
                            from_primitive("province"), //
                            nullptr)                    //
                        )                               //
                    ),                                  //

                // members
                make_element<MemberElement>(      //
                    "foo",                        //
                    from_primitive(42)            //
                    ),                            //
                make_element<MemberElement>(      //
                    "bar",                        //
                    make_empty<StringElement>()), //
                make_element<MemberElement>(      //
                    "zoo",                        //
                    make_element<ArrayElement>(   //
                        from_primitive("lorem"),  //
                        from_primitive(5))        //
                    )                             //
                ),                                //

            // SECOND
            make_element<ObjectElement>(

                // Include
                make_element<RefElement>("Ipsum"),   //
                make_element<RefElement>("Dolorem"), //

                // One of
                make_element<SelectElement>(             //
                    make_element<OptionElement>(         //
                        make_element<MemberElement>(     //
                            from_primitive("theist"),    //
                            make_empty<StringElement>()) //
                        ),                               //
                    make_element<OptionElement>(         //
                        make_element<MemberElement>(     //
                            from_primitive("atheist"),   //
                            make_empty<StringElement>()) //
                        )                                //
                    ),                                   //

                // members
                make_element<MemberElement>(              //
                    "state",                              //
                    from_primitive("Prague, the capital") //
                    ),                                    //
                make_element<MemberElement>(              //
                    "bar",                                //
                    from_primitive("quab")                //
                    ),                                    //
                make_element<MemberElement>(              //
                    "lorem",                              //
                    from_primitive("ipsum")               //
                    )                                     //
                ),                                        //

            // THIRD
            make_empty<ObjectElement>() //
        );

        WHEN("it is merged")
        {
            auto merged = extend->get().merge();

            THEN("the result is not null")
            {
                REQUIRE(merged);

                THEN("the result is not empty")
                {
                    REQUIRE(!merged->empty());
                }

                THEN("the result element has no attributes")
                {
                    REQUIRE(merged->attributes().size() == 0);
                }

                THEN("the result element has no meta entries")
                {
                    REQUIRE(merged->meta().size() == 0);
                }

                auto result = dynamic_cast<const ObjectElement*>(merged.get());
                THEN("the result is a an object element")
                {
                    REQUIRE(result);
                }

                THEN("the result contains two ref elements")
                {
                    auto ipsum = make_element<RefElement>("Ipsum");
                    auto dolorem = make_element<RefElement>("Dolorem");

                    REQUIRE(std::find_if( //
                                result->get().begin(),
                                result->get().end(),
                                [&ipsum](const auto& elptr) { return *elptr == *ipsum; })
                        != result->get().end());

                    REQUIRE(std::find_if( //
                                result->get().begin(),
                                result->get().end(),
                                [&dolorem](const auto& elptr) { return *elptr == *dolorem; })
                        != result->get().end());

                    REQUIRE(
                        std::count_if( //
                            result->get().begin(),
                            result->get().end(),
                            [](const auto& elptr) { return nullptr != dynamic_cast<const RefElement*>(elptr.get()); })
                        == 2);

                    THEN("the result contains one select element")
                    {
                        auto expected = make_element<SelectElement>( //
                            make_element<OptionElement>(             //
                                make_element<MemberElement>(         //
                                    from_primitive("theist"),        //
                                    make_empty<StringElement>())     //
                                ),                                   //
                            make_element<OptionElement>(             //
                                make_element<MemberElement>(         //
                                    from_primitive("atheist"),       //
                                    make_empty<StringElement>())     //
                                )                                    //
                        );                                           //

                        REQUIRE(std::find_if( //
                                    result->get().begin(),
                                    result->get().end(),
                                    [&expected](const auto& elptr) { return *elptr == *expected; })
                            != result->get().end());

                        REQUIRE(std::count_if( //
                                    result->get().begin(),
                                    result->get().end(),
                                    [](const auto& elptr) {
                                        return nullptr != dynamic_cast<const SelectElement*>(elptr.get());
                                    })
                            == 1);
                    }

                    THEN("the result contains five member elements")
                    {
                        auto foo = make_element<MemberElement>(   //
                            "foo",                                //
                            from_primitive(42)                    //
                        );                                        //
                        auto zoo = make_element<MemberElement>(   //
                            "zoo",                                //
                            make_element<ArrayElement>(           //
                                from_primitive("lorem"),          //
                                from_primitive(5))                //
                        );                                        //
                        auto state = make_element<MemberElement>( //
                            "state",                              //
                            from_primitive("Prague, the capital") //
                        );                                        //
                        auto bar = make_element<MemberElement>(   //
                            "bar",                                //
                            from_primitive("quab")                //
                        );                                        //
                        auto lorem = make_element<MemberElement>( //
                            "lorem",                              //
                            from_primitive("ipsum")               //
                        );                                        //

                        REQUIRE(std::find_if( //
                                    result->get().begin(),
                                    result->get().end(),
                                    [&foo](const auto& elptr) { return *elptr == *foo; })
                            != result->get().end());

                        REQUIRE(std::find_if( //
                                    result->get().begin(),
                                    result->get().end(),
                                    [&zoo](const auto& elptr) { return *elptr == *zoo; })
                            != result->get().end());

                        REQUIRE(std::find_if( //
                                    result->get().begin(),
                                    result->get().end(),
                                    [&state](const auto& elptr) { return *elptr == *state; })
                            != result->get().end());

                        REQUIRE(std::find_if( //
                                    result->get().begin(),
                                    result->get().end(),
                                    [&bar](const auto& elptr) { return *elptr == *bar; })
                            != result->get().end());

                        REQUIRE(std::find_if( //
                                    result->get().begin(),
                                    result->get().end(),
                                    [&lorem](const auto& elptr) { return *elptr == *lorem; })
                            != result->get().end());

                        REQUIRE(std::count_if( //
                                    result->get().begin(),
                                    result->get().end(),
                                    [](const auto& elptr) {
                                        return nullptr != dynamic_cast<const MemberElement*>(elptr.get());
                                    })
                            == 5);
                    }
                }
            }
        }
    }
}
