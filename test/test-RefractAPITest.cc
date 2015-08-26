#include "draftertest.h"

using namespace draftertest;

TEST_CASE("Testing refract serialization for API description", "[refract][api]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/api/description", drafter::RefractASTType));
}

TEST_CASE("Testing refract serialization for metadata", "[refract][api]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/api/metadata", drafter::RefractASTType));
}

TEST_CASE("Testing refract serialization for resource group", "[refract][api]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/api/resource-group", drafter::RefractASTType));
}

TEST_CASE("Testing refract serialization for data structure", "[refract][api]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/api/data-structure", drafter::RefractASTType));
}

TEST_CASE("Testing refract serialization for resource", "[refract][api]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/api/resource", drafter::RefractASTType));
}

TEST_CASE("Testing refract serialization for action", "[refract][api]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/api/action", drafter::RefractASTType));
}

TEST_CASE("Testing refract serialization for advanced action", "[refract][api]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/api/advanced-action", drafter::RefractASTType));
}

TEST_CASE("Testing refract serialization for relation", "[refract][api]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/api/relation", drafter::RefractASTType));
}

TEST_CASE("Testing refract serialization for resource attributes", "[refract][api]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/api/resource-attributes", drafter::RefractASTType));
}

TEST_CASE("Testing refract serialization for action attributes", "[refract][api]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/api/action-attributes", drafter::RefractASTType));
}

TEST_CASE("Testing refract serialization for payload attributes", "[refract][api]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/api/payload-attributes", drafter::RefractASTType));
}

TEST_CASE("Testing refract serialization for transaction", "[refract][api]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/api/transaction", drafter::RefractASTType));
}

TEST_CASE("Testing refract serialization for headers", "[refract][api]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/api/headers", drafter::RefractASTType));
}

TEST_CASE("Testing refract serialization for resource parameters", "[refract][api]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/api/resource-parameters", drafter::RefractASTType));
}

TEST_CASE("Testing refract serialization for action parameters", "[refract][api]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/api/action-parameters", drafter::RefractASTType));
}

TEST_CASE("Testing refract serialization for request only action", "[refract][api]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/api/request-only", drafter::RefractASTType));
}

TEST_CASE("Testing refract serialization for rendered asset", "[refract][api]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/api/asset", drafter::RefractASTType));
}

TEST_CASE("Testing refract serialization for response attributes with references", "[refract][api]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/api/attributes-references", drafter::RefractASTType));
}
