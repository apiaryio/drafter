//
//  test-RefractParseResultTest.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 26/09/15.
//  Copyright (c) 2015 Apiary. All rights reserved.
//

#include "draftertest.h"

using namespace draftertest;

TEST_CASE("Testing refract serialization for simple parse result", "[refract][parse_result]")
{
    REQUIRE(FixtureHelper::handleResultJSON("test/fixtures/parse-result/simple", true));
}

TEST_CASE("Testing refract serialization when it has an error", "[refract][parse_result]")
{
    REQUIRE(FixtureHelper::handleResultJSON("test/fixtures/parse-result/error"));
}

TEST_CASE("Testing refract serialization when it has a warning", "[refract][parse_result]")
{
    REQUIRE(FixtureHelper::handleResultJSON("test/fixtures/parse-result/warning", true));
}

TEST_CASE("Testing refract serialization when it has multiple warnings", "[refract][parse_result]")
{
    REQUIRE(FixtureHelper::handleResultJSON("test/fixtures/parse-result/warnings", true));
}

TEST_CASE("Testing refract serialization when it has error and warning", "[refract][parse_result]")
{
    REQUIRE(FixtureHelper::handleResultJSON("test/fixtures/parse-result/error-warning"));
}
