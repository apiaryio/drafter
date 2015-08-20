#include "draftertest.h"
#include "SerializeResult.h"

using namespace draftertest;

TEST_CASE("Integration test for parse result serialization", "[result]")
{
    ITFixtureFiles fixture = ITFixtureFiles("test/fixtures/blueprint");

    snowcrash::ParseResult<snowcrash::Blueprint> blueprint;
    int result = snowcrash::parse(fixture.get(".apib"), snowcrash::ExportSourcemapOption, blueprint);

    REQUIRE(result == snowcrash::Error::OK);

    std::stringstream outStream;
    sos::SerializeJSON serializer;

    serializer.process(drafter::WrapResult(blueprint, snowcrash::ExportSourcemapOption, drafter::NormalASTType), outStream);
    outStream << "\n";

    REQUIRE(outStream.str() == fixture.get(".result.sourcemap.json"));
}

TEST_CASE("Integration test for parse result refract serialization without sourcemap", "[result]")
{
    ITFixtureFiles fixture = ITFixtureFiles("test/fixtures/blueprint");

    snowcrash::ParseResult<snowcrash::Blueprint> blueprint;
    int result = snowcrash::parse(fixture.get(".apib"), snowcrash::ExportSourcemapOption, blueprint);

    REQUIRE(result == snowcrash::Error::OK);

    std::stringstream outStream;
    sos::SerializeJSON serializer;

    serializer.process(drafter::WrapResult(blueprint, 0, drafter::RefractASTType), outStream);
    outStream << "\n";

    REQUIRE(outStream.str() == fixture.get(".refract.result.json"));
}

