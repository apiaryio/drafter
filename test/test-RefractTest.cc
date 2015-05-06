#include "test-drafter.h"

#include <string>

#include "snowcrash.h"

#include "sosJSON.h"
#include "SerializeAST.h"

TEST_CASE("Testing refract serialization","[refract]")
{
    ITFixtureFiles fixture = ITFixtureFiles("test/fixtures/mson1");

    snowcrash::ParseResult<snowcrash::Blueprint> blueprint;
    int result = snowcrash::parse(fixture.get(".apib"), 0, blueprint);

    REQUIRE(result == snowcrash::Error::OK);

    std::stringstream outStream;
    sos::SerializeJSON serializer;

    serializer.process(drafter::WrapBlueprint(blueprint.node), outStream);
    outStream << "\n";

    REQUIRE(outStream.str() == fixture.get(".json"));
}
