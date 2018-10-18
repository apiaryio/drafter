#include "catch.hpp"

#include "../src/SourceMapUtils.h"

using namespace drafter;

/*
Map:
'^' - newline
'_' - other character

01234567
   
____^     0, 5
____^     5, 5
^        10, 1
___^     11, 4
__^      15, 3 
_______^ 18, 8
*/

static const NewLinesIndex nlIndex = { 0, 5, 10, 11, 15, 18, 26 };

TEST_CASE("AnnotationPosition is initialized to zero", "[sourcemap utils]")
{
    AnnotationPosition a;

    REQUIRE(a.fromLine == 0);
    REQUIRE(a.fromColumn == 0);
    REQUIRE(a.toLine == 0);
    REQUIRE(a.toColumn == 0);
}

TEST_CASE("GetLineFromMap for position [0,3]", "[sourcemap utils]")
{
    AnnotationPosition a;
    mdp::Range r = {0,3};

    GetLineFromMap(nlIndex, r, a);

    REQUIRE(a.fromLine == 1);
    REQUIRE(a.fromColumn == 0);
    REQUIRE(a.toLine == 1);
    REQUIRE(a.toColumn == 2);
}

TEST_CASE("GetLineFromMap for position - 1st line w/o NL", "[sourcemap utils]")
{
    AnnotationPosition a;
    mdp::Range r = {0,4};

    GetLineFromMap(nlIndex, r, a);

    REQUIRE(a.fromLine == 1);
    REQUIRE(a.fromColumn == 0);
    REQUIRE(a.toLine == 1);
    REQUIRE(a.toColumn == 3);
}

TEST_CASE("GetLineFromMap for position - 1st line w/ NL", "[sourcemap utils]")
{
    AnnotationPosition a;
    mdp::Range r = {0,5};

    GetLineFromMap(nlIndex, r, a);

    REQUIRE(a.fromLine == 1);
    REQUIRE(a.fromColumn == 0);
    REQUIRE(a.toLine == 1);
    REQUIRE(a.toColumn == 4);
}

TEST_CASE("GetLineFromMap for position - 2nd line w/ NL", "[sourcemap utils]")
{
    AnnotationPosition a;
    mdp::Range r = {5,5};

    GetLineFromMap(nlIndex, r, a);

    REQUIRE(a.fromLine == 2);
    REQUIRE(a.fromColumn == 0);
    REQUIRE(a.toLine == 2);
    REQUIRE(a.toColumn == 4);
}

TEST_CASE("GetLineFromMap for position - last line w/ NL", "[sourcemap utils]")
{
    AnnotationPosition a;
    mdp::Range r = {18,8};

    GetLineFromMap(nlIndex, r, a);

    REQUIRE(a.fromLine == 6);
    REQUIRE(a.fromColumn == 0);
    REQUIRE(a.toLine == 6);
    REQUIRE(a.toColumn == 7);
}

TEST_CASE("GetLineFromMap for position - 3rd empty line - just NL", "[sourcemap utils]")
{
    AnnotationPosition a;
    mdp::Range r = {10,1};

    GetLineFromMap(nlIndex, r, a);

    REQUIRE(a.fromLine == 3);
    REQUIRE(a.fromColumn == 0);
    REQUIRE(a.toLine == 3);
    REQUIRE(a.toColumn == 0);
}

TEST_CASE("GetLineFromMap for position - 1st two lines", "[sourcemap utils]")
{
    AnnotationPosition a;
    mdp::Range r = {0,10};

    GetLineFromMap(nlIndex, r, a);

    REQUIRE(a.fromLine == 1);
    REQUIRE(a.fromColumn == 0);
    REQUIRE(a.toLine == 2);
    REQUIRE(a.toColumn == 4);
}

TEST_CASE("GetLineFromMap for position - lines 1-3", "[sourcemap utils]")
{
    AnnotationPosition a;
    mdp::Range r = {5,10};

    GetLineFromMap(nlIndex, r, a);

    REQUIRE(a.fromLine == 2);
    REQUIRE(a.fromColumn == 0);
    REQUIRE(a.toLine == 4);
    REQUIRE(a.toColumn == 3);
}

TEST_CASE("GetLineFromMap for position - all lines", "[sourcemap utils]")
{
    AnnotationPosition a;
    mdp::Range r = {0,26};

    GetLineFromMap(nlIndex, r, a);

    REQUIRE(a.fromLine == 1);
    REQUIRE(a.fromColumn == 0);
    REQUIRE(a.toLine == 6);
    REQUIRE(a.toColumn == 7);
}
