#include "grammar/Mediatype.h"

#include <apib/syntax/MediaType.h>
#include <catch2/catch.hpp>
#include <tao/pegtl.hpp>

using namespace apib::parser;
using namespace apib::syntax;

TEST_CASE("parse valid type/subtype")
{
    media_type result;
    tao::pegtl::memory_input<> in("image/jpeg", "");
    REQUIRE(tao::pegtl::parse<mediatype::match_grammar, mediatype::action /*, errors */>(in, result));
    REQUIRE(result.type == "image");
    REQUIRE(result.subtype == "jpeg");
    REQUIRE(result.suffix.empty());
    REQUIRE(result.parameters.empty());
}

TEST_CASE("parse valid type/subtype+suffix")
{
    media_type result;
    tao::pegtl::memory_input<> in("type/subtype+suffix", "");
    REQUIRE(tao::pegtl::parse<mediatype::match_grammar, mediatype::action /*, errors */>(in, result));
    REQUIRE(result.type == "type");
    REQUIRE(result.subtype == "subtype");
    REQUIRE(result.suffix == "suffix");
    REQUIRE(result.parameters.empty());
}

TEST_CASE("parse valid type/just+last+suffix+is+valid")
{
    media_type result;
    tao::pegtl::memory_input<> in("type/just+last+suffix+is+valid", "");
    REQUIRE(tao::pegtl::parse<mediatype::match_grammar, mediatype::action /*, errors */>(in, result));
    REQUIRE(result.type == "type");
    REQUIRE(result.subtype == "just+last+suffix+is");
    REQUIRE(result.suffix == "valid");
    REQUIRE(result.parameters.empty());
}

TEST_CASE("parse valid type/sub with param")
{
    media_type result;
    tao::pegtl::memory_input<> in("type/sub;key=value", "");
    REQUIRE(tao::pegtl::parse<mediatype::match_grammar, mediatype::action /*, errors */>(in, result));
    REQUIRE(result.type == "type");
    REQUIRE(result.subtype == "sub");
    REQUIRE(result.suffix.empty());
    REQUIRE(result.parameters.size() == 1);
    REQUIRE(std::get<0>(result.parameters[0]) == "key");
    REQUIRE(std::get<1>(result.parameters[0]) == "value");
}

TEST_CASE("multiple params")
{
    media_type result;
    tao::pegtl::memory_input<> in("type/sub ; key=value x=y", "");
    REQUIRE(tao::pegtl::parse<mediatype::match_grammar, mediatype::action /*, errors */>(in, result));
    REQUIRE(result.type == "type");
    REQUIRE(result.subtype == "sub");
    REQUIRE(result.suffix.empty());
    REQUIRE(result.parameters.size() == 2);
    REQUIRE(std::get<0>(result.parameters[0]) == "key");
    REQUIRE(std::get<1>(result.parameters[0]) == "value");
    REQUIRE(std::get<0>(result.parameters[1]) == "x");
    REQUIRE(std::get<1>(result.parameters[1]) == "y");
}

TEST_CASE("same params - with same key")
{ // is this correct? or there should be just one value pair with latest value?
    media_type result;
    tao::pegtl::memory_input<> in("type/sub ; b=c b=b", "");
    REQUIRE(tao::pegtl::parse<mediatype::match_grammar, mediatype::action /*, errors */>(in, result));
    REQUIRE(result.type == "type");
    REQUIRE(result.subtype == "sub");
    REQUIRE(result.suffix.empty());
    REQUIRE(result.parameters.size() == 2);
    REQUIRE(std::get<0>(result.parameters[0]) == "b");
    REQUIRE(std::get<1>(result.parameters[0]) == "c");
    REQUIRE(std::get<0>(result.parameters[1]) == "b");
    REQUIRE(std::get<1>(result.parameters[1]) == "b");
}

TEST_CASE("quoted value in param")
{
    media_type result;
    tao::pegtl::memory_input<> in("type/sub;k=\" \"", "");
    REQUIRE(tao::pegtl::parse<mediatype::match_grammar, mediatype::action /*, errors */>(in, result));
    REQUIRE(result.type == "type");
    REQUIRE(result.subtype == "sub");
    REQUIRE(result.suffix.empty());
    REQUIRE(result.parameters.size() == 1);
    REQUIRE(std::get<0>(result.parameters[0]) == "k");
    REQUIRE(std::get<1>(result.parameters[0]) == " ");
}

TEST_CASE("quoted-pairs in param valuem")
{
    media_type result;
    tao::pegtl::memory_input<> in("type/sub;k=\"\\\"\\ \"", "");
    REQUIRE(tao::pegtl::parse<mediatype::match_grammar, mediatype::action /*, errors */>(in, result));
    REQUIRE(result.type == "type");
    REQUIRE(result.subtype == "sub");
    REQUIRE(result.suffix.empty());
    REQUIRE(result.parameters.size() == 1);
    REQUIRE(std::get<0>(result.parameters[0]) == "k");
    REQUIRE(std::get<1>(result.parameters[0]) == "\\\"\\ ");
}

TEST_CASE("x-prefixed subtype")
{
    media_type result;
    tao::pegtl::memory_input<> in("type/x-prefixed", "");
    REQUIRE(tao::pegtl::parse<mediatype::match_grammar, mediatype::action /*, errors */>(in, result));
    REQUIRE(result.type == "type");
    REQUIRE(result.subtype == "x-prefixed");
    REQUIRE(result.suffix.empty());
    REQUIRE(result.parameters.empty());
}

TEST_CASE("subtype tree")
{
    media_type result;
    tao::pegtl::memory_input<> in("application/vnd.api+json", "");
    REQUIRE(tao::pegtl::parse<mediatype::match_grammar, mediatype::action /*, errors */>(in, result));
    REQUIRE(result.type == "application");
    REQUIRE(result.subtype == "vnd.api");
    REQUIRE(result.suffix == "json");
    REQUIRE(result.parameters.empty());
}

TEST_CASE("Allow reserved chars in (sub)type definition")
{
    media_type result;
    tao::pegtl::memory_input<> in("t!#$&^_-./s!#$&^_-.", "");
    REQUIRE(tao::pegtl::parse<mediatype::match_grammar, mediatype::action /*, errors */>(in, result));
    REQUIRE(result.type == "t!#$&^_-.");
    REQUIRE(result.subtype == "s!#$&^_-.");
    REQUIRE(result.suffix.empty());
    REQUIRE(result.parameters.empty());
}

// Here follows non-fully parsed cases
// we have two options how to solve:
// - check for false value returned from `parse()` and/or use patially parsed result
// - convert casses to `must<>` rule and throw, but then we will not receive result of of partially parsing

using namespace Catch::Matchers;

TEST_CASE("missing subtype")
{
    media_type result;
    tao::pegtl::memory_input<> in("type", "");
    REQUIRE_THROWS_WITH((tao::pegtl::parse<mediatype::match_grammar, mediatype::action /*, errors */>(in, result)),
        Contains(":1:4(4): parse error") && Contains("apib::parser::mediatype::slash"));
}

TEST_CASE("invalid char in type")
{
    media_type result;
    tao::pegtl::memory_input<> in("ty?pe/subtype", "");
    REQUIRE_THROWS_WITH((tao::pegtl::parse<mediatype::match_grammar, mediatype::action /*, errors */>(in, result)),
        Contains(":1:2(2): parse error") && Contains("apib::parser::mediatype::slash"));
}

TEST_CASE("double slash")
{
    media_type result;
    tao::pegtl::memory_input<> in("type/sub/type", "");
    REQUIRE_FALSE(tao::pegtl::parse<mediatype::match_grammar, mediatype::action>(in, result));
}

TEST_CASE("nonclosed quoted value in param")
{
    media_type result;
    tao::pegtl::memory_input<> in("type/sub;k=\" ", "");
    REQUIRE_FALSE(tao::pegtl::parse<mediatype::match_grammar, mediatype::action /*, errors */>(in, result));
    REQUIRE(result.type == "type");
    REQUIRE(result.subtype == "sub");
    REQUIRE(result.suffix.empty());
    REQUIRE(result.parameters.empty());
}

TEST_CASE("missing value in param")
{
    media_type result;
    tao::pegtl::memory_input<> in("type/sub;k=", "");
    REQUIRE_FALSE(tao::pegtl::parse<mediatype::match_grammar, mediatype::action /*, errors */>(in, result));
    REQUIRE(result.type == "type");
    REQUIRE(result.subtype == "sub");
    REQUIRE(result.suffix.empty());
    REQUIRE(result.parameters.empty());
}

TEST_CASE("just key in param")
{
    media_type result;
    tao::pegtl::memory_input<> in("type/sub;k", "");
    REQUIRE_FALSE(tao::pegtl::parse<mediatype::match_grammar, mediatype::action /*, errors */>(in, result));
    REQUIRE(result.type == "type");
    REQUIRE(result.subtype == "sub");
    REQUIRE(result.suffix.empty());
    REQUIRE(result.parameters.empty());
}

TEST_CASE("invalid char in param key")
{
    media_type result;
    tao::pegtl::memory_input<> in("type/sub;k/ey=v", "");
    REQUIRE_FALSE(tao::pegtl::parse<mediatype::match_grammar, mediatype::action /*, errors */>(in, result));
    REQUIRE(result.type == "type");
    REQUIRE(result.subtype == "sub");
    REQUIRE(result.suffix.empty());
    REQUIRE(result.parameters.empty());
}

TEST_CASE("invalid char in param value")
{
    media_type result;
    tao::pegtl::memory_input<> in("type/sub;key=v/alue", "");
    REQUIRE_FALSE(tao::pegtl::parse<mediatype::match_grammar, mediatype::action /*, errors */>(in, result));
    REQUIRE(result.type == "type");
    REQUIRE(result.subtype == "sub");
    REQUIRE(result.suffix.empty());
    REQUIRE(result.parameters.size() == 1);
    REQUIRE(std::get<0>(result.parameters[0]) == "key");
    REQUIRE(std::get<1>(result.parameters[0]) == "v");
}
