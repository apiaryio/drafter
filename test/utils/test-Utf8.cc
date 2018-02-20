//
//  test/utils/test-Utf8.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 18/01/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include <catch.hpp>

#include "utils/Utf8.h"

#include <string>
#include <fstream>
#include <array>
#include <utility>

using namespace drafter;
using namespace utils;
using namespace utf8;

namespace
{
    // clang-format off
    const std::array<std::pair<int, const char*>, 30>  utf8fixtures = {
        std::make_pair(  235188, "utf8_sequence_0-0x10ffff_assigned_including-unprintable-asis_unseparated.txt"),
        std::make_pair(  235188, "utf8_sequence_0-0x10ffff_assigned_including-unprintable-replaced_unseparated.txt"),
        std::make_pair(  235129, "utf8_sequence_0-0x10ffff_assigned_printable_unseparated.txt"),
        std::make_pair( 1111998, "utf8_sequence_0-0x10ffff_including-unassigned_including-unprintable-asis_unseparated.txt"),
        std::make_pair( 1111998, "utf8_sequence_0-0x10ffff_including-unassigned_including-unprintable-replaced_unseparated.txt"),
        std::make_pair(   60530, "utf8_sequence_0-0x1ffff_assigned_including-unprintable-asis_unseparated.txt"),
        std::make_pair(   60530, "utf8_sequence_0-0x1ffff_assigned_including-unprintable-replaced_unseparated.txt"),
        std::make_pair(   60471, "utf8_sequence_0-0x1ffff_assigned_printable_unseparated.txt"),
        std::make_pair(  128988, "utf8_sequence_0-0x1ffff_including-unassigned_including-unprintable-asis_unseparated.txt"),
        std::make_pair(  128988, "utf8_sequence_0-0x1ffff_including-unassigned_including-unprintable-replaced_unseparated.txt"),
        std::make_pair(  103783, "utf8_sequence_0-0x2ffff_assigned_including-unprintable-asis_unseparated.txt"),
        std::make_pair(  103783, "utf8_sequence_0-0x2ffff_assigned_including-unprintable-replaced_unseparated.txt"),
        std::make_pair(  103724, "utf8_sequence_0-0x2ffff_assigned_printable_unseparated.txt"),
        std::make_pair(  194522, "utf8_sequence_0-0x2ffff_including-unassigned_including-unprintable-asis_unseparated.txt"),
        std::make_pair(  194522, "utf8_sequence_0-0x2ffff_including-unassigned_including-unprintable-replaced_unseparated.txt"),
        std::make_pair(     256, "utf8_sequence_0-0xff_assigned_including-unprintable-asis_unseparated.txt"),
        std::make_pair(     256, "utf8_sequence_0-0xff_assigned_including-unprintable-replaced_unseparated.txt"),
        std::make_pair(     197, "utf8_sequence_0-0xff_assigned_printable_unseparated.txt"),
        std::make_pair(    2994, "utf8_sequence_0-0xfff_assigned_including-unprintable-asis_unseparated.txt"),
        std::make_pair(    2994, "utf8_sequence_0-0xfff_assigned_including-unprintable-replaced_unseparated.txt"),
        std::make_pair(    2935, "utf8_sequence_0-0xfff_assigned_printable_unseparated.txt"),
        std::make_pair(   58140, "utf8_sequence_0-0xffff_assigned_including-unprintable-asis_unseparated.txt"),
        std::make_pair(   58140, "utf8_sequence_0-0xffff_assigned_including-unprintable-replaced_unseparated.txt"),
        std::make_pair(   58081, "utf8_sequence_0-0xffff_assigned_printable_unseparated.txt"),
        std::make_pair(   63454, "utf8_sequence_0-0xffff_including-unassigned_including-unprintable-asis_unseparated.txt"),
        std::make_pair(   63454, "utf8_sequence_0-0xffff_including-unassigned_including-unprintable-replaced_unseparated.txt"),
        std::make_pair(    4096, "utf8_sequence_0-0xfff_including-unassigned_including-unprintable-asis_unseparated.txt"),
        std::make_pair(    4096, "utf8_sequence_0-0xfff_including-unassigned_including-unprintable-replaced_unseparated.txt"),
        std::make_pair(     256, "utf8_sequence_0-0xff_including-unassigned_including-unprintable-asis_unseparated.txt"),
        std::make_pair(     256, "utf8_sequence_0-0xff_including-unassigned_including-unprintable-replaced_unseparated.txt")
    };
    // clang-format on
}

SCENARIO("iteration over ASCII strings", "[utf8]")
{
    using tested = input_iterator<std::string::const_iterator>;

    GIVEN("An empty std::string and utf8 iterators to its begin & end")
    {
        std::string str{};

        tested b{ str.begin(), str.end() };
        tested e{ str.end(), str.end() };

        THEN("the iterators equal")
        {
            REQUIRE(b == e);
        }
    }

    GIVEN("A std::string{\"Foo\\n\"} and utf8 iterators to its begin & end")
    {
        std::string str{ "Fo\n" };

        tested b{ str.begin(), str.end() };
        tested e{ str.end(), str.end() };

        WHEN("the utf iterator to begin is incremented 0 times")
        {
            THEN("dereferencing it yields 0x46 (F)")
            {
                REQUIRE(*b == 0x46); // codepoint for 'F'
            }
        }

        WHEN("the utf iterator to begin is incremented 1 time")
        {
            ++b;
            THEN("dereferencing it yields 0x6F (o)")
            {
                REQUIRE(*b == 0x6F); // codepoint for 'o'
            }
        }

        WHEN("the utf iterator to begin is incremented 2 times")
        {
            ++b;
            ++b;
            THEN("dereferencing it yields 0x0A (\\n)")
            {
                REQUIRE(*b == 0x0A); // codepoint for '\n'
            }
        }

        WHEN("the utf iterator to begin is incremented 3 times")
        {
            ++b;
            ++b;
            ++b;
            THEN("it equals the utf iterator to the end")
            {
                REQUIRE(b == e);
            }
        }
    }
}

SCENARIO("iteration over utf-8 fixtures", "[utf8]")
{
    using tested = input_iterator<std::string::const_iterator>;

    GIVEN("std::strings constructed from fixtures and utf iterators to them")
    {
        for (const auto& file_entry : utf8fixtures) {

            std::ifstream input_is{ //
                std::string("test/fixtures/utf-8/") + file_entry.second,
                std::ios_base::binary
            };
            REQUIRE(input_is);

            const std::string str( //
                (std::istreambuf_iterator<char>(input_is)),
                std::istreambuf_iterator<char>());

            tested b{ str.begin(), str.end() };
            tested e{ str.end(), str.end() };

            WHEN("the utf iterator to begin is incremented until it is equal to end")
            {
                int i = 0;
                for (; b != e; ++b, ++i)
                    ;

                THEN("it has been incremented `wc -m $fixture` times")
                {
                    REQUIRE(file_entry.first == i);
                }
            }

            WHEN("serialize(codepoint) is used on deref'd iterator")
            {
                THEN("the result is the same as the original input")
                {
                    auto p_exp = str.begin();
                    for (; b != e; ++b) {
                        std::vector<char> cs;

                        // TODO @tjanc@ also test return value
                        serialize(*b, std::back_insert_iterator<std::vector<char> >(cs));

                        for (const char& c : cs) {
                            if (c != *p_exp) // Catch REQUIRE not used; would clutter
                                break;       // number of assertions in Catch output
                            ++p_exp;
                        }
                    }

                    // asserting we arrived at the end is sufficient
                    REQUIRE(p_exp == str.end());
                }
            }
        }
    }
}

SCENARIO("iteration over non utf-8 strings", "[utf8]")
{
    using tested = input_iterator<std::string::const_iterator>;

    GIVEN("a string ending prematurely; and an utf8 iterator to it")
    {
        std::string str{ "ab\xC2" };

        tested b{ str.begin(), str.end() };

        WHEN("the iterator is advanced two times")
        {
            using std::advance;
            advance(b, 2);

            THEN("it dereferences to the replacement character, 0xFFFD")
            {
                REQUIRE(0xFFFD == *b);
            }

            WHEN("incremented")
            {
                ++b;

                THEN("it is equal to an utf8 iterator to the end of the string")
                {
                    tested end{ str.end(), str.end() };
                    REQUIRE(b == end);
                }
            }
        }
    }

    // TODO @tjanc@ test ending prematurely 3-byte
    // TODO @tjanc@ test ending prematurely 4-byte
    // TODO @tjanc@ test oversized 2-byte
    // TODO @tjanc@ test oversized 3-byte
    // TODO @tjanc@ test oversized 4-byte
    // TODO @tjanc@ test surrogates as codepoints (3-byte)
}
