//
//  test/refract/test-JsonValue.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 17/02/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "catch.hpp"

#include "refract/JsonValue.h"
#include "refract/Element.h"
#include "refract/ElementUtils.h"
#include "utils/so/JsonIo.h"

#include <sstream>

using namespace drafter::utils;
using namespace so;
using namespace refract;

namespace
{
    std::string to_string(const so::Value& v)
    {
        std::ostringstream ss{};
        so::serialize_json(ss, v, so::packed{});
        return ss.str();
    }

    template <typename Element>
    std::unique_ptr<RefElement> resolved(Element&& e)
    {
        auto result = make_empty<RefElement>();
        result->attributes().set("resolved", std::forward<Element>(e));
        return result;
    }

} // namespace

SCENARIO("JSON value serialization of NullElement", "[json-value][null]")
{
    GIVEN("An empty NullElement")
    {
        auto el = make_empty<NullElement>();

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `null`")
            {
                REQUIRE(to_string(result) == "null");
            }
        }
    }
}

SCENARIO("JSON value serialization of BooleanElement", "[json-value][boolean]")
{
    GIVEN("An empty BooleanElement")
    {
        auto el = make_empty<BooleanElement>();

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `false`")
            {
                REQUIRE(to_string(result) == "false");
            }
        }
    }

    GIVEN("An empty BooleanElement with a sample")
    {
        auto el = make_empty<BooleanElement>();
        addSample(*el, from_primitive(true));

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `true`")
            {
                REQUIRE(to_string(result) == "true");
            }
        }
    }

    GIVEN("An empty BooleanElement with a default")
    {
        auto el = make_empty<BooleanElement>();
        setDefault(*el, from_primitive(true));

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `true`")
            {
                REQUIRE(to_string(result) == "true");
            }
        }
    }

    GIVEN("An empty BooleanElement with sample and default")
    {
        auto el = make_empty<BooleanElement>();
        setDefault(*el, from_primitive(false));
        addSample(*el, from_primitive(true));

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `true`")
            {
                REQUIRE(to_string(result) == "true");
            }
        }
    }

    GIVEN("An empty, nullable BooleanElement with sample and default")
    {
        auto el = make_empty<BooleanElement>();
        setDefault(*el, from_primitive(false));
        addSample(*el, from_primitive(true));
        setTypeAttribute(*el, "nullable");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `true`")
            {
                REQUIRE(to_string(result) == "true");
            }
        }
    }

    GIVEN("A nullable BooleanElement with sample and default with content")
    {
        auto el = make_element<BooleanElement>(false);
        setDefault(*el, from_primitive(true));
        addSample(*el, from_primitive(true));
        setTypeAttribute(*el, "nullable");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `false`")
            {
                REQUIRE(to_string(result) == "false");
            }
        }
    }

    GIVEN("An empty, nullable BooleanElement")
    {
        auto el = make_empty<BooleanElement>();
        setTypeAttribute(*el, "nullable");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `null`")
            {
                REQUIRE(to_string(result) == "null");
            }
        }
    }

    GIVEN("A BooleanElement with content")
    {
        auto el = from_primitive(true);

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `true`")
            {
                REQUIRE(to_string(result) == "true");
            }
        }
    }
}

SCENARIO("JSON value serialization of NumberElement", "[json-value][number]")
{
    GIVEN("An empty NumberElement")
    {
        auto el = make_empty<NumberElement>();

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `0`")
            {
                REQUIRE(to_string(result) == "0");
            }
        }
    }

    GIVEN("An empty NumberElement with a sample")
    {
        auto el = make_empty<NumberElement>();
        addSample(*el, from_primitive(42));

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `42`")
            {
                REQUIRE(to_string(result) == "42");
            }
        }
    }

    GIVEN("An empty NumberElement with sample and default")
    {
        auto el = make_empty<NumberElement>();
        setDefault(*el, from_primitive(42));
        addSample(*el, from_primitive(3));

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `3`")
            {
                REQUIRE(to_string(result) == "3");
            }
        }
    }

    GIVEN("An empty, nullable NumberElement with sample and default")
    {
        auto el = make_empty<NumberElement>();
        setDefault(*el, from_primitive(42));
        addSample(*el, from_primitive(3));
        setTypeAttribute(*el, "nullable");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `3`")
            {
                REQUIRE(to_string(result) == "3");
            }
        }
    }

    GIVEN("A nullable NumberElement with sample and default with content")
    {
        auto el = make_element<NumberElement>(5);
        setDefault(*el, from_primitive(42));
        addSample(*el, from_primitive(3));
        setTypeAttribute(*el, "nullable");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `5`")
            {
                REQUIRE(to_string(result) == "5");
            }
        }
    }

    GIVEN("An empty NumberElement with a default")
    {
        auto el = make_empty<NumberElement>();
        setDefault(*el, from_primitive(42));

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `42`")
            {
                REQUIRE(to_string(result) == "42");
            }
        }
    }

    GIVEN("An empty, nullable NumberElement")
    {
        auto el = make_empty<NumberElement>();
        setTypeAttribute(*el, "nullable");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `null`")
            {
                REQUIRE(to_string(result) == "null");
            }
        }
    }

    GIVEN("A NumberElement with content")
    {
        auto el = make_element<NumberElement>("42.1930e-39");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `42.1930e-39`")
            {
                REQUIRE(to_string(result) == "42.1930e-39");
            }
        }
    }
}

SCENARIO("JSON value serialization of StringElement", "[json-value][string]")
{
    GIVEN("An empty StringElement")
    {
        auto el = make_empty<StringElement>();

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `\"\"`")
            {
                REQUIRE(to_string(result) == "\"\"");
            }
        }
    }

    GIVEN("An empty StringElement with a sample")
    {
        auto el = make_empty<StringElement>();
        addSample(*el, from_primitive("42"));

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `\"42\"`")
            {
                REQUIRE(to_string(result) == "\"42\"");
            }
        }
    }

    GIVEN("An empty StringElement with a default")
    {
        auto el = make_empty<StringElement>();
        setDefault(*el, from_primitive("42"));

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `\"42\"`")
            {
                REQUIRE(to_string(result) == "\"42\"");
            }
        }
    }

    GIVEN("An empty StringElement with sample and default")
    {
        auto el = make_empty<StringElement>();
        setDefault(*el, from_primitive("42"));
        addSample(*el, from_primitive("1"));

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `\"1\"`")
            {
                REQUIRE(to_string(result) == "\"1\"");
            }
        }
    }

    GIVEN("An empty, nullable StringElement with sample and default")
    {
        auto el = make_empty<StringElement>();
        setDefault(*el, from_primitive("42"));
        addSample(*el, from_primitive("1"));
        setTypeAttribute(*el, "nullable");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `\"1\"`")
            {
                REQUIRE(to_string(result) == "\"1\"");
            }
        }
    }

    GIVEN("A nullable StringElement with sample and default with content")
    {
        auto el = make_element<StringElement>("foo");
        setDefault(*el, from_primitive("bar"));
        addSample(*el, from_primitive("baz"));
        setTypeAttribute(*el, "nullable");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `\"foo\"`")
            {
                REQUIRE(to_string(result) == "\"foo\"");
            }
        }
    }

    GIVEN("An empty, nullable StringElement")
    {
        auto el = make_empty<StringElement>();
        setTypeAttribute(*el, "nullable");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `null`")
            {
                REQUIRE(to_string(result) == "null");
            }
        }
    }

    GIVEN("A StringElement with content")
    {
        auto el = make_element<StringElement>("42.1930e-39");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `\"42.1930e-39\"`")
            {
                REQUIRE(to_string(result) == "\"42.1930e-39\"");
            }
        }
    }
}

SCENARIO("JSON value serialization of ExtendElement", "[json-value][extend]")
{
    GIVEN("An ExtendElement with a StringElement hierarchy")
    {
        auto last = make_empty<StringElement>();
        addSample(*last, from_primitive("baz"));

        auto el = make_element<ExtendElement>( //
            from_primitive("foo"),             //
            from_primitive("bar"),             //
            std::move(last)                    //
        );

        auto merged = generateJsonValue(*el->get().merge());

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is equal to the JSON value after merging")
            {
                REQUIRE(to_string(result) == to_string(merged));
            }
        }
    }

    GIVEN("An ExtendElement with a NumberElement hierarchy")
    {
        auto last = make_empty<NumberElement>();
        addSample(*last, from_primitive(3));

        auto el = make_element<ExtendElement>( //
            from_primitive(5),                 //
            from_primitive(4),                 //
            std::move(last)                    //
        );

        auto merged = generateJsonValue(*el->get().merge());

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is equal to the JSON value after merging")
            {
                REQUIRE(to_string(result) == to_string(merged));
            }
        }
    }

    GIVEN("An ExtendElement with a BooleanElement hierarchy")
    {
        auto last = make_empty<BooleanElement>();
        addSample(*last, from_primitive(false));

        auto el = make_element<ExtendElement>( //
            from_primitive(false),             //
            from_primitive(true),              //
            std::move(last)                    //
        );

        auto merged = generateJsonValue(*el->get().merge());

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is equal to the JSON value after merging")
            {
                REQUIRE(to_string(result) == to_string(merged));
            }
        }
    }

    // OPTIM: @tjanc@ also add tests for other Element hierarchies
}

SCENARIO("JSON value serialization of EnumElement", "[json-value][enum]")
{
    GIVEN("An empty EnumElement")
    {
        auto el = make_empty<EnumElement>();

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `null`")
            {
                REQUIRE(to_string(result) == "null");
            }
        }
    }

    GIVEN("An empty EnumElement with a sample")
    {
        auto el = make_empty<EnumElement>();
        addEnumeration(*el, make_empty<NumberElement>());
        addSample(*el, from_primitive(42));

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `42`")
            {
                REQUIRE(to_string(result) == "42");
            }
        }
    }

    GIVEN("An empty EnumElement with a default")
    {
        auto el = make_empty<EnumElement>();
        addEnumeration(*el, make_empty<NumberElement>());
        setDefault(*el, from_primitive(42));

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `42`")
            {
                REQUIRE(to_string(result) == "42");
            }
        }
    }

    GIVEN("An empty EnumElement with sample and default")
    {
        auto el = make_empty<EnumElement>();
        addEnumeration(*el, make_empty<NumberElement>());
        setDefault(*el, from_primitive(1));
        addSample(*el, from_primitive(2));

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `2`")
            {
                REQUIRE(to_string(result) == "2");
            }
        }
    }

    GIVEN("An empty, nullable EnumElement with sample and default")
    {
        auto el = make_empty<EnumElement>();
        addEnumeration(*el, make_empty<NumberElement>());
        setDefault(*el, from_primitive(1));
        addSample(*el, from_primitive(2));
        setTypeAttribute(*el, "nullable");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `2`")
            {
                REQUIRE(to_string(result) == "2");
            }
        }
    }

    GIVEN("A nullable EnumElement with sample and default with content")
    {
        auto el = make_element<EnumElement>(from_primitive(42));
        addEnumeration(*el, make_empty<NumberElement>());
        setDefault(*el, from_primitive(1));
        addSample(*el, from_primitive(2));
        setTypeAttribute(*el, "nullable");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `42`")
            {
                REQUIRE(to_string(result) == "42");
            }
        }
    }

    GIVEN("An empty, nullable EnumElement")
    {
        auto el = make_empty<EnumElement>();
        setTypeAttribute(*el, "nullable");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `null`")
            {
                REQUIRE(to_string(result) == "null");
            }
        }
    }

    GIVEN("An EnumElement with content")
    {
        auto el = make_element<EnumElement>(from_primitive(42));
        addEnumeration(*el, make_empty<NumberElement>());

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `42`")
            {
                REQUIRE(to_string(result) == "42");
            }
        }
    }
}

SCENARIO("JSON value serialization of ArrayElement", "[json-value][array]")
{
    GIVEN("An empty ArrayElement")
    {
        auto el = make_empty<ArrayElement>();

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `[]`")
            {
                REQUIRE(to_string(result) == "[]");
            }
        }
    }

    GIVEN("An empty ArrayElement with a sample")
    {
        auto item1 = make_empty<NumberElement>();
        setDefault(*item1, from_primitive(1));

        auto item2 = make_empty<StringElement>();
        addSample(*item2, from_primitive("3"));

        auto el = make_empty<ArrayElement>();
        addSample(*el,
            make_element<ArrayElement>( //
                std::move(item1),
                std::move(item2),
                from_primitive(true)));

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `[1,\"3\",true]`")
            {
                REQUIRE(to_string(result) == "[1,\"3\",true]");
            }
        }
    }

    GIVEN("An empty ArrayElement with a default")
    {
        auto item1 = make_empty<NumberElement>();
        setDefault(*item1, from_primitive(1));

        auto item2 = make_empty<StringElement>();
        addSample(*item2, from_primitive("3"));

        auto el = make_empty<ArrayElement>();
        setDefault(*el,
            make_element<ArrayElement>( //
                std::move(item1),
                std::move(item2),
                from_primitive(true)));

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `[1,\"3\",true]`")
            {
                REQUIRE(to_string(result) == "[1,\"3\",true]");
            }
        }
    }

    GIVEN("An empty ArrayElement with sample and default")
    {
        auto item1 = make_empty<NumberElement>();
        setDefault(*item1, from_primitive(1));

        auto item2 = make_empty<StringElement>();
        addSample(*item2, from_primitive("3"));

        auto el = make_empty<ArrayElement>();
        addSample(*el,
            make_element<ArrayElement>( //
                std::move(item1),
                std::move(item2),
                from_primitive(true)));
        setDefault(*el, make_element<ArrayElement>());

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `[1,\"3\",true]`")
            {
                REQUIRE(to_string(result) == "[1,\"3\",true]");
            }
        }
    }

    GIVEN("An empty, nullable ArrayElement with sample and default")
    {
        auto item1 = make_empty<NumberElement>();
        setDefault(*item1, from_primitive(1));

        auto item2 = make_empty<StringElement>();
        addSample(*item2, from_primitive("3"));

        auto el = make_empty<ArrayElement>();
        addSample(*el,
            make_element<ArrayElement>( //
                std::move(item1),
                std::move(item2),
                from_primitive(true)));
        setDefault(*el, make_element<ArrayElement>());
        setTypeAttribute(*el, "nullable");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `[1,\"3\",true]`")
            {
                REQUIRE(to_string(result) == "[1,\"3\",true]");
            }
        }
    }

    GIVEN("A nullable ArrayElement with sample and default with content")
    {
        auto item1 = make_empty<NumberElement>();
        setDefault(*item1, from_primitive(1));

        auto item2 = make_empty<StringElement>();
        addSample(*item2, from_primitive("3"));

        auto el = make_element<ArrayElement>( //
            std::move(item1),
            std::move(item2),
            make_empty<BooleanElement>());
        addSample(*el, make_element<ArrayElement>());
        setDefault(*el, make_element<ArrayElement>());
        setTypeAttribute(*el, "nullable");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `[1,\"3\"]`")
            {
                REQUIRE(to_string(result) == "[1,\"3\"]");
            }
        }
    }

    GIVEN("An empty, nullable ArrayElement")
    {
        auto el = make_empty<ArrayElement>();
        setTypeAttribute(*el, "nullable");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `null`")
            {
                REQUIRE(to_string(result) == "null");
            }
        }
    }

    GIVEN("An empty, fixed ArrayElement with a sample")
    {
        auto item1 = make_empty<ArrayElement>();
        setDefault(*item1, make_element<ArrayElement>(from_primitive(4), from_primitive("3")));

        auto item2 = make_empty<StringElement>();
        addSample(*item2, from_primitive("foobar"));

        auto el = make_empty<ArrayElement>();
        addSample(*el,
            make_element<ArrayElement>( //
                std::move(item1),
                std::move(item2),
                make_empty<BooleanElement>()));
        setTypeAttribute(*el, "fixed");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `[[4,\"3\"],\"foobar\"]`")
            {
                REQUIRE(to_string(result) == "[[4,\"3\"],\"foobar\"]");
            }
        }
    }

    GIVEN("An empty, fixed ArrayElement with a sample with content, some of which is empty and fixed")
    {
        auto item1 = make_empty<ArrayElement>();
        setDefault(*item1, make_element<ArrayElement>(from_primitive(4), from_primitive("3")));

        auto item2 = make_empty<StringElement>();
        addSample(*item2, from_primitive("foobar"));

        auto emptyAndFixed = make_empty<BooleanElement>();
        setTypeAttribute(*emptyAndFixed, "fixed");

        auto el = make_empty<ArrayElement>();
        addSample(*el,
            make_element<ArrayElement>( //
                std::move(item1),
                std::move(item2),
                std::move(emptyAndFixed)));
        setTypeAttribute(*el, "fixed");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `[[4,\"3\"],\"foobar\",false]`")
            {
                REQUIRE(to_string(result) == "[[4,\"3\"],\"foobar\",false]");
            }
        }
    }

    GIVEN("An ArrayElement with content, some of which is empty")
    {
        auto el = make_element<ArrayElement>( //
            make_empty<StringElement>(),      //
            from_primitive(42),               //
            from_primitive(true),             //
            make_empty<NumberElement>()       //
        );

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `[42,true]`")
            {
                REQUIRE(to_string(result) == "[42,true]");
            }
        }
    }

    GIVEN("A fixed ArrayElement with content, some of which is empty")
    {
        auto el = make_element<ArrayElement>( //
            make_empty<StringElement>(),      //
            from_primitive(42),               //
            from_primitive(true),             //
            make_empty<NumberElement>()       //
        );
        setTypeAttribute(*el, "fixed");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `[42,true]`")
            {
                REQUIRE(to_string(result) == "[42,true]");
            }
        }
    }

    GIVEN("An ArrayElement with content, some of which is empty and fixed")
    {
        auto emptyAndFixed = make_empty<NumberElement>();
        setTypeAttribute(*emptyAndFixed, "fixed");
        auto el = make_element<ArrayElement>( //
            make_empty<StringElement>(),      //
            from_primitive(42),               //
            from_primitive(true),             //
            std::move(emptyAndFixed)          //
        );

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `[42,true,0]`")
            {
                REQUIRE(to_string(result) == "[42,true,0]");
            }
        }
    }

    GIVEN("An ArrayElement with content, some of which is a RefElement resolved to an empty ArrayElement")
    {
        auto ref = resolved(make_empty<ArrayElement>());
        auto el = make_element<ArrayElement>( //
            make_empty<StringElement>(),      //
            from_primitive(42),               //
            from_primitive(true),             //
            std::move(ref)                    //
        );

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `[42,true]`")
            {
                REQUIRE(to_string(result) == "[42,true]");
            }
        }
    }

    GIVEN("An ArrayElement with content, some of which is RefElement resolved to ArrayElement with content")
    {
        auto ref = resolved(make_element<ArrayElement>( //
            from_primitive("foo"),                      //
            from_primitive(3))                          //
        );

        auto el = make_element<ArrayElement>( //
            make_empty<StringElement>(),      //
            from_primitive(42),               //
            std::move(ref),                   //
            from_primitive(true)              //
        );

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `[42,\"foo\",3,true]`")
            {
                REQUIRE(to_string(result) == "[42,\"foo\",3,true]");
            }
        }
    }
}

SCENARIO("JSON value serialization of ObjectElement", "[json-value][object]")
{
    GIVEN("An empty ObjectElement")
    {
        auto el = make_empty<ObjectElement>();

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `{}`")
            {
                REQUIRE(to_string(result) == "{}");
            }
        }
    }

    GIVEN("An empty ObjectElement with a sample")
    {
        auto property1 = make_element<MemberElement>("foo", make_empty<NumberElement>());
        auto property2 = make_element<MemberElement>("song", from_primitive("indiscipline"));

        auto el = make_empty<ObjectElement>();
        addSample(*el,
            make_element<ObjectElement>(                                           //
                std::move(property1),                                              //
                std::move(property2),                                              //
                make_element<MemberElement>("plusplusgood", from_primitive(true))) //
        );

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `{\"foo\":0,\"song\":\"indiscipline\",\"baz\":true}`")
            {
                REQUIRE(to_string(result) == "{\"foo\":0,\"song\":\"indiscipline\",\"plusplusgood\":true}");
            }
        }
    }

    GIVEN("An empty ObjectElement with a default")
    {
        auto property1 = make_element<MemberElement>("foo", make_empty<NumberElement>());
        auto property2 = make_element<MemberElement>("song", from_primitive("indiscipline"));

        auto el = make_empty<ObjectElement>();
        setDefault(*el,
            make_element<ObjectElement>(                                           //
                std::move(property1),                                              //
                std::move(property2),                                              //
                make_element<MemberElement>("plusplusgood", from_primitive(true))) //
        );

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `{\"foo\":0,\"song\":\"indiscipline\",\"baz\":true}`")
            {
                REQUIRE(to_string(result) == "{\"foo\":0,\"song\":\"indiscipline\",\"plusplusgood\":true}");
            }
        }
    }

    GIVEN("An empty ObjectElement with a sample and default")
    {
        auto property1 = make_element<MemberElement>("foo", make_empty<NumberElement>());
        auto property2 = make_element<MemberElement>("song", from_primitive("indiscipline"));

        auto el = make_empty<ObjectElement>();
        addSample(*el,
            make_element<ObjectElement>(                                           //
                std::move(property1),                                              //
                std::move(property2),                                              //
                make_element<MemberElement>("plusplusgood", from_primitive(true))) //
        );
        setDefault(*el, make_element<ObjectElement>());

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `{\"foo\":0,\"song\":\"indiscipline\",\"baz\":true}`")
            {
                REQUIRE(to_string(result) == "{\"foo\":0,\"song\":\"indiscipline\",\"plusplusgood\":true}");
            }
        }
    }

    GIVEN("An empty, nullable ObjectElement with a sample and default")
    {
        auto property1 = make_element<MemberElement>("foo", make_empty<NumberElement>());
        auto property2 = make_element<MemberElement>("song", from_primitive("indiscipline"));

        auto el = make_empty<ObjectElement>();
        addSample(*el,
            make_element<ObjectElement>(                                           //
                std::move(property1),                                              //
                std::move(property2),                                              //
                make_element<MemberElement>("plusplusgood", from_primitive(true))) //
        );
        setDefault(*el, make_element<ObjectElement>());
        setTypeAttribute(*el, "nullable");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `{\"foo\":0,\"song\":\"indiscipline\",\"baz\":true}`")
            {
                REQUIRE(to_string(result) == "{\"foo\":0,\"song\":\"indiscipline\",\"plusplusgood\":true}");
            }
        }
    }

    GIVEN("A nullable ObjectElement with a sample and default with content")
    {
        auto property1 = make_element<MemberElement>("foo", make_empty<NumberElement>());
        auto property2 = make_element<MemberElement>("song", from_primitive("indiscipline"));

        auto el = make_element<ObjectElement>(                                //
            std::move(property1),                                             //
            std::move(property2),                                             //
            make_element<MemberElement>("plusplusgood", from_primitive(true)) //
        );
        addSample(*el, make_element<ObjectElement>());
        setDefault(*el, make_element<ObjectElement>());
        setTypeAttribute(*el, "nullable");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `{\"foo\":0,\"song\":\"indiscipline\",\"baz\":true}`")
            {
                REQUIRE(to_string(result) == "{\"foo\":0,\"song\":\"indiscipline\",\"plusplusgood\":true}");
            }
        }
    }

    GIVEN("An empty, nullable ObjectElement")
    {
        auto el = make_empty<ObjectElement>();
        setTypeAttribute(*el, "nullable");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `null`")
            {
                REQUIRE(to_string(result) == "null");
            }
        }
    }

    GIVEN("An empty, fixed ObjectElement with a sample")
    {
        auto property1 = make_element<MemberElement>("foo", make_empty<NumberElement>());
        auto property2 = make_element<MemberElement>("song", from_primitive("indiscipline"));

        auto el = make_empty<ObjectElement>();
        addSample(*el,
            make_element<ObjectElement>(                                           //
                std::move(property1),                                              //
                std::move(property2),                                              //
                make_element<MemberElement>("plusplusgood", from_primitive(true))) //
        );
        setTypeAttribute(*el, "fixed");

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `{\"foo\":0,\"song\":\"indiscipline\",\"baz\":true}`")
            {
                REQUIRE(to_string(result) == "{\"foo\":0,\"song\":\"indiscipline\",\"plusplusgood\":true}");
            }
        }
    }

    GIVEN("An ObjectElement with nullable, empty MemberElements")
    {
        auto property1 = make_element<MemberElement>("foo", make_empty<NumberElement>());
        setTypeAttribute(*property1, "nullable");

        auto property2 = make_element<MemberElement>("bar", make_empty<ArrayElement>());
        setTypeAttribute(*property2, "nullable");

        auto el = make_element<ObjectElement>( //
            std::move(property1),              //
            std::move(property2)               //
        );

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `{\"foo\":null,\"bar\":null}`")
            {
                REQUIRE(to_string(result) == "{\"foo\":null,\"bar\":null}");
            }
        }
    }

    GIVEN("An ObjectElement with content, one of which is a RefElement resolved to a MemberElement")
    {
        auto property1 = make_element<MemberElement>("foo", make_empty<NumberElement>());
        setTypeAttribute(*property1, "nullable");

        auto el = make_element<ObjectElement>(                               //
            std::move(property1),                                            //
            resolved(make_element<MemberElement>("bar", from_primitive(42))) //
        );

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `{\"foo\":null,\"bar\":42}`")
            {
                REQUIRE(to_string(result) == "{\"foo\":null,\"bar\":42}");
            }
        }
    }

    GIVEN("An ObjectElement with content, one of which is a RefElement resolved to a SelectElement")
    {
        auto property1 = make_element<MemberElement>("foo", make_empty<NumberElement>());
        setTypeAttribute(*property1, "nullable");

        auto el = make_element<ObjectElement>(                                           //
            std::move(property1),                                                        //
            resolved(                                                                    //
                make_element<SelectElement>(                                             //
                    make_element<OptionElement>(                                         //
                        make_element<MemberElement>("bar", from_primitive("bar")),       //
                        make_element<MemberElement>("baz", make_empty<BooleanElement>()) //
                        ),                                                               //
                    make_element<OptionElement>(                                         //
                        make_element<MemberElement>("a", from_primitive("c")),           //
                        make_element<MemberElement>("b", from_primitive(42))             //
                        )                                                                //
                    )                                                                    //
                )                                                                        //
        );

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `{\"foo\":null,\"bar\":\"bar\",\"baz\":false}`")
            {
                REQUIRE(to_string(result) == "{\"foo\":null,\"bar\":\"bar\",\"baz\":false}");
            }
        }
    }

    GIVEN("An ObjectElement with content, one of which is a RefElement resolved to an ObjectElement")
    {
        auto emptyWithSample = make_empty<ObjectElement>();
        addSample(*emptyWithSample,                                                                    //
            make_element<ObjectElement>(make_element<MemberElement>("status", from_primitive("idle"))) //
        );

        auto el = make_element<ObjectElement>(                                   //
            make_element<MemberElement>("foo", make_empty<NumberElement>()),     //
            resolved(make_element<ObjectElement>(                                //
                make_element<MemberElement>("bar", from_primitive("bar")),       //
                make_element<MemberElement>("baz", make_empty<BooleanElement>()) //
                )),                                                              //
            make_element<MemberElement>("mid", from_primitive(true)),            //
            std::move(emptyWithSample),
            make_element<ObjectElement>(), //
            make_element<MemberElement>("last",
                make_element<ArrayElement>(       //
                    from_primitive("a"),          //
                    make_empty<BooleanElement>()) //
                )                                 //
        );

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN(
                "the result is "
                "`{\"foo\":0,\"bar\":\"bar\",\"baz\":false,\"mid\":true,\"status\":\"idle\",\"last\":[\"a\"]}`")
            {
                REQUIRE(to_string(result)
                    == "{\"foo\":0,\"bar\":\"bar\",\"baz\":false,\"mid\":true,\"status\":\"idle\",\"last\":[\"a\"]}");
            }
        }
    }

    GIVEN("An ObjectElement with content, some of which is ObjectElements")
    {
        auto emptyWithSample = make_empty<ObjectElement>();
        addSample(*emptyWithSample,                                                                    //
            make_element<ObjectElement>(make_element<MemberElement>("status", from_primitive("idle"))) //
        );

        auto el = make_element<ObjectElement>(                                           //
            make_element<MemberElement>("foo", make_empty<NumberElement>()),             //
            make_element<ObjectElement>(                                                 //
                make_element<SelectElement>(                                             //
                    make_element<OptionElement>(                                         //
                        make_element<MemberElement>("bar", from_primitive("bar")),       //
                        make_element<MemberElement>("baz", make_empty<BooleanElement>()) //
                        ),                                                               //
                    make_element<OptionElement>(                                         //
                        make_element<MemberElement>("a", from_primitive("c")),           //
                        make_element<MemberElement>("b", from_primitive(42))             //
                        )                                                                //
                    )                                                                    //
                ),                                                                       //
            make_element<MemberElement>("mid", from_primitive(true)),                    //
            std::move(emptyWithSample),
            make_element<ObjectElement>(), //
            make_element<MemberElement>("last",
                make_element<ArrayElement>(       //
                    from_primitive("a"),          //
                    make_empty<BooleanElement>()) //
                )                                 //
        );

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN(
                "the result is "
                "`{\"foo\":0,\"bar\":\"bar\",\"baz\":false,\"mid\":true,\"status\":\"idle\",\"last\":[\"a\"]}`")
            {
                REQUIRE(to_string(result)
                    == "{\"foo\":0,\"bar\":\"bar\",\"baz\":false,\"mid\":true,\"status\":\"idle\",\"last\":[\"a\"]}");
            }
        }
    }

    GIVEN("An ObjectElement with content, some of which is SelectElements")
    {
        auto el = make_element<ObjectElement>(                                       //
            make_element<MemberElement>("foo", make_empty<NumberElement>()),         //
            make_element<SelectElement>(                                             //
                make_element<OptionElement>(                                         //
                    make_element<MemberElement>("bar", from_primitive("bar")),       //
                    make_element<MemberElement>("baz", make_empty<BooleanElement>()) //
                    ),                                                               //
                make_element<OptionElement>(                                         //
                    make_element<MemberElement>("a", from_primitive("c")),           //
                    make_element<MemberElement>("b", from_primitive(42))             //
                    )                                                                //
                ),                                                                   //
            make_element<MemberElement>("mid", from_primitive(true)),                //
            make_element<SelectElement>(                                             //
                make_element<OptionElement>(                                         //
                    make_element<MemberElement>("status", from_primitive("idle"))    //
                    ),                                                               //
                make_element<OptionElement>(                                         //
                    make_element<MemberElement>("idle", from_primitive(true))        //
                    )                                                                //
                ),                                                                   //
            make_element<SelectElement>(make_element<OptionElement>()),              //
            make_element<MemberElement>("last",
                make_element<ArrayElement>(       //
                    from_primitive("a"),          //
                    make_empty<BooleanElement>()) //
                )                                 //
        );

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN(
                "the result is "
                "`{\"foo\":0,\"bar\":\"bar\",\"baz\":false,\"mid\":true,\"status\":\"idle\",\"last\":[\"a\"]}`")
            {
                REQUIRE(to_string(result)
                    == "{\"foo\":0,\"bar\":\"bar\",\"baz\":false,\"mid\":true,\"status\":\"idle\",\"last\":[\"a\"]}");
            }
        }
    }

    GIVEN("An ObjectElement with content, some of which is ExtendElements")
    {
        auto emptyWithSample = make_empty<ObjectElement>();
        addSample(*emptyWithSample,                                                                    //
            make_element<ObjectElement>(make_element<MemberElement>("status", from_primitive("idle"))) //
        );

        auto withRef = make_element<ObjectElement>(                              //
            make_element<MemberElement>("foo", make_empty<NumberElement>()),     //
            resolved(make_element<ObjectElement>(                                //
                make_element<MemberElement>("bar", from_primitive("bar")),       //
                make_element<MemberElement>("baz", make_empty<BooleanElement>()) //
                )),                                                              //
            make_element<MemberElement>("mid",
                make_element<ArrayElement>(       //
                    from_primitive("a"),          //
                    make_empty<BooleanElement>()) //
                )                                 //
        );

        auto el = make_element<ObjectElement>(                                           //
            make_element<ExtendElement>(std::move(emptyWithSample), std::move(withRef)), //
            make_element<ExtendElement>(
                make_element<ObjectElement>(make_element<MemberElement>("mid", from_primitive(true)))), //
            make_element<MemberElement>("last", from_primitive(42))                                     //
        );

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `{\"foo\":0,\"bar\":\"bar\",\"baz\":false,\"mid\":true,\"last\":42}`")
            {
                REQUIRE(to_string(result) == "{\"foo\":0,\"bar\":\"bar\",\"baz\":false,\"mid\":true,\"last\":42}");
            }
        }
    }

    GIVEN("An ObjectElement with many MemberElements with same key")
    {
        auto emptyKeyWithSample = make_empty<StringElement>();
        addSample(*emptyKeyWithSample, from_primitive("bar"));
        auto el = make_element<ObjectElement>(                                                         //
            make_element<MemberElement>("foo", from_primitive(42)),                                    //
            make_element<MemberElement>("bar", make_element<ArrayElement>()),                          //
            make_element<MemberElement>("bar", from_primitive(3)),                                     //
            make_element<MemberElement>(std::move(emptyKeyWithSample), make_element<ObjectElement>()), //
            make_element<MemberElement>("foo", from_primitive(3))                                      //
        );

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `{\"foo\":3,\"bar\":{}}`")
            {
                REQUIRE(to_string(result) == "{\"foo\":3,\"bar\":{}}");
            }
        }
    }

    GIVEN("An ObjectElement with many MemberElements with empty key")
    {
        auto el = make_element<ObjectElement>(                                              //
            make_element<MemberElement>(make_empty<StringElement>(), from_primitive(42)),   //
            make_element<MemberElement>(make_empty<StringElement>(), from_primitive("42")), //
            make_element<MemberElement>(make_empty<StringElement>(), from_primitive(true))  //
        );

        WHEN("a JSON value is generated from it")
        {
            auto result = generateJsonValue(*el);

            THEN("the result is `{}`")
            {
                REQUIRE(to_string(result) == "{}");
            }
        }
    }
}
