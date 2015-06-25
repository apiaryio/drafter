#include "draftertest.h"

using namespace draftertest;

TEST_CASE("Testing refract serialization for primitive types", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-primitives"));
}

TEST_CASE("Testing refract serialization for named types with inheritance", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-inheritance"));
}

TEST_CASE("Testing refract serialization for array[type]", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-typed-array"));
}

TEST_CASE("Testing refract serialization for typed object", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-typed-object"));
}

TEST_CASE("Testing refract serialization for nontypped object", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-nontyped-object"));
}

TEST_CASE("Testing refract serialization for enums", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-enum"));
}

TEST_CASE("Testing refract serialization for oneof", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-oneof"));
}

TEST_CASE("Testing refract serialization for mixin", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-mixin"));
}

TEST_CASE("Testing refract serialization for nonexistent mixin", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-mixin-nonexistent", false));
}

TEST_CASE("Testing refract serialization for primitive with samples", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-string-sample"));
}

TEST_CASE("Testing refract serialization for typed array samples", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-typed-array-sample"));
}

TEST_CASE("Testing refract serialization for 'One Of' with grouped elements", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-group"));
}

TEST_CASE("Testing refract serialization array with empty sample", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-empty-sample"));
}

TEST_CASE("Testing refract serialization with inner inheritance", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-inner-inheritance"));
}

TEST_CASE("Testing refract serialization oneof w/ sample", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-oneof-sample"));
}

TEST_CASE("Testing refract serialization with multiline comments", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-multiline-description"));
}

TEST_CASE("Testing refract serialization for primitive variables", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-primitive-variables"));
}

TEST_CASE("Testing refract serialization for NamedTypes w/ type specification", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-named-with-types"));
}

TEST_CASE("Testing refract serialization for nontypes array", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-nontyped-array"));
}

TEST_CASE("Testing refract serialization for wrong number value", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-number-wrong-value"));
}

TEST_CASE("Testing refract serialization for enum samples", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-enum-sample"));
}

TEST_CASE("Testing refract serialization primitive elements w/ members", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-primitive-with-members", false));
}

TEST_CASE("Testing refract serialization of nontyped array w/ samples", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-nontyped-array-sample"));
}

TEST_CASE("Testing refract with anonymous resource", "[refract][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-resource-anonymous"));
}

TEST_CASE("Testing refract resource w/ nested inheritance", "[refract][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-resource-nested-inheritance"));
}

TEST_CASE("Testing refract resource w/ nested mixin", "[refract][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-resource-nested-mixin"));
}

TEST_CASE("Testing refract resource w/ unresolved reference", "[refract][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-resource-unresolved-reference"));
}

TEST_CASE("Testing refract resource resolve basetype from other resource", "[refract][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-resource-resolve-basetype"));
}

TEST_CASE("Testing refract resource - mixin from primitive type", "[refract][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-resource-primitive-mixin"));
}

TEST_CASE("Testing refract - array typed content", "[refract]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-array-typed-content"));
}

TEST_CASE("Testing refract resource - nested type in array", "[refract][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson-resource-nested-member"));
}
