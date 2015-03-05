#include "test-drafter.h"

#include "cdrafter.h"

#include <string.h>


TEST_CASE("c-interface parses blueprint correctly","[c-interface]")
{
    it_fixture_files fixture = it_fixture_files("test/fixtures/annotations-with-warning");

    std::string source = fixture.get(".apib");

    char *ast = NULL, *report = NULL;

    int ret = drafter_c_parse(source.c_str(), 0, &report, &ast);

    REQUIRE(ret == 0);

    REQUIRE(report != NULL);
    REQUIRE(ast != NULL);

    REQUIRE(strcmp(ast, fixture.get(".ast.json").c_str()) == 0);
    REQUIRE(strcmp(report, fixture.get(".result.json").c_str()) == 0);

    free(ast);
    free(report);
}

TEST_CASE("just check result, no memory alloc","[c-interface]")
{
    it_fixture_files fixture = it_fixture_files("test/fixtures/annotations-with-warning");

    std::string source = fixture.get(".apib");

    char *ast = NULL, *report = NULL;

    int ret = drafter_c_parse(source.c_str(), 0, NULL, NULL);

    REQUIRE(ret == 0);
}

