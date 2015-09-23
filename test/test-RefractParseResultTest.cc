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
    REQUIRE(FixtureHelper::handleResultJSON("test/fixtures/parse-result/simple"));
}
