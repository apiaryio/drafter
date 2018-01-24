//
//  test/refract/dsd/test-Null.cc
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "catch.hpp"

#include "refract/dsd/Null.h"

using namespace refract;
using namespace dsd;

TEST_CASE("`Null`'s default element name is `null`", "[Element][Null]")
{
    REQUIRE(std::string(Null::name) == "null");
}
