#include "draftertest.h"

using namespace draftertest;

TEST_CASE("Testing refract serialization for primitive types", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/primitives"));
}

TEST_CASE("Testing refract serialization for named types with inheritance", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/inheritance"));
}

TEST_CASE("Testing refract serialization for array[type]", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/typed-array"));
}

TEST_CASE("Testing refract serialization for typed object", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/typed-object"));
}

TEST_CASE("Testing refract serialization for nontypped object", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/nontyped-object"));
}

TEST_CASE("Testing refract serialization for enums", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/enum"));
}

TEST_CASE("Testing refract serialization for oneof", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/oneof"));
}

TEST_CASE("Testing refract serialization for mixin", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mixin"));
}

TEST_CASE("Testing refract serialization for nonexistent mixin", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mixin-nonexistent", drafter::NormalASTType, false));
}

TEST_CASE("Testing refract serialization for primitive with samples", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/string-sample"));
}

TEST_CASE("Testing refract serialization for typed array samples", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/typed-array-sample"));
}

TEST_CASE("Testing refract serialization for 'One Of' with grouped elements", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/group"));
}

TEST_CASE("Testing refract serialization array with empty sample", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/empty-sample"));
}

TEST_CASE("Testing refract serialization with inner inheritance", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/inner-inheritance"));
}

TEST_CASE("Testing refract serialization oneof w/ sample", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/oneof-sample"));
}

TEST_CASE("Testing refract serialization with multiline comments", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/multiline-description"));
}

TEST_CASE("Testing refract serialization for primitive variables", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/primitive-variables"));
}

TEST_CASE("Testing refract serialization for NamedTypes w/ type specification", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/named-with-types"));
}

TEST_CASE("Testing refract serialization for nontypes array", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/nontyped-array"));
}

TEST_CASE("Testing refract serialization for wrong number value", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/number-wrong-value"));
}

TEST_CASE("Testing refract serialization for enum samples", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/enum-sample"));
}

TEST_CASE("Testing refract serialization primitive elements w/ members", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/primitive-with-members", drafter::NormalASTType, false));
}

TEST_CASE("Testing refract serialization of nontyped array w/ samples", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/nontyped-array-sample"));
}

TEST_CASE("Testing refract with anonymous resource", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-anonymous"));
}

TEST_CASE("Testing refract resource w/ nested inheritance", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-nested-inheritance"));
}

TEST_CASE("Testing refract resource w/ nested mixin", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-nested-mixin"));
}

TEST_CASE("Testing refract resource w/ unresolved reference", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-unresolved-reference"));
}

TEST_CASE("Testing refract resource resolve basetype from other resource", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-resolve-basetype"));
}

TEST_CASE("Testing refract resource - mixin from primitive type", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-primitive-mixin"));
}

TEST_CASE("Testing refract - array typed content", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/array-typed-content"));
}

TEST_CASE("Testing refract resource - nested type in array", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-nested-member"));
}

TEST_CASE("Testing refract named structure - array samples and default", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/array-sample"));
}

TEST_CASE("Testing refract named structure - object samples and default", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/object-sample"));
}
