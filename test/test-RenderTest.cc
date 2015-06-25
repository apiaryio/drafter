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

TEST_CASE("Testing render of a property without sample in object", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render-object-without-sample"));
}

TEST_CASE("Testing render of an object referencing object", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render-object-ref-object"));
}

TEST_CASE("Testing render of an object nested under array", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render-array-nested-object"));
}

TEST_CASE("Testing render of an array nested under array", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render-array-nested-array"));
}

TEST_CASE("Testing render of an array referencing array", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render-array-ref-array"));
}

TEST_CASE("Testing render of an array referencing object", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render-array-ref-object"));
}

TEST_CASE("Testing render of an object mixin", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render-object-mixin"));
}

TEST_CASE("Testing render of an array mixin", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render-array-mixin"));
}
