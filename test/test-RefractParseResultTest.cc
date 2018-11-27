//
//  test-RefractParseResultTest.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 26/09/15.
//  Copyright (c) 2015 Apiary. All rights reserved.
//

#include "draftertest.h"

using namespace draftertest;

#if ! defined (_MSC_VER) || ! defined (_DEBUG)
TEST_REFRACT("parse-result", "simple");
TEST_REFRACT("parse-result", "warning");
TEST_REFRACT("parse-result", "warnings");
#endif

TEST_REFRACT("parse-result", "error");

TEST_REFRACT("parse-result", "error-warning");
TEST_REFRACT("parse-result", "blueprint");
TEST_REFRACT("parse-result", "mson");

TEST_REFRACT("mson", "type-attributes");

#if ! defined (_MSC_VER) || ! defined (_DEBUG)
TEST_REFRACT("mson", "type-attributes-payload");
#endif
