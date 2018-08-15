//
//  test/utils/so/test-JsonIo.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 23/01/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include <catch.hpp>

#include <iostream>
#include <fstream>
#include <array>
#include <string>
#include <limits>

#include "utils/so/JsonIo.h"

using namespace drafter;
using namespace utils;
using namespace so;

using namespace std::literals;

namespace
{
    const std::array<const char*, 30> utf8fixtures = {
        //
        "utf8_sequence_0-0x10ffff_assigned_including-unprintable-asis_unseparated.txt",
        "utf8_sequence_0-0x10ffff_assigned_including-unprintable-replaced_unseparated.txt",
        "utf8_sequence_0-0x10ffff_assigned_printable_unseparated.txt",
        "utf8_sequence_0-0x10ffff_including-unassigned_including-unprintable-asis_unseparated.txt",
        "utf8_sequence_0-0x10ffff_including-unassigned_including-unprintable-replaced_unseparated.txt",
        "utf8_sequence_0-0x1ffff_assigned_including-unprintable-asis_unseparated.txt",
        "utf8_sequence_0-0x1ffff_assigned_including-unprintable-replaced_unseparated.txt",
        "utf8_sequence_0-0x1ffff_assigned_printable_unseparated.txt",
        "utf8_sequence_0-0x1ffff_including-unassigned_including-unprintable-asis_unseparated.txt",
        "utf8_sequence_0-0x1ffff_including-unassigned_including-unprintable-replaced_unseparated.txt",
        "utf8_sequence_0-0x2ffff_assigned_including-unprintable-asis_unseparated.txt",
        "utf8_sequence_0-0x2ffff_assigned_including-unprintable-replaced_unseparated.txt",
        "utf8_sequence_0-0x2ffff_assigned_printable_unseparated.txt",
        "utf8_sequence_0-0x2ffff_including-unassigned_including-unprintable-asis_unseparated.txt",
        "utf8_sequence_0-0x2ffff_including-unassigned_including-unprintable-replaced_unseparated.txt",
        "utf8_sequence_0-0xff_assigned_including-unprintable-asis_unseparated.txt",
        "utf8_sequence_0-0xff_assigned_including-unprintable-replaced_unseparated.txt",
        "utf8_sequence_0-0xff_assigned_printable_unseparated.txt",
        "utf8_sequence_0-0xfff_assigned_including-unprintable-asis_unseparated.txt",
        "utf8_sequence_0-0xfff_assigned_including-unprintable-replaced_unseparated.txt",
        "utf8_sequence_0-0xfff_assigned_printable_unseparated.txt",
        "utf8_sequence_0-0xffff_assigned_including-unprintable-asis_unseparated.txt",
        "utf8_sequence_0-0xffff_assigned_including-unprintable-replaced_unseparated.txt",
        "utf8_sequence_0-0xffff_assigned_printable_unseparated.txt",
        "utf8_sequence_0-0xffff_including-unassigned_including-unprintable-asis_unseparated.txt",
        "utf8_sequence_0-0xffff_including-unassigned_including-unprintable-replaced_unseparated.txt",
        "utf8_sequence_0-0xfff_including-unassigned_including-unprintable-asis_unseparated.txt",
        "utf8_sequence_0-0xfff_including-unassigned_including-unprintable-replaced_unseparated.txt",
        "utf8_sequence_0-0xff_including-unassigned_including-unprintable-asis_unseparated.txt",
        "utf8_sequence_0-0xff_including-unassigned_including-unprintable-replaced_unseparated.txt"
    };
}

// clang-format off
namespace {
const std::string shallow_array_indented = {
R"JSON([
  "Hello world!",
  5
])JSON"};

const std::string shallow_array_packed = {
R"JSON(["Hello world!",5])JSON"};

const std::string shallow_object_indented = {
R"JSON({
  "foo": "Hello world!",
  "bar": 5
})JSON"};

const std::string shallow_object_packed = {
R"JSON({"foo":"Hello world!","bar":5})JSON"};

const std::string deep_object_indented {
R"JSON({
  "foo": "Hello world!",
  "empty": {},
  "bar": {
    "id": 5,
    "data": [
      "Here comes the sun",
      {
        "type": "blob"
      }
    ]
  }
})JSON"};

const std::string deep_object_packed {
R"JSON({"foo":"Hello world!","empty":{},"bar":{"id":5,"data":["Here comes the sun",{"type":"blob"}]}})JSON"
};
}
// clang-format on

namespace
{
    template <typename It>
    void dump_string(std::ostream& out, It b, It e)
    {
        for (; b != e; ++b)
            out << std::setfill(' ') << std::setw(2) << *b << ' ';
    }

    template <typename It>
    void dump_hex_string(std::ostream& out, It b, It e)
    {
        for (; b != e; ++b)
            out << std::setfill('0') << std::setw(2) << std::hex
                << static_cast<unsigned int>(reinterpret_cast<const std::uint8_t&>(*b)) << ' ';
    }

    template <typename It>
    const It& clamp(const It& it, const It& b, const It& e)
    {
        if (it < b)
            return b;
        if (it > e)
            return e;
        return it;
    };
} // namespace

SCENARIO("Serialize strings from utf-8 fixtures as JSON", "[simple-object][json][utf-8]")
{
    GIVEN("a String constructed in place from fixtures/utf-8/")
    {
        WHEN("it is serialized into JSON")
        {
            for (const auto& file_name : utf8fixtures) {

                std::ifstream input_is{ //
                    std::string("test/fixtures/utf-8/") + file_name,
                    std::ios_base::binary
                };
                REQUIRE(input_is);

                const Value value( //
                    in_place_type<String>{},
                    std::string( //
                        (std::istreambuf_iterator<char>(input_is)),
                        std::istreambuf_iterator<char>()));

                std::ostringstream ss{};
                serialize_json(ss, value, packed{});
                const std::string actual = ss.str();

                THEN("the stringstream contains matching JSON from fixtures/utf-8/")
                {

                    std::ifstream expected_is{ //
                        std::string("test/fixtures/utf-8/") + file_name + ".json",
                        std::ios_base::binary
                    };
                    REQUIRE(expected_is);

                    const std::string expected( //
                        (std::istreambuf_iterator<char>(expected_is)),
                        std::istreambuf_iterator<char>());

                    auto act = actual.begin();
                    auto exp = expected.begin();

                    const auto start_exp = expected.begin();
                    const auto end_exp = expected.end();

                    const auto start_act = actual.begin();
                    const auto end_act = actual.end();

                    for (; act != end_act && exp != end_exp; ++act, ++exp) {
                        if (*act != *exp) {

                            std::stringstream result;
                            result << "[\t" << (act - start_act) << "]\t";
                            result << "expected " << *exp << " (";
                            dump_hex_string(result, exp, exp + 1);
                            result << ") but got " << *act << " (";
                            dump_hex_string(result, act, act + 1);
                            result << ")\n";

                            dump_hex_string(result, //
                                clamp(exp - 2, start_exp, end_exp),
                                clamp(exp + 8, start_exp, end_exp));
                            result << '\n';
                            dump_string(result, //
                                clamp(exp - 2, start_exp, end_exp),
                                clamp(exp + 8, start_exp, end_exp));
                            result << '\n';

                            dump_hex_string(result, //
                                clamp(act - 2, start_act, end_act),
                                clamp(act + 8, start_act, end_act));
                            result << '\n';
                            dump_string(result, //
                                clamp(act - 2, start_act, end_act),
                                clamp(act + 8, start_act, end_act));
                            result << '\n';

                            result << '\n';

                            FAIL(result.str());
                        }
                    }

                    REQUIRE(act == end_act); // expected less characters
                    REQUIRE(exp == end_exp); // expected more characters
                }
            }
        }
    }
}

SCENARIO("Serialize strings containing control characters", "[simple-object][json]")
{
    GIVEN("an in place constructed String{`\\n\\r`}")
    {
        Value value(in_place_type<String>{}, "\n\r");

        WHEN("it is serialized into stringstream as JSON")
        {
            std::stringstream ss;
            serialize_json(ss, value, packed{});

            THEN("the stringstream contains: \"\\n\\r\"")
            {
                REQUIRE(ss.str() == "\"\\n\\r\"");
            }
        }
    }

    GIVEN("an in place constructed String{`\\n\\0abc\\u7F\\u14\\/\\u9F\\r`}")
    {
        Value value(in_place_type<String>{},
            std::string{ "\n\x00"
                         "abc\x7F\x14\\/\x9F\r",
                11 }); // needs to be specified because 0x00
        std::string expected{ "\"\\n\\u0000abc\x7f\\u0014\\\\/\x9f\\r\"" };

        WHEN("it is serialized into stringstream as JSON")
        {
            std::ostringstream ss;
            serialize_json(ss, value, packed{});
            const std::string actual = ss.str();

            THEN("the stringstream contains: \"\"\\n\\u0000abc\\x7f\\u0014\\\\/\\x9f\\r\"")
            {
                REQUIRE(actual == expected);
            }
        }
    }

    GIVEN("an in place constructed String{`ࠀ`}")
    {
        Value value(in_place_type<String>{}, std::string{ "ࠀ" });
        std::string expected{ "\"ࠀ\"" };

        WHEN("it is serialized into stringstream as JSON")
        {
            std::stringstream ss;
            serialize_json(ss, value, packed{});

            THEN("the stringstream contains: \"ࠀ\"")
            {
                REQUIRE(ss.str() == expected);
            }
        }
    }
}

SCENARIO("Serialize a utils::so::Value into indented and/or packed JSON", "[simple-object][json]")
{
    GIVEN("an in place constructed Null in Value")
    {
        Value value(in_place_type<Null>{});

        WHEN("it is serialized into stringstream as indented JSON")
        {
            std::stringstream ss;
            serialize_json(ss, value);

            THEN("the stringstream contains: null")
            {
                REQUIRE(ss.str() == "null");
            }
        }

        WHEN("it is serialized into stringstream as packed JSON")
        {
            std::stringstream ss;
            serialize_json(ss, value, packed{});

            THEN("the stringstream contains: null")
            {
                REQUIRE(ss.str() == "null");
            }
        }
    }

    GIVEN("an in place constructed True in Value")
    {
        Value value(in_place_type<True>{});

        WHEN("it is serialized into stringstream as indented JSON")
        {
            std::stringstream ss;
            serialize_json(ss, value);

            THEN("the stringstream contains: true")
            {
                REQUIRE(ss.str() == "true");
            }
        }

        WHEN("it is serialized into stringstream as packed JSON")
        {
            std::stringstream ss;
            serialize_json(ss, value, packed{});

            THEN("the stringstream contains: true")
            {
                REQUIRE(ss.str() == "true");
            }
        }
    }

    GIVEN("an in place constructed False in Value")
    {
        Value value(in_place_type<False>{});

        WHEN("it is serialized into stringstream as indented JSON")
        {
            std::stringstream ss;
            serialize_json(ss, value);

            THEN("the stringstream contains: false")
            {
                REQUIRE(ss.str() == "false");
            }
        }

        WHEN("it is serialized into stringstream as packed JSON")
        {
            std::stringstream ss;
            serialize_json(ss, value, packed{});

            THEN("the stringstream contains: false")
            {
                REQUIRE(ss.str() == "false");
            }
        }
    }

    GIVEN("an in place constructed String{`Hello world!`}")
    {
        Value value(in_place_type<String>{}, "Hello world!");

        WHEN("it is serialized into stringstream as indented JSON")
        {
            std::stringstream ss;
            serialize_json(ss, value);

            THEN("the stringstream contains: \"Hello world!\"")
            {
                REQUIRE(ss.str() == "\"Hello world!\"");
            }
        }

        WHEN("it is serialized into stringstream as packed JSON")
        {
            std::stringstream ss;
            serialize_json(ss, value, packed{});

            THEN("the stringstream contains: \"Hello world!\"")
            {
                REQUIRE(ss.str() == "\"Hello world!\"");
            }
        }
    }

    GIVEN("an in place constructed Array{from_list{}, String{`Hello world!`}, Number{5}}")
    {
        Value value(in_place_type<Array>{}, from_list{}, String{ "Hello world!" }, Number{ 5 });

        WHEN("it is serialized into stringstream as indented JSON")
        {
            std::stringstream ss;
            serialize_json(ss, value);

            THEN("the stringstream contains: [\"Hello world!\",5]")
            {
                REQUIRE(ss.str() == shallow_array_indented);
            }
        }

        WHEN("it is serialized into stringstream as packed JSON")
        {
            std::stringstream ss;
            serialize_json(ss, value, packed{});

            THEN("the stringstream contains: [\"Hello world!\",5]")
            {
                REQUIRE(ss.str() == shallow_array_packed);
            }
        }
    }

    GIVEN("an in place constructed Object{`foo` -> String{`Hello world!`}, `bar` -> Number{5}}")
    {
        Value value(in_place_type<Object>{}, //
            from_list{},
            std::make_pair("foo", String{ "Hello world!" }),
            std::make_pair("bar", Number{ 5 }));

        WHEN("it is serialized into stringstream as indented JSON")
        {
            std::stringstream ss;
            serialize_json(ss, value);

            THEN("the stringstream contains: {\\n  \"foo\": \"Hello world!\",\\n  \"bar\": 5\\n}")
            {
                REQUIRE(ss.str() == shallow_object_indented);
            }
        }

        WHEN("it is serialized into stringstream as packed JSON")
        {
            std::stringstream ss;
            serialize_json(ss, value, packed{});

            THEN("the stringstream contains: {\"foo\":\"Hello world!\",\"bar\":5}")
            {
                REQUIRE(ss.str() == shallow_object_packed);
            }
        }
    }
}

SCENARIO("Serialize a utils::so::Value holding numbers json", "[simple-object][json]")
{
    GIVEN("an in place constructed Number{0}")
    {
        Value value(in_place_type<Number>{}, 0);

        WHEN("it is serialized into stringstream as JSON")
        {
            std::stringstream ss;
            serialize_json(ss, value);

            THEN("the stringstream contains: 0")
            {
                REQUIRE("0" == ss.str());
            }
        }
    }
}

SCENARIO("Serialize a utils::so::Value holding deep objects into indented json", "[simple-object][json]")
{
    GIVEN("a deep object")
    {
        Value value(in_place_type<Object>{}, //
            from_list{},
            std::make_pair("foo", String{ "Hello world!" }),
            std::make_pair("empty", Object{}),
            std::make_pair("bar",
                Object{ from_list{}, //
                    std::make_pair("id", so::Number{ 5 }),
                    std::make_pair("data",
                        Array{ from_list{}, //
                            String{ "Here comes the sun" },
                            Object{
                                from_list{},
                                std::make_pair("type", String{ "blob" }),
                            } }) }));

        WHEN("it is serialized into stringstream as indented JSON")
        {
            std::stringstream ss;
            serialize_json(ss, value);

            THEN("it serializes correctly")
            {
                REQUIRE(deep_object_indented == ss.str());
            }
        }

        WHEN("it is serialized into stringstream as packed JSON")
        {
            std::stringstream ss;
            serialize_json(ss, value, packed{});

            THEN("it serializes correctly")
            {
                REQUIRE(deep_object_packed == ss.str());
            }
        }
    }
}
