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
