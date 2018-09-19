//
//  test-SyntaxIssuesTest.cc
//  drafter
//
//  Created by Jiří Kratochvíl on 19-09-2016.
//  Copyright (c) 2016 Apiary. All rights reserved.
//

#include "draftertest.h"

using namespace draftertest;

TEST_REFRACT("syntax", "issue-350");
TEST_REFRACT("syntax", "undisclosed-listitem");
TEST_REFRACT("syntax", "mixed-inheritance-and-mixin");
