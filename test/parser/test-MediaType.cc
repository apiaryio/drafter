#include "parser/Mediatype.h"

#include <catch2/catch.hpp>

using namespace parser::mediatype;
using namespace tao;

TEST_CASE("test comparators") {

    REQUIRE(state{ "a", "b", "c"} == state{ "a", "b", "c"});
    REQUIRE(state{ "a", "b", "c"} != state{ "a", "b"});

    REQUIRE(state{ "a", "b"} == state{ "a", "b"});
    REQUIRE(state{ "a", "b"} != state{ "a", "b", "c"});


    // state vs string
    REQUIRE(state{ "a", "b"} == std::string{ "a/b"});
    REQUIRE(state{ "a", "b"} != std::string{ "a/b+c"});
    REQUIRE(state{ "a", "b", "c"} == std::string{ "a/b+c"});
    REQUIRE(state{ "a", "b", "c"} != std::string{ "a/b"});


    // string vs state
    REQUIRE(std::string{ "a/b+c"} == state{ "a", "b", "c"});
    REQUIRE(std::string{ "a/b"} != state{ "a", "b", "c"});

    // with params
    REQUIRE(state{ "a", "b", {}, {{"k","v"}}} == state{ "a", "b", {}, {{"k","v"}}});
    REQUIRE(state{ "a", "b", {}, {{"k","v"}}} != state{ "a", "b", {}, {{"k","x"}}});
    REQUIRE(state{ "a", "b", {}, {{"k","v"}}} != state{ "a", "b"});

    // string vs params - params are ignored
    REQUIRE(state{ "a", "b", {}, {{"k","v"}}} == std::string{ "a/b"});
}

TEST_CASE("case insensitive comparation") {
    REQUIRE(state{ "a", "b", "c"} == state{ "A", "b", "c"});
    REQUIRE(state{ "a", "b", "c"} == state{ "a", "B", "c"});
    REQUIRE(state{ "a", "b", "c"} == state{ "a", "b", "C"});
    REQUIRE(state{ "a", "b", "c"} == state{ "A", "B", "C"});

    REQUIRE(state{ "a", "b", "c", {{"key","value"}}} == state{ "a", "b", "c", {{"KEY","value"}}});
    REQUIRE(state{ "a", "b", "c", {{"key","value"}}} != state{ "a", "b", "c", {{"key","VALUE"}}});
}

TEST_CASE("case insensitive comparation state/string") {
    REQUIRE(std::string{ "A/b"} == state{ "a", "b"});
    REQUIRE(std::string{ "a/B"} == state{ "a", "b"});
    REQUIRE(std::string{ "A/B"} == state{ "a", "b"});

    REQUIRE(std::string{ "a/b"} == state{ "A", "B"});

    REQUIRE(std::string{ "A/b+c"} == state{ "a", "b", "c"});
    REQUIRE(std::string{ "A/b+C"} == state{ "a", "b", "c"});

    REQUIRE(std::string{ "a/b+c"} == state{ "A", "B", "C"});
}

TEST_CASE("parse valid type/subtype") {

    state result;
    pegtl::memory_input<> in("image/jpeg","");
    REQUIRE(pegtl::parse<match_grammar, action/*, errors */>(in, result));
    REQUIRE(result == state{"image", "jpeg", "", {}});
}

TEST_CASE("parse valid type/subtype+suffix") {

    state result;
    pegtl::memory_input<> in("type/subtype+suffix","");
    REQUIRE(pegtl::parse<match_grammar, action/*, errors */>(in, result));
}

TEST_CASE("parse valid type/just+last+suffix+is+valid") {

    state result;
    pegtl::memory_input<> in("type/just+last+suffix+is+valid","");
    REQUIRE(pegtl::parse<match_grammar, action/*, errors */>(in, result));
    REQUIRE(result == state{"type", "just+last+suffix+is", "valid", {}});
}

TEST_CASE("parse valid type/just+last+part+is+suffix") {

    state result;
    pegtl::memory_input<> in("type/just+last+part+is+suffix","");
    REQUIRE(pegtl::parse<match_grammar, action/*, errors */>(in, result));
    REQUIRE(result == state{"type", "just+last+part+is", "suffix", {}});
}

TEST_CASE("parse valid type/sub with param") {

    state result;
    pegtl::memory_input<> in("type/sub;key=value","");
    REQUIRE(pegtl::parse<match_grammar, action/*, errors */>(in, result));
    REQUIRE(result == state{"type", "sub", {}, {{"key", "value"}}});

    // additional test to compare string vs state with params - params are ignored while compare
    REQUIRE(result == "type/sub");
    REQUIRE("type/sub" == result);
}

TEST_CASE("multiple params") {

    state result;
    pegtl::memory_input<> in("type/sub ; key=value x=y","");
    REQUIRE(pegtl::parse<match_grammar, action/*, errors */>(in, result));
    REQUIRE(result == state{"type", "sub", {}, {{"key", "value"}, {"x","y"}}});
}

TEST_CASE("multiple params - order by alphaber") {

    state result;
    pegtl::memory_input<> in("type/sub; b=c a=b","");
    REQUIRE(pegtl::parse<match_grammar, action/*, errors */>(in, result));
    REQUIRE(result == state{"type", "sub", {}, {{"a", "b"}, {"b","c"}}});
}

TEST_CASE("same params - with same key") { // is this correct? or there should be just one value pair with latest value?

    state result;
    pegtl::memory_input<> in("type/sub ; b=c b=b","");
    REQUIRE(pegtl::parse<match_grammar, action/*, errors */>(in, result));
    REQUIRE(result == state{"type", "sub", {}, {{"b", "c"}, {"b","b"}}});
}

TEST_CASE("quoted value in param") {

    state result;
    pegtl::memory_input<> in("type/sub;k=\" \"","");
    REQUIRE(pegtl::parse<match_grammar, action/*, errors */>(in, result));
    REQUIRE(result == state{"type", "sub", {}, {{"k"," "}}});
}

TEST_CASE("x-prefixed subtype") {

    state result;
    pegtl::memory_input<> in("type/x-prefixed","");
    REQUIRE(pegtl::parse<match_grammar, action/*, errors */>(in, result));
    REQUIRE(result == state{"type", "x-prefixed"});
}

TEST_CASE("subtype tree") {

    state result;
    pegtl::memory_input<> in("application/vnd.api+json","");
    REQUIRE(pegtl::parse<match_grammar, action/*, errors */>(in, result));
    REQUIRE(result == state{"application", "vnd.api", "json"});
}

TEST_CASE("Allow reserved chars in (sub)type definition") {

    state result;
    pegtl::memory_input<> in("t!#$&^_-./s!#$&^_-.","");
    REQUIRE(pegtl::parse<match_grammar, action/*, errors */>(in, result));
    REQUIRE(result == state{"t!#$&^_-.", "s!#$&^_-."});
}


// Here follows non-fully parsed cases 
// we have two options how to solve:
// - check for false value returned from `parse()` and use patially parsed result
// - convert casses to `must<>` rule and throw, but then we will not receive result of of partially parsing

TEST_CASE("missing subtype") { 

    state result;
    pegtl::memory_input<> in("type","");
    REQUIRE_THROWS_WITH((pegtl::parse<match_grammar, action/*, errors */>(in, result)), ":1:4(4): parse error matching parser::mediatype::slash");
}

TEST_CASE("invalid char in type") { 
    state result;
    pegtl::memory_input<> in("ty?pe/subtype","");
    REQUIRE_THROWS_WITH((pegtl::parse<match_grammar, action/*, errors */>(in, result)), ":1:2(2): parse error matching parser::mediatype::slash");
}

TEST_CASE("double slash") { 
    state result;
    pegtl::memory_input<> in("type/sub/type","");
    REQUIRE_THROWS_WITH((pegtl::parse<match_grammar, action/*, errors */>(in, result)), ":1:8(8): parse error matching parser::mediatype::bad_slash");
}

TEST_CASE("nonclosed quoted value in param") {

    state result;
    pegtl::memory_input<> in("type/sub;k=\" ","");
    REQUIRE_FALSE(pegtl::parse<match_grammar, action/*, errors */>(in, result));
    REQUIRE(result == state{"type", "sub"});
}

TEST_CASE("missing value in param") {

    state result;
    pegtl::memory_input<> in("type/sub;k=", "");
    REQUIRE_FALSE(pegtl::parse<match_grammar, action/*, errors */>(in, result));
    REQUIRE(result == state{"type", "sub"});
}

TEST_CASE("just key in param") {

    state result;
    pegtl::memory_input<> in("type/sub;k", "");
    REQUIRE_FALSE(pegtl::parse<match_grammar, action/*, errors */>(in, result));
    REQUIRE(result == state{"type", "sub"});
}

TEST_CASE("invalid char in param key") {

    state result;
    pegtl::memory_input<> in("type/sub;k/ey=v", "");
    REQUIRE_FALSE(pegtl::parse<match_grammar, action/*, errors */>(in, result));
    REQUIRE(result == state{"type", "sub"});
}

TEST_CASE("invalid char in param value") {

    state result;
    pegtl::memory_input<> in("type/sub;key=v/alue", "");
    REQUIRE_FALSE(pegtl::parse<match_grammar, action/*, errors */>(in, result));
    REQUIRE(result == state{"type", "sub", {}, {{"key", "v"}}});
}

