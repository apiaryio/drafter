//
//  test-SerializeSourceMapTest.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 14/04/16.
//  Copyright (c) 2016 Apiary. All rights reserved.
//

#include "draftertest.h"

using namespace draftertest;

TEST_AST_SOURCE_MAP("parse-result", "simple");
TEST_AST_SOURCE_MAP("parse-result", "warning");
TEST_AST_SOURCE_MAP("parse-result", "warnings");
TEST_AST_SOURCE_MAP("parse-result", "error-warning");
TEST_AST_SOURCE_MAP("parse-result", "blueprint");
TEST_AST_SOURCE_MAP("parse-result", "mson");