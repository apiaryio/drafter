#include <apib/syntax/MediaType.h>

#include <catch2/catch.hpp>

using namespace apib::syntax;

TEST_CASE("test matchers")
{
    REQUIRE(apib::isJSONSchema(media_type{ "application", "schema", "json", {} }));
    REQUIRE(apib::isJSONSchema(media_type{ "application", "schema", "json", { { "version", "1" } } }));

    REQUIRE(apib::isJSON(media_type{ "application", "json", "", {} }));
    REQUIRE(apib::isJSON(media_type{ "application", "hal", "json", {} }));
    REQUIRE(apib::isJSON(media_type{ "application", "json", "", { { "some", "param" }, { "and", "another" } } }));

    REQUIRE(apib::isJSON(media_type{ "application", "schema", "json", {} }));

    REQUIRE_FALSE(apib::isJSONSchema(media_type{ "dummy", "schema", "json", {} }));
    REQUIRE_FALSE(apib::isJSON(media_type{ "dummy", "json", "", {} }));
    REQUIRE_FALSE(apib::isJSON(media_type{ "dummy", "hal", "json", {} }));
}

TEST_CASE("test case insensitivity")
{
    REQUIRE(apib::isJSONSchema(media_type{ "Application", "schema", "json", {} }));
    REQUIRE(apib::isJSONSchema(media_type{ "Application", "schemA", "json", {} }));
    REQUIRE(apib::isJSONSchema(media_type{ "Application", "schemA", "Json", {} }));

    REQUIRE(apib::isJSON(media_type{ "Application", "json", "", {} }));
    REQUIRE(apib::isJSON(media_type{ "application", "JSON", "", {} }));
    REQUIRE(apib::isJSON(media_type{ "application", "HAL", "json", {} }));

    REQUIRE(apib::isJSON(media_type{ "application", "SCHEMA", "json", {} }));
}
