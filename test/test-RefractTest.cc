#include "test-drafter.h"

#include <string>

#include "snowcrash.h"

#include "sosJSON.h"
#include "SerializeAST.h"


bool HandleFixtureTest(const std::string& basepath, bool mustBeOk = true) 
{
    ITFixtureFiles fixture = ITFixtureFiles(basepath);

    snowcrash::ParseResult<snowcrash::Blueprint> blueprint;
    int result = snowcrash::parse(fixture.get(".apib"), 0, blueprint);

    if (mustBeOk) {
        REQUIRE(result == snowcrash::Error::OK);
    }

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

TEST_CASE("Testing refract serialization for array[type]","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-typed-array"));
}

TEST_CASE("Testing refract serialization for typed object","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-typed-object"));
}

TEST_CASE("Testing refract serialization for nontypped object","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-nontyped-object"));
}

TEST_CASE("Testing refract serialization for enums","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-enum"));
}

TEST_CASE("Testing refract serialization for oneof","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-oneof"));
}

TEST_CASE("Testing refract serialization for mixin","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-mixin"));
}

TEST_CASE("Testing refract serialization for primitive with samples","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-string-sample"));
}

TEST_CASE("Testing refract serialization for typed array samples","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-typed-array-sample"));
}

TEST_CASE("Testing refract serialization for 'One Of' with grouped elements","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-group"));
}

TEST_CASE("Testing refract serialization array with empty sample","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-empty-sample"));
}

TEST_CASE("Testing refract serialization with inner inheritance","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-inner-inheritance"));
}

TEST_CASE("Testing refract serialization oneof w/ sample","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-oneof-sample"));
}

TEST_CASE("Testing refract serialization with multiline comments","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-multiline-description"));
}

TEST_CASE("Testing refract serialization for primitive variables","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-primitive-variables"));
}

TEST_CASE("Testing refract serialization for NamedTypes w/ type specification","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-named-with-types"));
}

TEST_CASE("Testing refract serialization for nontypes array","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-nontyped-array"));
}

TEST_CASE("Testing refract serialization for wrong number value","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-number-wrong-value"));
}

TEST_CASE("Testing refract serialization for enum samples","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-enum-sample"));
}

TEST_CASE("Testing refract serialization primitive elements w/ members","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-primitive-with-members", false));
}
#if 0

TEST_CASE("Testing refract serialization of nontyped array w/ samples","[refract]")
{
    REQUIRE(HandleFixtureTest("test/fixtures/mson-nontyped-array-sample"));
}
#endif
