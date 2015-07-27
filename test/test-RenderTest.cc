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
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-simple-object"));
}

TEST_CASE("Testing render of a nested object", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-nested-object"));
}

TEST_CASE("Testing render of a complex object", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-complex-object"));
}

TEST_CASE("Testing render of a simple array", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-simple-array"));
}

TEST_CASE("Testing render of a property without sample in object", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-object-without-sample"));
}

TEST_CASE("Testing render of an object referencing object", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-object-ref-object"));
}

TEST_CASE("Testing render of an object nested under array", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-array-nested-object"));
}

TEST_CASE("Testing render of an array nested under array", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-array-nested-array"));
}

TEST_CASE("Testing render of an array referencing array", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-array-ref-array"));
}

TEST_CASE("Testing render of an array referencing object", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-array-ref-object"));
}

TEST_CASE("Testing render of an object mixin", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-object-mixin"));
}

TEST_CASE("Testing render of an primitive types w/ samples", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-primitive-samples"));
}

TEST_CASE("Testing render of an array mixin", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-array-mixin"));
}

TEST_CASE("Testing render of an array w/ samples", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-array-samples"));
}

TEST_CASE("Testing render of an object w/ samples", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-object-samples"));
}

TEST_CASE("Testing render of an inherited array w/ samples", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-inheritance-array-sample"));
}

TEST_CASE("Testing render of an inherited object w/ samples", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-inheritance-object-sample"));
}

TEST_CASE("Testing render of an array mixin w/ samples", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-mixin-array-sample"));
}

TEST_CASE("Testing render of an object mixin w/ samples", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/render-mixin-object-sample"));
}

