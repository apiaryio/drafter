//
//  test-RefractParseResultTest.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 26/09/15.
//  Copyright (c) 2015 Apiary. All rights reserved.
//

#include "draftertest.h"

using namespace draftertest;

TEST_REFRACT("parse-result", "simple");
TEST_REFRACT("parse-result", "warning");
TEST_REFRACT("parse-result", "warnings");
TEST_REFRACT("parse-result", "error-warning");
TEST_REFRACT("parse-result", "blueprint");
TEST_REFRACT("parse-result", "mson");
