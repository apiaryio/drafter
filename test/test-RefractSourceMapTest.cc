#include "draftertest.h"

using namespace draftertest;

static drafter::WrappingContext SourceMapTestContext(drafter::RefractASTType, false, true);

TEST_CASE("Testing refract sourcemaps serialization for API description", "[refract][sourcemap]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/sourcemap/full", SourceMapTestContext));
}

