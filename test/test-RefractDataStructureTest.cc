#include "draftertest.h"

using namespace draftertest;

TEST_CASE("Testing refract serialization for primitive types", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-primitives"));
}

TEST_CASE("Testing refract serialization for named types with inheritance", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-inheritance"));
}

TEST_CASE("Testing refract serialization for array[type]", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-typed-array"));
}

TEST_CASE("Testing refract serialization for typed object", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-typed-object"));
}

TEST_CASE("Testing refract serialization for nontypped object", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-nontyped-object"));
}

TEST_CASE("Testing refract serialization for enums", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-enum"));
}

TEST_CASE("Testing refract serialization for oneof", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-oneof"));
}

TEST_CASE("Testing refract serialization for mixin", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-mixin"));
}

TEST_CASE("Testing refract serialization for nonexistent mixin", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-mixin-nonexistent", drafter::NormalASTType, false));
}

TEST_CASE("Testing refract serialization for primitive with samples", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-string-sample"));
}

TEST_CASE("Testing refract serialization for typed array samples", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-typed-array-sample"));
}

TEST_CASE("Testing refract serialization for 'One Of' with grouped elements", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-group"));
}

TEST_CASE("Testing refract serialization array with empty sample", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-empty-sample"));
}

TEST_CASE("Testing refract serialization with inner inheritance", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-inner-inheritance"));
}

TEST_CASE("Testing refract serialization oneof w/ sample", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-oneof-sample"));
}

TEST_CASE("Testing refract serialization with multiline comments", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-multiline-description"));
}

TEST_CASE("Testing refract serialization for primitive variables", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-primitive-variables"));
}

TEST_CASE("Testing refract serialization for NamedTypes w/ type specification", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-named-with-types"));
}

TEST_CASE("Testing refract serialization for nontypes array", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-nontyped-array"));
}

TEST_CASE("Testing refract serialization for wrong number value", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-number-wrong-value"));
}

TEST_CASE("Testing refract serialization for enum samples", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-enum-sample"));
}

TEST_CASE("Testing refract serialization primitive elements w/ members", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-primitive-with-members", drafter::NormalASTType, false));
}

TEST_CASE("Testing refract serialization of nontyped array w/ samples", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-nontyped-array-sample"));
}

TEST_CASE("Testing refract with anonymous resource", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-resource-anonymous"));
}

TEST_CASE("Testing refract resource w/ nested inheritance", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-resource-nested-inheritance"));
}

TEST_CASE("Testing refract resource w/ nested mixin", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-resource-nested-mixin"));
}

TEST_CASE("Testing refract resource w/ unresolved reference", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-resource-unresolved-reference"));
}

TEST_CASE("Testing refract resource resolve basetype from other resource", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-resource-resolve-basetype"));
}

TEST_CASE("Testing refract resource - mixin from primitive type", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-resource-primitive-mixin"));
}

TEST_CASE("Testing refract - array typed content", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-array-typed-content"));
}

TEST_CASE("Testing refract resource - nested type in array", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-resource-nested-member"));
}

TEST_CASE("Testing refract named structure - array samples and default", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-array-sample"));
}

TEST_CASE("Testing refract named structure - object samples and default", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mson-object-sample"));
}
