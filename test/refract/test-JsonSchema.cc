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
#include "refract/JSONSchemaVisitor.h"
#include "utils/so/JsonIo.h"
#include "utils/log/Trivial.h"

#include <chrono>
#include <sstream>

using namespace drafter::utils;
using namespace drafter::utils::log;
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

    GIVEN("A named NullElement with content")
    {
        auto el = make_element<NullElement>();
        el->element("Foo");

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches null")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","definitions":{"Foo":{"type":"null"}},"$ref":"#/definitions/Foo"})");
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

    GIVEN("A named BooleanElement with content and fixed attribute true")
    {
        auto el = from_primitive(true);
        el->element("Bar");
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches the boolean true")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","definitions":{"Bar":{"type":"boolean","enum":[true]}},"$ref":"#/definitions/Bar"})");
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

    GIVEN("A named NumberElement with content and fixed attribute true")
    {
        auto el = from_primitive(42.0);
        el->element("Baz");
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches the number 42")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","definitions":{"Baz":{"type":"number","enum":[42]}},"$ref":"#/definitions/Baz"})");
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

    GIVEN("A named StringElement with content and fixed attribute true")
    {
        auto el = from_primitive("foo");
        el->element("Flip");
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches the string \"foo\"")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","definitions":{"Flip":{"type":"string","enum":["foo"]}},"$ref":"#/definitions/Flip"})");
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

    GIVEN("A named ArrayElement with some content and fixed attribute true")
    {
        auto flap = make_element<ArrayElement>( //
            make_empty<StringElement>(),        //
            from_primitive(true));
        flap->element("Flap");

        auto el = make_element<ArrayElement>( //
            from_primitive("Hello world!"),   //
            make_empty<NumberElement>(),      //
            std::move(flap));

        el->element("Flop");
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN(
                "the schema matches arrays of size 3 containing \"Hello world!\", a number and arrays of size 2 "
                "containing a string and true")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","definitions":{"Flap":{"type":"array","minItems":2,"items":[{"type":"string"},{"type":"boolean","enum":[true]}],"additionalItems":false},"Flop":{"type":"array","minItems":3,"items":[{"type":"string","enum":["Hello world!"]},{"type":"number"},{"$ref":"#/definitions/Flap"}],"additionalItems":false}},"$ref":"#/definitions/Flop"})");
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

    GIVEN("An empty, named EnumElement with non-empty enumerations and fixed attribute true")
    {
        auto el = make_empty<EnumElement>();
        el->element("Flap");
        {
            auto a = make_element<ArrayElement>( //
                make_empty<StringElement>(),     //
                from_primitive(true));
            a->element("Flop");

            el->attributes().set("enumerations",
                make_element<ArrayElement>(         //
                    from_primitive("Hello world!"), //
                    make_empty<NumberElement>(),    //
                    std::move(a)));
        }
        el->attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = schema::generateJsonSchema(*el);

            THEN("the schema matches \"Hello world!\", a number and arrays of size 2 containing a string and true")
            {
                REQUIRE(to_string(result) == R"({"$schema":"http://json-schema.org/draft-04/schema#","definitions":{"Flop":{"type":"array","minItems":2,"items":[{"type":"string"},{"type":"boolean","enum":[true]}],"additionalItems":false},"Flap":{"anyOf":[{"type":"string","enum":["Hello world!"]},{"type":"number"},{"$ref":"#/definitions/Flop"}]}},"$ref":"#/definitions/Flap"})");
            }
        }
    }
}

namespace
{
    constexpr int run_count = 100000;

    std::string to_long_string(const so::Value& v)
    {
        std::ostringstream ss{};
        so::serialize_json(ss, v);
        return ss.str();
    }

    decltype(auto) run_json_schema(const IElement& el)
    {
        using namespace std::literals;

        using clock = std::chrono::steady_clock;
        using result_type = clock::duration;

        result_type result = 0s;

        std::ostringstream ss;
        for (int i = run_count; i > 0; --i) {
            const auto start = clock::now();
            serialize_json(ss, schema::generateJsonSchema(el));
            const auto end = clock::now();
            REQUIRE(!ss.str().empty());
            ss.str("");
            result += end - start;
        }
        LOG(warning) << to_long_string(schema::generateJsonSchema(el));
        return result;
    }

    decltype(auto) run_json_schema_legacy(const IElement& el)
    {
        using namespace std::literals;

        using clock = std::chrono::steady_clock;
        using result_type = clock::duration;

        result_type result = 0s;

        for (int i = run_count; i > 0; --i) {
            const auto start = clock::now();
            auto str = renderJsonSchema(el);
            const auto end = clock::now();
            REQUIRE(!str.empty());
            result += end - start;
        }
        LOG(warning) << renderJsonSchema(el);
        return result;
    }
} // namespace

SCENARIO("Test JSON Schema generation performance", "[json-schema-perf][.]")
{
    ENABLE_LOGGING;

    GIVEN("A StringElement")
    {
        StringElement el{ dsd::String{
            "something never printed, yet not easily copied because it is too long for SSO!" } };

        WHEN("a JSON Schema is generated from it")
        {
            auto result = run_json_schema(el);
            WARN("rendering time (us): " << duration_cast<microseconds>(result).count());
        }

        WHEN("a JSON Schema is generated from it (legacy)")
        {
            auto result = run_json_schema_legacy(el);
            WARN("rendering time (us): " << duration_cast<microseconds>(result).count());
        }
    }

    GIVEN("A StringElement; explicitely fixed; value in content")
    {
        StringElement el{ dsd::String{ "foo-bar" } };
        el.attributes().set("fixed", from_primitive(true));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = run_json_schema(el);
            WARN("rendering time (us): " << duration_cast<microseconds>(result).count());
        }

        WHEN("a JSON Schema is generated from it (legacy)")
        {
            auto result = run_json_schema_legacy(el);
            WARN("rendering time (us): " << duration_cast<microseconds>(result).count());
        }
    }

    GIVEN("A BooleanElement")
    {
        BooleanElement el{ dsd::Boolean{ true } };

        WHEN("a JSON Schema is generated from it")
        {
            auto result = run_json_schema(el);
            WARN("rendering time (us): " << duration_cast<microseconds>(result).count());
        }

        WHEN("a JSON Schema is generated from it (legacy)")
        {
            auto result = run_json_schema_legacy(el);
            WARN("rendering time (us): " << duration_cast<microseconds>(result).count());
        }
    }

    GIVEN("A NumberElement")
    {
        NumberElement el{ dsd::Number{ 42 } };

        WHEN("a JSON Schema is generated from it")
        {
            auto result = run_json_schema(el);
            WARN("rendering time (us): " << duration_cast<microseconds>(result).count());
        }

        WHEN("a JSON Schema is generated from it (legacy)")
        {
            auto result = run_json_schema_legacy(el);
            WARN("rendering time (us): " << duration_cast<microseconds>(result).count());
        }
    }

    GIVEN("An ArrayElement")
    {
        ArrayElement el{ dsd::Array{ //
            from_primitive("Hello you uncomfortable world with strings easily exceeding 40 characters"),
            from_primitive(42.0),
            make_element<ArrayElement>( //
                from_primitive("short string"),
                from_primitive(true)) } };

        el.attributes().set("fixed", from_primitive(true));

        WHEN("a JSON Schema is generated from it")
        {
            auto result = run_json_schema(el);
            WARN("rendering time (us): " << duration_cast<microseconds>(result).count());
        }

        WHEN("a JSON Schema is generated from it (legacy)")
        {
            auto result = run_json_schema_legacy(el);
            WARN("rendering time (us): " << duration_cast<microseconds>(result).count());
        }
    }
}
