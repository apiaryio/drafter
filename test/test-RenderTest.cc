//
//  test-RenderTest.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 17/6/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "draftertest.h"

using namespace draftertest;

TEST_AST("render", "simple-object");
TEST_AST("render", "content-type");
TEST_AST("render", "nested-object");
TEST_AST("render", "complex-object");
TEST_AST("render", "simple-array");
TEST_AST("render", "object-without-sample");
TEST_AST("render", "object-ref-object");
TEST_AST("render", "array-nested-object");
TEST_AST("render", "array-nested-array");
TEST_AST("render", "array-ref-array");
TEST_AST("render", "array-ref-object");
TEST_AST("render", "object-mixin");
TEST_AST("render", "primitive-samples");
TEST_AST("render", "array-mixin");
TEST_AST("render", "array-samples");
TEST_AST("render", "object-samples");
TEST_AST("render", "inheritance-array-sample");
TEST_AST("render", "inheritance-object-sample");
TEST_AST("render", "mixin-array-sample");
TEST_AST("render", "mixin-object-sample");
TEST_AST("render", "nullable");
TEST_AST("render", "override");
TEST_AST("render", "action-request-attributes");
