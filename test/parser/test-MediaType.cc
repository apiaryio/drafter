#include "parser/Mediatype.h"

#include <catch2/catch.hpp>

using namespace apib::parser::mediatype;
using state = apib::parser::mediatype::state;
using params = state::parameters_type;


TEST_CASE("test comparators")
{

    REQUIRE(state{ "a", "b", "c", params()} == state{ "a", "b", "c", params()});
    REQUIRE(state{ "a", "b", "c", params()} != state{ "a", "b", {}, params() });

    REQUIRE(state{ "a", "b", {}, params()} == state{ "a", "b", {}, params() });
    REQUIRE(state{ "a", "b", {}, params() } != state{ "a", "b", "c", params() });

    // state vs string
    REQUIRE(state{ "a", "b", {}, params() } == std::string{ "a/b" });
    REQUIRE(state{ "a", "b", {}, params() } != std::string{ "a/b+c" });
    REQUIRE(state{ "a", "b", "c", params() } == std::string{ "a/b+c" });
    REQUIRE(state{ "a", "b", "c", params() } != std::string{ "a/b" });

    // string vs state
    REQUIRE(std::string{ "a/b+c" } == state{ "a", "b", "c", params() });
    REQUIRE(std::string{ "a/b" } != state{ "a", "b", "c", params() });

    // with params
    REQUIRE(state{ "a", "b", {}, { { "k", "v" } } } == state{ "a", "b", {}, { { "k", "v" } } });
    REQUIRE(state{ "a", "b", {}, { { "k", "v" } } } != state{ "a", "b", {}, { { "k", "x" } } });
    REQUIRE(state{ "a", "b", {}, { { "k", "v" } } } != state{ "a", "b", {}, params()});

    // string vs params - params are ignored
    REQUIRE(state{ "a", "b", {}, { { "k", "v" } } } == std::string{ "a/b" });
}

TEST_CASE("case insensitive comparation")
{
    REQUIRE(state{ "a", "b", "c", params()} == state{ "A", "b", "c", params()});
    REQUIRE(state{ "a", "b", "c", params()} == state{ "a", "B", "c", params()});
    REQUIRE(state{ "a", "b", "c", params()} == state{ "a", "b", "C", params()});
    REQUIRE(state{ "a", "b", "c", params()} == state{ "A", "B", "C", params()});

    REQUIRE(state{ "a", "b", "c", { { "key", "value" } } } == state{ "a", "b", "c", { { "KEY", "value" } } });
    REQUIRE(state{ "a", "b", "c", { { "key", "value" } } } != state{ "a", "b", "c", { { "key", "VALUE" } } });
}

TEST_CASE("case insensitive comparation state/string")
{
    REQUIRE(std::string{ "A/b" } == state{ "a", "b", {}, params()});
    REQUIRE(std::string{ "a/B" } == state{ "a", "b", {}, params()});
    REQUIRE(std::string{ "A/B" } == state{ "a", "b", {}, params()});

    REQUIRE(std::string{ "a/b" } == state{ "A", "B", {}, params()});

    REQUIRE(std::string{ "A/b+c" } == state{ "a", "b", "c", params() });
    REQUIRE(std::string{ "A/b+C" } == state{ "a", "b", "c", params() });

    REQUIRE(std::string{ "a/b+c" } == state{ "A", "B", "C", params() });
}

TEST_CASE("parse valid type/subtype")
{

    state result;
    tao::pegtl::memory_input<> in("image/jpeg", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action /*, errors */>(in, result));
    REQUIRE(result == state{ "image", "jpeg", {}, params() });
}

TEST_CASE("parse valid type/subtype+suffix")
{

    state result;
    tao::pegtl::memory_input<> in("type/subtype+suffix", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action /*, errors */>(in, result));
    REQUIRE(result == state{ "type", "subtype", "suffix", params() });
}

TEST_CASE("parse valid type/just+last+suffix+is+valid")
{

    state result;
    tao::pegtl::memory_input<> in("type/just+last+suffix+is+valid", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action /*, errors */>(in, result));
    REQUIRE(result == state{ "type", "just+last+suffix+is", "valid", params() });
}

TEST_CASE("parse valid type/sub with param")
{

    state result;
    tao::pegtl::memory_input<> in("type/sub;key=value", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action /*, errors */>(in, result));
    REQUIRE(result == state{ "type", "sub", {}, { { "key", "value" } } });

    // additional test to compare string vs state with params - params are ignored while compare
    REQUIRE(result == "type/sub");
    REQUIRE("type/sub" == result);
}

TEST_CASE("multiple params")
{

    state result;
    tao::pegtl::memory_input<> in("type/sub ; key=value x=y", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action /*, errors */>(in, result));
    REQUIRE(result == state{ "type", "sub", {}, { { "key", "value" }, { "x", "y" } } });
}

TEST_CASE("same params - with same key")
{ // is this correct? or there should be just one value pair with latest value?

    state result;
    tao::pegtl::memory_input<> in("type/sub ; b=c b=b", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action /*, errors */>(in, result));
    REQUIRE(result == state{ "type", "sub", {}, { { "b", "c" }, { "b", "b" } } });
}

TEST_CASE("quoted value in param")
{

    state result;
    tao::pegtl::memory_input<> in("type/sub;k=\" \"", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action /*, errors */>(in, result));
    REQUIRE(result == state{ "type", "sub", {}, { { "k", " " } } });
}

TEST_CASE("quoted-pairs in param valuem")
{

    state result;
    tao::pegtl::memory_input<> in("type/sub;k=\"\\\"\\ \"", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action /*, errors */>(in, result));
    REQUIRE(result == state{ "type", "sub", {}, { { "k", "\\\"\\ " } } });
}

TEST_CASE("x-prefixed subtype")
{

    state result;
    tao::pegtl::memory_input<> in("type/x-prefixed", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action /*, errors */>(in, result));
    REQUIRE(result == state{ "type", "x-prefixed", {}, params() });
}

TEST_CASE("subtype tree")
{

    state result;
    tao::pegtl::memory_input<> in("application/vnd.api+json", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action /*, errors */>(in, result));
    REQUIRE(result == state{ "application", "vnd.api", "json", params() });
}

TEST_CASE("Allow reserved chars in (sub)type definition")
{

    state result;
    tao::pegtl::memory_input<> in("t!#$&^_-./s!#$&^_-.", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action /*, errors */>(in, result));
    REQUIRE(result == state{ "t!#$&^_-.", "s!#$&^_-.", {}, params() });
}

// Here follows non-fully parsed cases
// we have two options how to solve:
// - check for false value returned from `parse()` and/or use patially parsed result
// - convert casses to `must<>` rule and throw, but then we will not receive result of of partially parsing

TEST_CASE("missing subtype")
{

    state result;
    tao::pegtl::memory_input<> in("type", "");
    REQUIRE_THROWS_WITH((tao::pegtl::parse<match_grammar, action /*, errors */>(in, result)),
        ":1:4(4): parse error matching apib::parser::mediatype::slash");
}

TEST_CASE("invalid char in type")
{
    state result;
    tao::pegtl::memory_input<> in("ty?pe/subtype", "");
    REQUIRE_THROWS_WITH((tao::pegtl::parse<match_grammar, action /*, errors */>(in, result)),
        ":1:2(2): parse error matching apib::parser::mediatype::slash");
}

TEST_CASE("double slash")
{
    state result;
    tao::pegtl::memory_input<> in("type/sub/type", "");
    REQUIRE_FALSE(tao::pegtl::parse<match_grammar, action>(in, result));
}

TEST_CASE("nonclosed quoted value in param")
{

    state result;
    tao::pegtl::memory_input<> in("type/sub;k=\" ", "");
    REQUIRE_FALSE(tao::pegtl::parse<match_grammar, action /*, errors */>(in, result));
    REQUIRE(result == state{ "type", "sub", {}, params() });
}

TEST_CASE("missing value in param")
{

    state result;
    tao::pegtl::memory_input<> in("type/sub;k=", "");
    REQUIRE_FALSE(tao::pegtl::parse<match_grammar, action /*, errors */>(in, result));
    REQUIRE(result == state{ "type", "sub", {}, params() });
}

TEST_CASE("just key in param")
{

    state result;
    tao::pegtl::memory_input<> in("type/sub;k", "");
    REQUIRE_FALSE(tao::pegtl::parse<match_grammar, action /*, errors */>(in, result));
    REQUIRE(result == state{ "type", "sub", {}, params() });
}

TEST_CASE("invalid char in param key")
{

    state result;
    tao::pegtl::memory_input<> in("type/sub;k/ey=v", "");
    REQUIRE_FALSE(tao::pegtl::parse<match_grammar, action /*, errors */>(in, result));
    REQUIRE(result == state{ "type", "sub", {}, params() });
}

TEST_CASE("invalid char in param value")
{

    state result;
    tao::pegtl::memory_input<> in("type/sub;key=v/alue", "");
    REQUIRE_FALSE(tao::pegtl::parse<match_grammar, action /*, errors */>(in, result));
    REQUIRE(result == state{ "type", "sub", {}, { { "key", "v" } } });
}

