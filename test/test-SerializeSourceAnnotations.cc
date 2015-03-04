#include "test-drafter.h"

#include <string>

#include "snowcrash.h"

#include "sosJSON.h"
#include "SerializeSourceAnnotations.h"


TEST_CASE("integration test for result parse serialization","[result serialization]")
{
    it_fixture_files fixture = it_fixture_files("test/fixtures/annotations-with-warning");

    snowcrash::ParseResult<snowcrash::Blueprint> blueprint;
    int result = snowcrash::parse(fixture.apib(), snowcrash::ExportSourcemapOption, blueprint);

    REQUIRE(result == snowcrash::Error::OK);

    std::stringstream outStream;
    sos::SerializeJSON serializer;

    serializer.process(drafter::WrapSourceAnnotations(blueprint.report, blueprint.sourceMap), outStream);
    outStream << "\n";

    REQUIRE(outStream.str() == fixture.json());
}
