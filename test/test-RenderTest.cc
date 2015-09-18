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
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/simple-object"));
}

TEST_CASE("Testing render of a simple object where content-type value has extra qualifiers", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/content-type"));
}

TEST_CASE("Testing render of a nested object", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/nested-object"));
}

TEST_CASE("Testing render of a complex object", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/complex-object"));
}

TEST_CASE("Testing render of a simple array", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/simple-array"));
}

TEST_CASE("Testing render of a property without sample in object", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/object-without-sample"));
}

TEST_CASE("Testing render of an object referencing object", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/object-ref-object"));
}

TEST_CASE("Testing render of an object nested under array", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/array-nested-object"));
}

TEST_CASE("Testing render of an array nested under array", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/array-nested-array"));
}

TEST_CASE("Testing render of an array referencing array", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/array-ref-array"));
}

TEST_CASE("Testing render of an array referencing object", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/array-ref-object"));
}

TEST_CASE("Testing render of an object mixin", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/object-mixin"));
}

TEST_CASE("Testing render of an primitive types w/ samples", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/primitive-samples"));
}

TEST_CASE("Testing render of an array mixin", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/array-mixin"));
}

TEST_CASE("Testing render of an array w/ samples", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/array-samples"));
}

TEST_CASE("Testing render of an object w/ samples", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/object-samples"));
}

TEST_CASE("Testing render of an inherited array w/ samples", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/inheritance-array-sample"));
}

TEST_CASE("Testing render of an inherited object w/ samples", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/inheritance-object-sample"));
}

TEST_CASE("Testing render of an array mixin w/ samples", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/mixin-array-sample"));
}

TEST_CASE("Testing render of an object mixin w/ samples", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/mixin-object-sample"));
}

TEST_CASE("Testing render of primitive types w/ nullable", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/nullable"));
}

TEST_CASE("Testing render when object is overridden", "[render]")
{
    REQUIRE(FixtureHelper::handleBlueprintJSON("test/fixtures/render/override"));
}
