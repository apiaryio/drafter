#include "test-drafter.h"

#include <string>

#include "snowcrash.h"

#include "sosJSON.h"
#include "SerializeAST.h"


bool HandleFixtureTest(const std::string& basepath) {
    ITFixtureFiles fixture = ITFixtureFiles(basepath);

    snowcrash::ParseResult<snowcrash::Blueprint> blueprint;
    int result = snowcrash::parse(fixture.get(".apib"), 0, blueprint);

    REQUIRE(result == snowcrash::Error::OK);

    std::stringstream outStream;
    sos::SerializeJSON serializer;

    serializer.process(drafter::WrapBlueprint(blueprint.node), outStream);
    outStream << "\n";

    return (outStream.str() == fixture.get(".json"));
}

TEST_CASE("Testing refract serialization for primitive types","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-primitives"));
}

TEST_CASE("Testing refract serialization for named types with inheritance","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-inheritance"));
}
