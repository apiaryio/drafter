#include "draftertest.h"
#include "cdrafter.h"

using namespace draftertest;

TEST_CASE("C-interface parse blueprint", "[c-interface]")
{
    ITFixtureFiles fixture = ITFixtureFiles("test/fixtures/blueprint");

    std::string source = fixture.get(".apib");

    char *result = NULL;

    int ret = drafter_c_parse(source.c_str(), 0, DRAFTER_NORMAL_AST_TYPE, &result);

    REQUIRE(ret == 0);

    REQUIRE(result);
    REQUIRE(strcmp(result, fixture.get(".result.json").c_str()) == 0);

    free(result);
}

TEST_CASE("C-interface parse blueprint with sourceMap", "[c-interface]")
{
    ITFixtureFiles fixture = ITFixtureFiles("test/fixtures/blueprint");

    std::string source = fixture.get(".apib");

    char *result = NULL;

    int ret = drafter_c_parse(source.c_str(), SC_EXPORT_SORUCEMAP_OPTION, DRAFTER_NORMAL_AST_TYPE, &result);

    REQUIRE(ret == 0);

    REQUIRE(result);
    REQUIRE(strcmp(result, fixture.get(".result.sourcemap.json").c_str()) == 0);

    free(result);
}

TEST_CASE("C-interface check result, without memory alloc", "[c-interface]")
{
    ITFixtureFiles fixture = ITFixtureFiles("test/fixtures/blueprint");

    std::string source = fixture.get(".apib");

    char *ast = NULL, *report = NULL;

    int ret = drafter_c_parse(source.c_str(), 0, DRAFTER_NORMAL_AST_TYPE, NULL);

    REQUIRE(ret == 0);
}

TEST_CASE("C-interface parse blueprint for refract", "[c-interface]")
{
    ITFixtureFiles fixture = ITFixtureFiles("test/fixtures/blueprint");

    std::string source = fixture.get(".apib");

    char *result = NULL;

    int ret = drafter_c_parse(source.c_str(), 0, DRAFTER_REFRACT_AST_TYPE, &result);

    REQUIRE(ret == 0);

    REQUIRE(result);
    REQUIRE(strcmp(result, fixture.get(".refract.result.json").c_str()) == 0);

    free(result);
}

