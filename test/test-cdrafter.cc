#include "test-drafter.h"

#include "cdrafter.h"

#include <string.h>

TEST_CASE("c-interface parses blueprint correctly","[c-interface]")
{
    ITFixtureFiles fixture = ITFixtureFiles("test/fixtures/annotations-with-warning");

    std::string source = fixture.get(".apib");

    char *result = NULL;

    int ret = drafter_c_parse(source.c_str(), 0, &result);

    REQUIRE(ret == 0);

    REQUIRE(result != NULL);
    REQUIRE(strcmp(result, fixture.get(".result.json").c_str()) == 0);

    free(result);
}

TEST_CASE("c-interface check result, without memory alloc","[c-interface]")
{
    ITFixtureFiles fixture = ITFixtureFiles("test/fixtures/annotations-with-warning");

    std::string source = fixture.get(".apib");

    char *ast = NULL, *report = NULL;

    int ret = drafter_c_parse(source.c_str(), 0, NULL);

    REQUIRE(ret == 0);
}

