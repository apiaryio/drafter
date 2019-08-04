#include "ContentTypeMatcher.h"

#include <catch2/catch.hpp>

using state = parser::mediatype::state;
using namespace drafter;

TEST_CASE("test JSON comparator") {

    JSONContentTypeComparator json;

    REQUIRE(json == state{"application", "json" });
    REQUIRE(json == state{"application", "hal", "json" });
    REQUIRE(json == state{"application", "json", {}, {{"key", "value"}}});

    REQUIRE_FALSE(json == state{"dummy", "hal", "json" });

}

TEST_CASE("test matchers") {
    REQUIRE(IsJSONSchemaContentType("application/schema+json"));
    REQUIRE(IsJSONSchemaContentType("application/schema+json ; version=1"));
    REQUIRE(IsJSONSchemaContentType("    application/schema+json "));

    REQUIRE(IsJSONContentType("application/json"));
    REQUIRE(IsJSONContentType("application/hal+json"));
    REQUIRE(IsJSONContentType("application/json; some=param and=another"));
    REQUIRE(IsJSONContentType("     application/json  "));

    REQUIRE_FALSE(IsJSONSchemaContentType("dummy/schema+json"));
    REQUIRE_FALSE(IsJSONContentType("dummy/json"));
    REQUIRE_FALSE(IsJSONContentType("dummy/hal+json"));
}

