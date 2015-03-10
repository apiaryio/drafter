#include "test-drafter.h"

#include <string>

#include "snowcrash.h"

#include "sosJSON.h"
#include "SerializeResult.h"



TEST_CASE("integration test for result parse serialization","[result serialization]")
{
    ITFixtureFiles fixture = ITFixtureFiles("test/fixtures/annotations-with-warning");

    snowcrash::ParseResult<snowcrash::Blueprint> blueprint;
    int result = snowcrash::parse(fixture.get(".apib"), snowcrash::ExportSourcemapOption, blueprint);

    REQUIRE(result == snowcrash::Error::OK);

    std::stringstream outStream;
    sos::SerializeJSON serializer;

    serializer.process(drafter::WrapResult(blueprint, snowcrash::ExportSourcemapOption), outStream);
    outStream << "\n";

    REQUIRE(outStream.str() == fixture.get(".result-with-sourcemap.json"));
}
