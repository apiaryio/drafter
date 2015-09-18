#include "draftertest.h"

using namespace draftertest;

TEST_CASE("Testing refract serialization for primitive types", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/primitives", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization for named types with inheritance", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/inheritance", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization for array[type]", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/typed-array", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization for typed object", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/typed-object", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization for nontypped object", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/nontyped-object", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization for enums", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/enum", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization for oneof", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/oneof", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization for mixin", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mixin", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization for nonexistent mixin", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/mixin-nonexistent", drafter::NormalASTType, true, false));
}

TEST_CASE("Testing refract serialization for primitive with samples", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/string-sample", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization for typed array samples", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/typed-array-sample", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization for 'One Of' with grouped elements", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/group", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization array with empty sample", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/empty-sample", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization with inner inheritance", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/inner-inheritance", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization oneof w/ sample", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/oneof-sample", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization with multiline comments", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/multiline-description", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization for primitive variables", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/primitive-variables", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization for NamedTypes w/ type specification", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/named-with-types", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization for nontypes array", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/nontyped-array", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization for wrong number value", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/number-wrong-value", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization for enum samples", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/enum-sample", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract serialization primitive elements w/ members", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/primitive-with-members", drafter::NormalASTType, true, false));
}

TEST_CASE("Testing refract serialization of nontyped array w/ samples", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/nontyped-array-sample", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract with anonymous resource", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-anonymous", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract resource w/ nested inheritance", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-nested-inheritance", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract resource w/ nested mixin", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-nested-mixin", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract resource w/ unresolved reference", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-unresolved-reference", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract resource resolve basetype from other resource", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-resolve-basetype", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract resource - mixin from primitive type", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-primitive-mixin", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract - array typed content", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/array-typed-content", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract resource - nested type in array", "[refract][mson][drafter.js]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/resource-nested-member", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract named structure - array samples and default", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/array-sample", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract named structure - object samples and default", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/object-sample", drafter::NormalASTType, true));
}

TEST_CASE("Testing description on members of enum|array", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/enum-members-description", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract array containing reference", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/array-reference", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract reference getting overridden", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/reference-override", drafter::NormalASTType, true));
}

TEST_CASE("Testing refract enum count of different variants value/samples", "[refract][mson]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/mson/enum-variants", drafter::NormalASTType, true));
}
