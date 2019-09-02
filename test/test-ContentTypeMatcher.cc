
#include "ContentTypeMatcher.h"
#include <catch2/catch.hpp>


using state = apib::parser::mediatype::state;
using namespace drafter;

TEST_CASE("test matchers")
{
    REQUIRE(IsJSONSchemaContentType(parseMediaType("application/schema+json")));
    REQUIRE(IsJSONSchemaContentType(parseMediaType("application/schema+json ; version=1")));
    REQUIRE(IsJSONSchemaContentType(parseMediaType("    application/schema+json ")));

    REQUIRE(IsJSONContentType(parseMediaType("application/json")));
    REQUIRE(IsJSONContentType(parseMediaType("application/hal+json")));
    REQUIRE(IsJSONContentType(parseMediaType("application/json; some=param and=another")));
    REQUIRE(IsJSONContentType(parseMediaType("     application/json  ")));

    REQUIRE_FALSE(IsJSONContentType(parseMediaType("application/schema+json")));

    REQUIRE_FALSE(IsJSONSchemaContentType(parseMediaType("dummy/schema+json")));
    REQUIRE_FALSE(IsJSONContentType(parseMediaType("dummy/json")));
    REQUIRE_FALSE(IsJSONContentType(parseMediaType("dummy/hal+json")));
}

TEST_CASE("test case insensitivity")
{
    REQUIRE(IsJSONSchemaContentType(parseMediaType("Application/schema+json")));
    REQUIRE(IsJSONSchemaContentType(parseMediaType("Application/schemA+json")));
    REQUIRE(IsJSONSchemaContentType(parseMediaType("Application/schemA+Json")));

    REQUIRE(IsJSONContentType(parseMediaType("Application/json")));
    REQUIRE(IsJSONContentType(parseMediaType("application/JSON")));
    REQUIRE(IsJSONContentType(parseMediaType("application/HAL+json")));

    REQUIRE_FALSE(IsJSONContentType(parseMediaType("application/SCHEMA+json")));
}

