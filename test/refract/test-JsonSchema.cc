//
//  test/refract/test-JsonSchema.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 17/02/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "catch.hpp"

#include "refract/JsonSchema.h"
#include "refract/Element.h"
#include "utils/so/JsonIo.h"

#include <chrono>
#include <sstream>

using namespace drafter::utils;
using namespace so;
using namespace std::chrono;
using namespace refract;

namespace
{
    std::string to_string(const so::Object& v)
    {
        std::ostringstream ss{};
        so::serialize_json(ss, v, so::packed{});
        return ss.str();
    }
} // namespace

SCENARIO("JSON Schema serialization of NullElement", "[json-schema]")
{
    GIVEN("An empty NullElement")
    {
        auto el = make_empty<NullElement>();

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches (any) null value")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"null"})");
            }
        }
    }

    GIVEN("A NullElement with content")
    {
        auto el = make_element<NullElement>();

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches (any) null value")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"null"})");
            }
        }
    }
}

SCENARIO("JSON Schema serialization of BooleanElement", "[json-schema]")
{
    GIVEN("An empty BooleanElement")
    {
        auto el = make_empty<BooleanElement>();

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches any boolean")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"boolean"})");
            }
        }
    }

    GIVEN("An empty BooleanElement with fixed attribute true")
    {
        auto el = make_empty<BooleanElement>();
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches any boolean")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"boolean"})");
            }
        }
    }

    GIVEN("A BooleanElement with content")
    {
        auto el = from_primitive(true);

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches any boolean")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"boolean"})");
            }
        }
    }

    GIVEN("A BooleanElement with content and fixed attribute true")
    {
        auto el = from_primitive(true);
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches true")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"boolean","enum":[true]})");
            }
        }
    }
}

SCENARIO("JSON Schema serialization of NumberElement", "[json-schema]")
{
    GIVEN("An empty NumberElement")
    {
        auto el = make_empty<NumberElement>();

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches any number")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"number"})");
            }
        }
    }

    GIVEN("An empty NumberElement with fixed attribute true")
    {
        auto el = make_empty<NumberElement>();
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches any number")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"number"})");
            }
        }
    }

    GIVEN("A NumberElement with content")
    {
        auto el = from_primitive(42.0);

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches any number")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"number"})");
            }
        }
    }

    GIVEN("A NumberElement with content and fixed attribute true")
    {
        auto el = from_primitive(42.0);
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches the number 42")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"number","enum":[42]})");
            }
        }
    }
}

SCENARIO("JSON Schema serialization of StringElement", "[json-schema]")
{
    GIVEN("An empty StringElement")
    {
        auto el = make_empty<StringElement>();

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches any string")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"string"})");
            }
        }
    }

    GIVEN("An empty StringElement with fixed attribute true")
    {
        auto el = make_empty<StringElement>();
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches any string")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"string"})");
            }
        }
    }

    GIVEN("A StringElement with content")
    {
        auto el = from_primitive("this will be ignored");

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches any string")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"string"})");
            }
        }
    }

    GIVEN("A StringElement with content and fixed attribute true")
    {
        auto el = from_primitive("foo");
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches the string \"foo\"")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"string","enum":["foo"]})");
            }
        }
    }
}

SCENARIO("JSON Schema serialization of ArrayElement", "[json-schema]")
{
    GIVEN("An empty ArrayElement")
    {
        auto el = make_empty<ArrayElement>();

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches any array")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"array"})");
            }
        }
    }

    GIVEN("An empty ArrayElement with fixed attribute true")
    {
        auto el = make_empty<ArrayElement>();
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches an empty array")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"array","enum":[[]]})");
            }
        }
    }

    GIVEN("An empty ArrayElement with fixedType attribute true")
    {
        auto el = make_empty<ArrayElement>();
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixedType")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches an empty array")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"array","enum":[[]]})");
            }
        }
    }

    GIVEN("An ArrayElement with empty string as content")
    {
        auto el = make_element<ArrayElement>(make_empty<StringElement>());

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches any array")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"array"})");
            }
        }
    }

    GIVEN("An ArrayElement with empty string as content and fixed attribute true")
    {
        auto el = make_element<ArrayElement>(make_empty<StringElement>());
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches arrays of a single entry that is a string")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"array","minItems":1,"items":[{"type":"string"}],"additionalItems":false})");
            }
        }
    }

    GIVEN("An ArrayElement with empty string as content and fixedType attribute true")
    {
        auto el = make_element<ArrayElement>(make_empty<StringElement>());
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixedType")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches arrays of strings")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"array","items":{"type":"string"}})");
            }
        }
    }

    GIVEN("An ArrayElement with string as content")
    {
        auto el = make_element<ArrayElement>(from_primitive("Hello world!"));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches any array")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"array"})");
            }
        }
    }

    GIVEN("An ArrayElement with fixed string as content and fixed attribute true")
    {
        auto el = make_element<ArrayElement>(from_primitive("Hello world!"));
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches [\"Hello world!\"] literals")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"array","minItems":1,"items":[{"type":"string","enum":["Hello world!"]}],"additionalItems":false})");
            }
        }
    }

    GIVEN("An ArrayElement with fixed string as content and fixedType attribute true")
    {
        auto el = make_element<ArrayElement>(from_primitive("Hello world!"));
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixedType")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches arrays of strings")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"array","items":{"type":"string"}})");
            }
        }
    }

    GIVEN("An ArrayElement with some content")
    {
        auto el = make_element<ArrayElement>(   //
            from_primitive("Hello world!"),     //
            from_primitive(42.0),               //
            make_element<ArrayElement>(         //
                from_primitive("short string"), //
                from_primitive(true)));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches any array")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"array"})");
            }
        }
    }

    GIVEN("An ArrayElement with some content and fixed attribute true")
    {
        auto el = make_element<ArrayElement>( //
            from_primitive("Hello world!"),   //
            make_empty<NumberElement>(),      //
            make_element<ArrayElement>(       //
                make_empty<StringElement>(),  //
                from_primitive(true)));

        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN(
                "the schema matches arrays of size 3 containing \"Hello world!\", a number and arrays of size 2 "
                "containing a string and true")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"array","minItems":3,"items":[{"type":"string","enum":["Hello world!"]},{"type":"number"},{"type":"array","minItems":2,"items":[{"type":"string"},{"type":"boolean","enum":[true]}],"additionalItems":false}],"additionalItems":false})");
            }
        }
    }

    GIVEN("An ArrayElement with some content and fixedType attribute true")
    {
        auto el = make_element<ArrayElement>(   //
            from_primitive("Hello world!"),     //
            from_primitive(42.0),               //
            make_element<ArrayElement>(         //
                from_primitive("short string"), //
                from_primitive(true)));

        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixedType")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches arrays of any size where elements match any of types string, number, array")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"array","items":{"anyOf":[{"type":"string"},{"type":"number"},{"type":"array"}]}})");
            }
        }
    }
}

SCENARIO("JSON Schema serialization of EnumElement", "[json-schema]")
{
    GIVEN("An empty EnumElement")
    {
        auto el = make_empty<EnumElement>();
        el->attributes().set("enumerations", make_element<ArrayElement>());

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches nothing")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","anyOf":[]})");
            }
        }
    }

    GIVEN("An empty EnumElement with fixed attribute true")
    {
        auto el = make_empty<EnumElement>();
        el->attributes().set("enumerations", make_element<ArrayElement>());
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches nothing")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","anyOf":[]})");
            }
        }
    }

    GIVEN("An empty EnumElement with non-empty enumerations")
    {
        auto el = make_empty<EnumElement>();
        el->attributes().set("enumerations",
            make_element<ArrayElement>(          //
                from_primitive("Hello world!"),  //
                make_empty<NumberElement>(),     //
                make_element<ArrayElement>(      //
                    make_empty<StringElement>(), //
                    from_primitive(true))));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches a string, a number and arrays")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","anyOf":[{"type":"string"},{"type":"number"},{"type":"array"}]})");
            }
        }
    }

    GIVEN("An empty EnumElement with non-empty enumerations and fixed attribute true")
    {
        auto el = make_empty<EnumElement>();
        el->attributes().set("enumerations",
            make_element<ArrayElement>(          //
                from_primitive("Hello world!"),  //
                make_empty<NumberElement>(),     //
                make_element<ArrayElement>(      //
                    make_empty<StringElement>(), //
                    from_primitive(true))));
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches \"Hello world!\", a number and arrays of size 2 containing a string and true")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","anyOf":[{"type":"string","enum":["Hello world!"]},{"type":"number"},{"type":"array","minItems":2,"items":[{"type":"string"},{"type":"boolean","enum":[true]}],"additionalItems":false}]})");
            }
        }
    }
}

SCENARIO("JSON Schema serialization of ObjectElement", "[json-schema]")
{
    GIVEN("An empty ObjectElement")
    {
        auto el = make_empty<ObjectElement>();

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches objects")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object"})");
            }
        }
    }

    GIVEN("An ObjectElement holding three MemberElements")
    {
        auto el = make_element<ObjectElement>(                                             //
            make_element<MemberElement>(from_primitive("first"), from_primitive(42.0)),    //
            make_element<MemberElement>(from_primitive("second"), from_primitive(true)),   //
            make_element<MemberElement>(from_primitive("third"), from_primitive("foobar")) //
        );

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches objects")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","properties":{"first":{"type":"number"},"second":{"type":"boolean"},"third":{"type":"string"}}})");
            }
        }
    }

    GIVEN("An ObjectElement holding three MemberElements, two of which are required")
    {
        auto first = make_element<MemberElement>(from_primitive("first"), from_primitive(42.0));
        auto second = make_element<MemberElement>(from_primitive("second"), from_primitive(true));
        second->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("required")));
        auto third = make_element<MemberElement>(from_primitive("third"), from_primitive("foobar"));
        third->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("required")));
        auto el = make_element<ObjectElement>(std::move(first), std::move(second), std::move(third));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches objects")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","properties":{"first":{"type":"number"},"second":{"type":"boolean"},"third":{"type":"string"}},"required":["second","third"]})");
            }
        }
    }

    GIVEN(
        "An ObjectElement holding three SelectElements with one OptionElement, two OptionElements and three "
        "OptionElements respectively")
    {
        auto el = make_element<ObjectElement>(                                                 //
            make_element<SelectElement>(                                                       //
                make_element<OptionElement>(                                                   //
                    make_element<MemberElement>(from_primitive("a"), from_primitive(42.0)),    //
                    make_element<MemberElement>(from_primitive("b"), from_primitive("foobar")) //
                    )                                                                          //
                ),                                                                             //
            make_element<SelectElement>(                                                       //
                make_element<OptionElement>(                                                   //
                    make_element<MemberElement>(from_primitive("c"), from_primitive(false))    //
                    ),                                                                         //
                make_element<OptionElement>(                                                   //
                    make_element<MemberElement>(from_primitive("d"), from_primitive("abc"))    //
                    )                                                                          //
                ),                                                                             //
            make_element<SelectElement>(                                                       //
                make_element<OptionElement>(                                                   //
                    make_element<MemberElement>(from_primitive("e"), from_primitive(42.0)),    //
                    make_element<MemberElement>(from_primitive("f"), from_primitive(true)),    //
                    make_element<MemberElement>(from_primitive("g"), from_primitive("abc"))    //
                    ),                                                                         //
                make_element<OptionElement>(                                                   //
                    make_element<MemberElement>(from_primitive("h"), from_primitive(42.0)),    //
                    make_element<MemberElement>(from_primitive("i"), from_primitive("foobar")) //
                    ),                                                                         //
                make_element<OptionElement>(                                                   //
                    make_element<MemberElement>(from_primitive("j"), from_primitive(42.0)),    //
                    make_element<MemberElement>(from_primitive("k"), from_primitive("foobar")) //
                    )                                                                          //
                ));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches objects")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","allOf":[{"oneOf":[{"properties":{"a":{"type":"number"},"b":{"type":"string"}}}]},{"oneOf":[{"properties":{"c":{"type":"boolean"}}},{"properties":{"d":{"type":"string"}}}]},{"oneOf":[{"properties":{"e":{"type":"number"},"f":{"type":"boolean"},"g":{"type":"string"}}},{"properties":{"h":{"type":"number"},"i":{"type":"string"}}},{"properties":{"j":{"type":"number"},"k":{"type":"string"}}}]}]})");
            }
        }
    }

    GIVEN(
        "An ObjectElement holding a SelectElement in turn holding, through OptionElements, a MemberElement and another "
        "SelectElement")
    {
        // clang-format off
        constexpr const char* expected = R"()";
        // clang-format on

        auto el = make_element<ObjectElement>(                                                         //
            make_element<SelectElement>(                                                               //
                make_element<OptionElement>(                                                           //
                    make_element<MemberElement>(from_primitive("a"), from_primitive(42.0))             //
                    ),                                                                                 //
                make_element<OptionElement>(                                                           //
                    make_element<SelectElement>(                                                       //
                        make_element<OptionElement>(                                                   //
                            make_element<MemberElement>(from_primitive("c"), from_primitive(42.0)),    //
                            make_element<MemberElement>(from_primitive("d"), from_primitive("foobar")) //
                            ),                                                                         //
                        make_element<OptionElement>(                                                   //
                            make_element<MemberElement>(from_primitive("f"), from_primitive("foobar")) //
                            )                                                                          //
                        ))                                                                             //
                ));
        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches objects")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","type":"object","allOf":[{"oneOf":[{"properties":{"a":{"type":"number"}}},{"allOf":[{"oneOf":[{"properties":{"c":{"type":"number"},"d":{"type":"string"}}},{"properties":{"f":{"type":"string"}}}]}]}]}]})");
            }
        }
    }
}
