#include "parser/Uritemplate.h"
#include <tao/pegtl/contrib/tracer.hpp>

#include <catch2/catch.hpp>

using namespace apib::parser::uritemplate;
using namespace Catch;

TEST_CASE("most simple template")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("{val}", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 1);

    auto e = mpark::get<state::expression>(result[0]);
    REQUIRE_FALSE(e.missing_expression_close);
    REQUIRE(e.variables.size() == 1);

    auto v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "val");
}

TEST_CASE("multiple variables template")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("{v1,v2,v3}", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 1);

    auto e = mpark::get<state::expression>(result[0]);
    REQUIRE(e.variables.size() == 3);

    auto v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "v1");

    v = mpark::get<state::variable>(e.variables[1]);
    REQUIRE(v.name == "v2");

    v = mpark::get<state::variable>(e.variables[2]);
    REQUIRE(v.name == "v3");
}

TEST_CASE("mod level 4")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("{v1,v2:333,v3*,v4}", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 1);

    auto e = mpark::get<state::expression>(result[0]);
    REQUIRE(e.type == state::expression_type::noop);
    REQUIRE_FALSE(e.missing_expression_close);

    REQUIRE(e.variables.size() == 4);

    auto v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "v1");
    REQUIRE(v.explode == false);
    REQUIRE(v.prefix == 0);

    v = mpark::get<state::variable>(e.variables[1]);
    REQUIRE(v.name == "v2");
    REQUIRE(v.explode == false);
    REQUIRE(v.prefix == 333);

    v = mpark::get<state::variable>(e.variables[2]);
    REQUIRE(v.name == "v3");
    REQUIRE(v.explode == true);
    REQUIRE(v.prefix == 0);

    v = mpark::get<state::variable>(e.variables[3]);
    REQUIRE(v.name == "v4");
    REQUIRE(v.explode == false);
    REQUIRE(v.prefix == 0);
}

TEST_CASE("operators l2, l3")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("{+reserved}{#fragment}{.label}{/path}{;param}{?query}{&param}", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 7);

    auto e = mpark::get<state::expression>(result[0]);
    REQUIRE(e.type == state::expression_type::reserved_chars);
    REQUIRE(e.variables.size() == 1);
    auto v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "reserved");

    e = mpark::get<state::expression>(result[1]);
    REQUIRE(e.type == state::expression_type::fragment);
    v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(e.variables.size() == 1);
    REQUIRE(v.name == "fragment");

    e = mpark::get<state::expression>(result[2]);
    REQUIRE(e.type == state::expression_type::label);
    REQUIRE(e.variables.size() == 1);
    v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "label");

    e = mpark::get<state::expression>(result[3]);
    REQUIRE(e.type == state::expression_type::path);
    REQUIRE(e.variables.size() == 1);
    v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "path");

    e = mpark::get<state::expression>(result[4]);
    REQUIRE(e.type == state::expression_type::path_param);
    REQUIRE(e.variables.size() == 1);
    v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "param");

    e = mpark::get<state::expression>(result[5]);
    REQUIRE(e.type == state::expression_type::query_param);
    REQUIRE(e.variables.size() == 1);
    v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "query");

    e = mpark::get<state::expression>(result[6]);
    REQUIRE(e.type == state::expression_type::query_continue);
    REQUIRE(e.variables.size() == 1);
    v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "param");
}

TEST_CASE("reserved operators")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("{=equal}{,comma}{!exclamation}{@at}{|pipe}", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 5);

    auto e = mpark::get<state::expression>(result[0]);
    REQUIRE(e.type == state::expression_type::reserved);
    REQUIRE(e.variables.size() == 1);
    auto v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "equal");

    e = mpark::get<state::expression>(result[1]);
    REQUIRE(e.type == state::expression_type::reserved);
    REQUIRE(e.variables.size() == 1);
    v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "comma");

    e = mpark::get<state::expression>(result[2]);
    REQUIRE(e.type == state::expression_type::reserved);
    REQUIRE(e.variables.size() == 1);
    v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "exclamation");

    e = mpark::get<state::expression>(result[3]);
    REQUIRE(e.type == state::expression_type::reserved);
    REQUIRE(e.variables.size() == 1);
    v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "at");

    e = mpark::get<state::expression>(result[4]);
    REQUIRE(e.type == state::expression_type::reserved);
    REQUIRE(e.variables.size() == 1);
    v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "pipe");
}

TEST_CASE("just literals")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("/simple/path", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 1);

    auto e = mpark::get<state::literals>(result[0]);
    REQUIRE(e == "/simple/path");
}

TEST_CASE("literals with variable")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("/begin/{name}/end", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 3);

    auto l = mpark::get<state::literals>(result[0]);
    REQUIRE(l == "/begin/");

    auto e = mpark::get<state::expression>(result[1]);
    REQUIRE(e.type == state::expression_type::noop);
    REQUIRE(e.variables.size() == 1);
    auto v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "name");

    l = mpark::get<state::literals>(result[2]);
    REQUIRE(l == "/end");
}

TEST_CASE("recovery parsing with simple varname")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("{v1,v^2,v3}", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 1);

    auto e = mpark::get<state::expression>(result[0]);
    REQUIRE(e.variables.size() == 3);

    auto v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "v1");

    auto i = mpark::get<state::invalid>(e.variables[1]);
    REQUIRE(i.content == "v^2");

    v = mpark::get<state::variable>(e.variables[2]);
    REQUIRE(v.name == "v3");
}

TEST_CASE("recovery parsing on `explode`")
{ 

    state::uritemplate result;
    tao::pegtl::memory_input<> in("{v1,v2**,v3}", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 1);

    auto e = mpark::get<state::expression>(result[0]);
    REQUIRE(e.variables.size() == 3);

    auto v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "v1");

    auto i = mpark::get<state::invalid>(e.variables[1]);
    REQUIRE(i.content == "v2**");

    v = mpark::get<state::variable>(e.variables[2]);
    REQUIRE(v.name == "v3");
}

TEST_CASE("recovery parsing on `prefix`")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("{v1,v2:1a,v3}", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 1);

    auto e = mpark::get<state::expression>(result[0]);
    REQUIRE(e.variables.size() == 3);

    auto v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "v1");

    auto i = mpark::get<state::invalid>(e.variables[1]);
    REQUIRE(i.content == "v2:1a");

    v = mpark::get<state::variable>(e.variables[2]);
    REQUIRE(v.name == "v3");
}

TEST_CASE("unknown operator")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("{-v1,v2}", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 1);

    auto e = mpark::get<state::expression>(result[0]);
    REQUIRE(e.variables.size() == 2);

    auto i = mpark::get<state::invalid>(e.variables[0]);
    REQUIRE(i.content == "-v1");

    auto v = mpark::get<state::variable>(e.variables[1]);
    REQUIRE(v.name == "v2");
}

TEST_CASE("unclosed template operator")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("{v1,v-2", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 1);

    auto e = mpark::get<state::expression>(result[0]);
    REQUIRE(e.missing_expression_close);
    REQUIRE(e.variables.size() == 2);

    auto v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "v1");

    auto i = mpark::get<state::invalid>(e.variables[1]);
    REQUIRE(i.content == "v-2");
}

TEST_CASE("inner template try")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("{v1{}v2}", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 2);

    auto e = mpark::get<state::expression>(result[0]);
    REQUIRE(e.variables.size() == 1);

    auto i = mpark::get<state::invalid>(e.variables[0]);
    REQUIRE(i.content == "v1{");

    auto l = mpark::get<state::invalid>(result[1]);
    REQUIRE(l.content == "v2}");
}

TEST_CASE("space in literal")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("xx yy", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 1);

    auto l = mpark::get<state::invalid>(result[0]);
    REQUIRE(l.content == "xx yy");
}

TEST_CASE("square brackets in literal")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("/a[2]", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 1);

    auto l = mpark::get<state::invalid>(result[0]);
    REQUIRE(l.content == "/a[2]");
}

TEST_CASE("recover both literal and expression")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("xx yy/{x-2}/ s", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 3);
    auto l = mpark::get<state::invalid>(result[0]);
    REQUIRE(l.content == "xx yy/");

    auto e = mpark::get<state::expression>(result[1]);
    REQUIRE(e.variables.size() == 1);

    auto i = mpark::get<state::invalid>(e.variables[0]);
    REQUIRE(i.content == "x-2");

    l = mpark::get<state::invalid>(result[2]);
    REQUIRE(l.content == "/ s");
}

TEST_CASE("complete URI")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("http://example.com/xxyy/{x}?abc=def{&par}", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 4);
    auto l = mpark::get<state::literals>(result[0]);
    REQUIRE(l == "http://example.com/xxyy/");

    auto e = mpark::get<state::expression>(result[1]);
    REQUIRE(e.variables.size() == 1);
    REQUIRE(e.type == state::expression_type::noop);

    auto v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "x");

    l = mpark::get<state::literals>(result[2]);
    REQUIRE(l == "?abc=def");

    e = mpark::get<state::expression>(result[3]);
    REQUIRE(e.variables.size() == 1);
    REQUIRE(e.type == state::expression_type::query_continue);

    v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "par");
}

TEST_CASE("dots in variable")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("{a.b.c}", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 1);

    auto e = mpark::get<state::expression>(result[0]);
    REQUIRE(e.variables.size() == 1);

    auto v = mpark::get<state::variable>(e.variables[0]);
    REQUIRE(v.name == "a.b.c");
}

TEST_CASE("double dots in variable")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("{a..b}", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 1);

    auto e = mpark::get<state::expression>(result[0]);
    REQUIRE(e.variables.size() == 1);

    auto i = mpark::get<state::invalid>(e.variables[0]);
    REQUIRE(i.content == "a..b");
}

TEST_CASE("invalid pct-encoded triplet")
{ 
    state::uritemplate result;
    tao::pegtl::memory_input<> in("{a%2zb}", "");
    REQUIRE(tao::pegtl::parse<match_grammar, action>(in, result));
    REQUIRE(result.size() == 1);

    auto e = mpark::get<state::expression>(result[0]);
    REQUIRE(e.variables.size() == 1);

    auto i = mpark::get<state::invalid>(e.variables[0]);
    REQUIRE(i.content == "a%2zb");
}
