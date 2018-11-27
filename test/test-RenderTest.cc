//
//  test-RenderTest.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 17/6/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "draftertest.h"

using namespace draftertest;

#if ! defined (_MSC_VER) || ! defined (_DEBUG)
TEST_REFRACT("render", "simple-object");
TEST_REFRACT("render", "content-type");
TEST_REFRACT("render", "nested-object");
TEST_REFRACT("render", "complex-object");
TEST_REFRACT("render", "simple-array");
TEST_REFRACT("render", "object-without-sample");
#endif

TEST_REFRACT("render", "object-ref-object");

#if ! defined (_MSC_VER) || ! defined (_DEBUG)
TEST_REFRACT("render", "array-nested-object");
TEST_REFRACT("render", "array-nested-array");
#endif

TEST_REFRACT("render", "array-ref-array");
TEST_REFRACT("render", "array-ref-object");
TEST_REFRACT("render", "object-mixin");

#if ! defined (_MSC_VER) || ! defined (_DEBUG)
TEST_REFRACT("render", "primitive-samples");
#endif

TEST_REFRACT("render", "array-mixin");

#if ! defined (_MSC_VER) || ! defined (_DEBUG)
TEST_REFRACT("render", "array-samples");
TEST_REFRACT("render", "object-samples");
#endif

TEST_REFRACT("render", "inheritance-array-sample");
TEST_REFRACT("render", "inheritance-object-sample");
TEST_REFRACT("render", "mixin-array-sample");
TEST_REFRACT("render", "mixin-object-sample");

#if ! defined (_MSC_VER) || ! defined (_DEBUG)
TEST_REFRACT("render", "nullable");
#endif

TEST_REFRACT("render", "override");
TEST_REFRACT("render", "action-request-attributes");

#if ! defined (_MSC_VER) || ! defined (_DEBUG)
TEST_REFRACT("render", "object-array-string");
TEST_REFRACT("render", "enum-default");
TEST_REFRACT("render", "enum-default-multiple");
TEST_REFRACT("render", "enum-sample");
#endif

TEST_REFRACT("render", "array-fixed-values");

TEST_REFRACT("render", "issue-246");
TEST_REFRACT("render", "issue-318");
TEST_REFRACT("render", "issue-312");

TEST_REFRACT("render", "issue-328-1");
TEST_REFRACT("render", "issue-328-2");

#if ! defined (_MSC_VER) || ! defined (_DEBUG)
TEST_REFRACT("render", "issue-556");
#endif

TEST_REFRACT("render", "fixed-attributes-section");
TEST_REFRACT("render", "fixed-named-type");
TEST_REFRACT("render", "mixin-override");

#if ! defined (_MSC_VER) || ! defined (_DEBUG)
TEST_REFRACT("render", "issue-547");

TEST_REFRACT("render", "nullable-samples");

TEST_REFRACT("render", "numbers");

TEST_REFRACT("render", "issue-566");
#endif

