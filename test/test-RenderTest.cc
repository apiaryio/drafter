//
//  test-RenderTest.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 17/6/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "draftertest.h"

using namespace draftertest;

TEST_CASE("Testing render of a simple object", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render-simple-object"));
}

TEST_CASE("Testing render of a nested object", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render-nested-object"));
}

TEST_CASE("Testing render of a complex object", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render-complex-object"));
}

TEST_CASE("Testing render of a simple array", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render-simple-array"));
}
