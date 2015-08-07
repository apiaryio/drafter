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
