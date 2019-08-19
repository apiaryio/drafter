
#include "ContentTypeMatcher.h"

#include <catch2/catch.hpp>

using state = parser::mediatype::state;
using params = state::parameters_type;
using namespace drafter;

TEST_CASE("test JSON comparator")
{

    JSONContentTypeComparator json;

    REQUIRE(json == state{ "application", "json", {}, params()});
    REQUIRE(json == state{ "application", "hal", "json", params()});
    REQUIRE(json == state{ "application", "json", {}, { { "key", "value" } } });

    REQUIRE_FALSE(json == state{ "dummy", "hal", "json", params() });
}

TEST_CASE("test matchers")
{
    REQUIRE(IsJSONSchemaContentType("application/schema+json"));
    REQUIRE(IsJSONSchemaContentType("application/schema+json ; version=1"));
    REQUIRE(IsJSONSchemaContentType("    application/schema+json "));

    REQUIRE(IsJSONContentType("application/json"));
    REQUIRE(IsJSONContentType("application/hal+json"));
    REQUIRE(IsJSONContentType("application/json; some=param and=another"));
    REQUIRE(IsJSONContentType("     application/json  "));

    REQUIRE_FALSE(IsJSONContentType("application/schema+json"));

    REQUIRE_FALSE(IsJSONSchemaContentType("dummy/schema+json"));
    REQUIRE_FALSE(IsJSONContentType("dummy/json"));
    REQUIRE_FALSE(IsJSONContentType("dummy/hal+json"));
}

TEST_CASE("test case insensitivity")
{
    REQUIRE(IsJSONSchemaContentType("Application/schema+json"));
    REQUIRE(IsJSONSchemaContentType("Application/schemA+json"));
    REQUIRE(IsJSONSchemaContentType("Application/schemA+Json"));

    REQUIRE(IsJSONContentType("Application/json"));
    REQUIRE(IsJSONContentType("application/JSON"));
    REQUIRE(IsJSONContentType("application/HAL+json"));

    REQUIRE_FALSE(IsJSONContentType("application/SCHEMA+json"));
}


struct ThrowingComparator {
    bool operator==(const parser::mediatype::state& other) const
    {
        throw "dummy error";
    }
};

TEST_CASE("throwing comparator")
{
    // we should use excepttion matching for specific type of exception
    // but gcc:4.8.5 on CI does not handle REQUIRE_THROWS_WITH() correctly
    // and finish test run with segfault. 

    //REQUIRE_THROWS_WITH((ContentTypeMatcher{}(ThrowingComparator{}, "a/b")), "dummy error");
    REQUIRE_THROWS(ContentTypeMatcher{}(ThrowingComparator{}, "a/b"));
}
