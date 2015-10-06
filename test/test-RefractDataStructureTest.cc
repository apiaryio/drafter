#include "draftertest.h"

using namespace draftertest;

static drafter::WrappingContext MSONTestContext(drafter::NormalASTType, true, false);

TEST_CASE("Testing refract serialization for primitive types", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/primitives", MSONTestContext));
}

TEST_CASE("Testing refract serialization for named types with inheritance", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/inheritance", MSONTestContext));
}

TEST_CASE("Testing refract serialization for array[type]", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/typed-array", MSONTestContext));
}

TEST_CASE("Testing refract serialization for typed object", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/typed-object", MSONTestContext));
}

TEST_CASE("Testing refract serialization for nontypped object", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/nontyped-object", MSONTestContext));
}

TEST_CASE("Testing refract serialization for enums", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/enum", MSONTestContext));
}

TEST_CASE("Testing refract serialization for oneof", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/oneof", MSONTestContext));
}

TEST_CASE("Testing refract serialization for mixin", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mixin", MSONTestContext));
}

TEST_CASE("Testing refract serialization for nonexistent mixin", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mixin-nonexistent", MSONTestContext, false));
}

TEST_CASE("Testing refract serialization for primitive with samples", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/string-sample", MSONTestContext));
}

TEST_CASE("Testing refract serialization for typed array samples", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/typed-array-sample", MSONTestContext));
}

TEST_CASE("Testing refract serialization for 'One Of' with grouped elements", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/group", MSONTestContext));
}

TEST_CASE("Testing refract serialization array with empty sample", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/empty-sample", MSONTestContext));
}

TEST_CASE("Testing refract serialization with inner inheritance", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/inner-inheritance", MSONTestContext));
}

TEST_CASE("Testing refract serialization oneof w/ sample", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/oneof-sample", MSONTestContext));
}

TEST_CASE("Testing refract serialization with multiline comments", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/multiline-description", MSONTestContext));
}

TEST_CASE("Testing refract serialization for primitive variables", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/primitive-variables", MSONTestContext));
}

TEST_CASE("Testing refract serialization for NamedTypes w/ type specification", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/named-with-types", MSONTestContext));
}

TEST_CASE("Testing refract serialization for nontypes array", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/nontyped-array", MSONTestContext));
}

TEST_CASE("Testing refract serialization for wrong number value", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/number-wrong-value", MSONTestContext));
}

TEST_CASE("Testing refract serialization for enum samples", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/enum-sample", MSONTestContext));
}

TEST_CASE("Testing refract serialization primitive elements w/ members", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/primitive-with-members", MSONTestContext, false));
}

TEST_CASE("Testing refract serialization of nontyped array w/ samples", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/nontyped-array-sample", MSONTestContext));
}

TEST_CASE("Testing refract with anonymous resource", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-anonymous", MSONTestContext));
}

TEST_CASE("Testing refract resource w/ nested inheritance", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-nested-inheritance", MSONTestContext));
}

TEST_CASE("Testing refract resource w/ nested mixin", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-nested-mixin", MSONTestContext));
}

TEST_CASE("Testing refract resource w/ unresolved reference", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-unresolved-reference", MSONTestContext));
}

TEST_CASE("Testing refract resource resolve basetype from other resource", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-resolve-basetype", MSONTestContext));
}

TEST_CASE("Testing refract resource - mixin from primitive type", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-primitive-mixin", MSONTestContext));
}

TEST_CASE("Testing refract - array typed content", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/array-typed-content", MSONTestContext));
}

TEST_CASE("Testing refract resource - nested type in array", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-nested-member", MSONTestContext));
}

TEST_CASE("Testing refract named structure - array samples and default", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/array-sample", MSONTestContext));
}

TEST_CASE("Testing refract named structure - object samples and default", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/object-sample", MSONTestContext));
}

TEST_CASE("Testing description on members of enum|array", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/enum-members-description", MSONTestContext));
}

TEST_CASE("Testing refract array containing reference", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/array-reference", MSONTestContext));
}

TEST_CASE("Testing refract reference getting overridden", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/reference-override", MSONTestContext));
}

TEST_CASE("Testing refract enum count of different variants value/samples", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/enum-variants", MSONTestContext));
}
