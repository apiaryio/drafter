#include "ElementInfoUtils.h"
#include <catch2/catch.hpp>

template <typename ElementT>
snowcrash::SourceMap<typename ElementT::ValueType> make_sourcemap(std::vector<mdp::Range>&& locations)
{
    snowcrash::SourceMap<typename ElementT::ValueType> res;
    std::move(locations.begin(), locations.end(), std::back_inserter(res.sourceMap));
    return res;
}

SCENARIO("Merging primitive ElementInfo", "[ElementInfo][merge]")
{
    using namespace drafter;

    using E = refract::StringElement;
    using Info = ElementInfo<E>;
    using Container = std::deque<Info>;

    GIVEN("Empty Container for primitive type")
    {

        Container c;
        WHEN("it is merged")
        {
            auto merged = Merge<E>()(std::move(c));

            THEN("merged element value will be empty")
            {
                REQUIRE(merged.value.empty());
            }

            THEN("it has no sourcemap")
            {
                REQUIRE(merged.sourceMap.sourceMap.empty());
            }
        }
    }

    GIVEN("Container has one primitive type")
    {
        Container c;
        c.emplace_back(Info("str", make_sourcemap<E>({ { 1, 2 }, { 4, 2 } })));

        WHEN("it is merged")
        {
            auto merged = Merge<E>()(std::move(c));

            THEN("value of info is not empty")
            {
                REQUIRE(!merged.value.empty());
            }

            THEN("value of info is equal to origin info")
            {
                REQUIRE(merged.value == "str");
            }

            THEN("it has sourceMap of origin info")
            {
                REQUIRE(!merged.sourceMap.sourceMap.empty());
                REQUIRE(merged.sourceMap.sourceMap.size() == 2);

                auto r = merged.sourceMap.sourceMap.begin();
                REQUIRE(r->location == 1);
                REQUIRE(r->length == 2);

                r++;
                REQUIRE(r->location == 4);
                REQUIRE(r->length == 2);

                r++;
                REQUIRE(r == merged.sourceMap.sourceMap.end());
            }
        }
    }

    GIVEN("Container has more primitive types")
    {
        Container c;
        c.emplace_back(Info("s1", make_sourcemap<E>({ { 1, 2 } })));
        c.emplace_back(Info("s2", make_sourcemap<E>({ { 2, 1 } })));

        WHEN("it is merged")
        {
            auto merged = Merge<E>()(std::move(c));

            THEN("value of info is not empty")
            {
                REQUIRE(!merged.value.empty());
            }

            THEN("value of info is equal to first info")
            {
                REQUIRE(merged.value == "s1");
            }

            THEN("it has sourceMap of origin info")
            {
                REQUIRE(!merged.sourceMap.sourceMap.empty());
                REQUIRE(merged.sourceMap.sourceMap.size() == 1);

                const auto& r = merged.sourceMap.sourceMap.front();
                REQUIRE(r.location == 1);
                REQUIRE(r.length == 2);
            }
        }
    }
}

SCENARIO("Merging complex ElementInfo", "[ElementInfo][merge]")
{
    using namespace drafter;

    using E = refract::ArrayElement;
    using Info = ElementInfo<E>;
    using Container = std::deque<Info>;

    GIVEN("Empty Container for complex type")
    {

        Container c;
        WHEN("it is merged")
        {
            auto merged = Merge<E>()(std::move(c));

            THEN("merged element value will be empty")
            {
                REQUIRE(merged.value.empty());
            }

            THEN("it has no sourcemap")
            {
                REQUIRE(merged.sourceMap.sourceMap.empty());
            }
        }
    }

    GIVEN("Container contains one element")
    {
        Container c;
        Info i;
        i.value.emplace_back(refract::make_element<refract::StringElement>("test"));
        i.sourceMap = make_sourcemap<E>({ { 4, 2 } });
        c.emplace_back(std::move(i));

        WHEN("it is merged")
        {
            auto merged = Merge<E>()(std::move(c));

            THEN("value of info is not empty")
            {
                REQUIRE(!merged.value.empty());
            }

            THEN("value of info is equal to origin info")
            {
                REQUIRE(merged.value.size() == 1);
                REQUIRE(*merged.value.front() == *refract::make_element<refract::StringElement>("test"));
            }

            THEN("it has sourceMap of origin info")
            {
                REQUIRE(!merged.sourceMap.sourceMap.empty());
                REQUIRE(merged.sourceMap.sourceMap.size() == 1);

                auto r = merged.sourceMap.sourceMap.front();
                REQUIRE(r.location == 4);
                REQUIRE(r.length == 2);
            }
        }
    }

    GIVEN(
        "Container contains two ElementInfo. First with one StringElement and Second with StringElement and "
        "NumberElement")
    {
        Container c;

        Info i1;
        i1.value.emplace_back(refract::make_element<refract::StringElement>("test"));
        i1.sourceMap = make_sourcemap<E>({ { 4, 2 } });
        c.emplace_back(std::move(i1));

        Info i2;
        i2.value.emplace_back(refract::make_element<refract::NumberElement>(2));
        i2.value.emplace_back(refract::make_element<refract::StringElement>("computadora"));
        i2.sourceMap = make_sourcemap<E>({ { 10, 20 } });
        c.emplace_back(std::move(i2));

        WHEN("it is merged")
        {
            auto merged = Merge<E>()(std::move(c));

            THEN("value of info is not empty")
            {
                REQUIRE(!merged.value.empty());
            }

            THEN("value of info contains three elements")
            {
                REQUIRE(merged.value.size() == 3);
            }

            auto r = merged.value.begin();

            THEN("First element is string with value 'test'")
            {
                REQUIRE(*(*r) == *refract::make_element<refract::StringElement>("test"));
            }

            r++;
            THEN("Second element is nmumber with value '2'")
            {
                REQUIRE(*(*r) == *refract::make_element<refract::NumberElement>(2));
            }

            r++;
            THEN("Thrird element is string with value 'computadora'")
            {
                REQUIRE(*(*r) == *refract::make_element<refract::StringElement>("computadora"));
            }

            r++;
            THEN("There is no other element in container")
            {
                REQUIRE(r == merged.value.end());
            }

            THEN("it has merged sourceaaps of origin info")
            {
                REQUIRE(!merged.sourceMap.sourceMap.empty());
                REQUIRE(merged.sourceMap.sourceMap.size() == 2);

                auto r = merged.sourceMap.sourceMap.begin();
                REQUIRE(r->location == 4);
                REQUIRE(r->length == 2);

                r++;
                REQUIRE(r->location == 10);
                REQUIRE(r->length == 20);
            }
        }
    }
}

SCENARIO("Cloning InfoElementsContainer")
{
    using namespace drafter;

    using E = refract::ArrayElement;
    using Info = ElementInfo<E>;
    using Container = std::deque<Info>;

    GIVEN("Empty InfoElementsContainer")
    {
        Container c;

        WHEN("it is cloned")
        {
            auto cloned = CloneElementInfoContainer(c);

            THEN("cloned id empty")
            {
                REQUIRE(cloned.empty());
            }
        }
    }

    GIVEN("Container with two InfoElements")
    {
        Container c;

        Info i1;
        i1.value.emplace_back(refract::make_element<refract::StringElement>("test"));
        i1.sourceMap = make_sourcemap<E>({ { 4, 2 } });
        c.emplace_back(std::move(i1));

        Info i2;
        i2.value.emplace_back(refract::make_element<refract::NumberElement>(2));
        i2.sourceMap = make_sourcemap<E>({ { 10, 20 } });
        c.emplace_back(std::move(i2));

        WHEN("It is cloned")
        {
            auto cloned = CloneElementInfoContainer(c);

            THEN("cloned is not empty")
            {
                REQUIRE(!cloned.empty());
            }

            THEN("cloned has same count of elements like original container")
            {
                REQUIRE(c.size() == cloned.size());
            }

            WHEN("we iterate trough members")
            {
                auto oi = c.begin();
                auto ci = cloned.begin();

                THEN("values and source maps are identic in first element")
                {
                    REQUIRE(oi->value.size() == ci->value.size());
                    REQUIRE(*oi->value.front() == *ci->value.front());

                    REQUIRE(oi->sourceMap.sourceMap.size() == ci->sourceMap.sourceMap.size());
                    REQUIRE(oi->sourceMap.sourceMap.front().location == ci->sourceMap.sourceMap.front().location);
                    REQUIRE(oi->sourceMap.sourceMap.front().length == ci->sourceMap.sourceMap.front().length);
                }

                // WHEN("move to next ElementInfo")
                oi++;
                ci++;

                THEN("values and source maps are identic in second element")
                {
                    REQUIRE(oi->value.size() == ci->value.size());
                    REQUIRE(*oi->value.front() == *ci->value.front());

                    REQUIRE(oi->sourceMap.sourceMap.size() == ci->sourceMap.sourceMap.size());
                    REQUIRE(oi->sourceMap.sourceMap.front().location == ci->sourceMap.sourceMap.front().location);
                    REQUIRE(oi->sourceMap.sourceMap.front().length == ci->sourceMap.sourceMap.front().length);
                }

                // WHEN("move to next ElementInfo")
                oi++;
                ci++;
                THEN("there is no orher element in both origin and clone")
                {
                    REQUIRE(oi == c.end());
                    REQUIRE(ci == cloned.end());
                }
            }

            WHEN("remove element form front of origin")
            {
                size_t s = c.size();
                c.pop_front();

                THEN("size of orging is changed")
                {
                    REQUIRE(c.size() == (s - 1));
                }

                THEN("size of clone is unchanged")
                {
                    REQUIRE(cloned.size() == s);
                }
            }
        }
    }
}
