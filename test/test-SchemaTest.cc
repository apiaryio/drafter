//
//  test-RenderTest.cc
//  drafter
//
//  Created by Vilibald Wanča on 19/11/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "draftertest.h"

using namespace draftertest;

TEST_REFRACT("schema", "boolean");
TEST_REFRACT("schema", "number");
TEST_REFRACT("schema", "string");
TEST_REFRACT("schema", "required");
TEST_REFRACT("schema", "optional");
TEST_REFRACT("schema", "object-simple");
TEST_REFRACT("schema", "required-object");
TEST_REFRACT("schema", "array-simple");
TEST_REFRACT("schema", "description");
TEST_REFRACT("schema", "required-array");
TEST_REFRACT("schema", "default-sample");
// FIXME:  enum bug #176
TEST_REFRACT("schema", "default-attribute");
TEST_REFRACT("schema", "escaping");
TEST_REFRACT("schema", "array-inline");
TEST_REFRACT("schema", "array-of-arrays");
TEST_REFRACT("schema", "array-of-types");
TEST_REFRACT("schema", "array-of-types-only");
TEST_REFRACT("schema", "array-with-nested-type");
TEST_REFRACT("schema", "array-with-nested-types");
TEST_REFRACT("schema", "array-of-types-mixed");
TEST_REFRACT("schema", "array-of-types-mixed-complex");
// FIXME:  enum bug #176
TEST_REFRACT("schema", "enum-with-type");
// FIXME:  enum bug #176
TEST_REFRACT("schema", "default-section");
// FIXME:  enum bug #176
TEST_REFRACT("schema", "enum-of-strings");
// FIXME:  enum bug #176
TEST_REFRACT("schema", "enum-containing-object");
// FIXME:  enum bug #176
TEST_REFRACT("schema", "enum-containing-enum");
// FIXME:  enum bug #176
TEST_REFRACT("schema", "enum-containing-sample");
// FIXME:  enum bug #176
TEST_REFRACT("schema", "sample");

//FIXME ... content differs, schema is the same
//TODO: Verify and decide what's correct
TEST_REFRACT("schema", "sample-complex");

// FIXME:  enum bug #176
TEST_REFRACT("schema", "sample-inline-attribute");

// FIXME:  enum bug #176
// TODO: Discuss it with hj, as current drafter seems legit
TEST_REFRACT("schema", "sample-inline-variable");

TEST_REFRACT("schema", "boolean-literal");
TEST_REFRACT("schema", "string-literal");
TEST_REFRACT("schema", "number-literal");
TEST_REFRACT("schema", "array-fixed");
TEST_REFRACT("schema", "array-fixed-inline");

// TODO: JSON Rendering incorrect?
// WTF Anyway? Needs detail look
TEST_REFRACT("schema", "array-fixed-inline-samples");

//TODO: JSON Rendering incorrect?
//WTF Anyway? Needs detail look
TEST_REFRACT("schema", "array-fixed-samples");

//TODO: JSON Rendering incorrect?
//WTF Anyway? Needs detail look
TEST_REFRACT("schema", "array-fixed-types-only");

//TODO: JSON Rendering incorrect?
//WTF Anyway? Needs detail look
TEST_REFRACT("schema", "array-fixed-samples-complex");

//TODO: JSON Rendering incorrect?
//WTF Anyway? Needs detail look
TEST_REFRACT("schema", "array-restricted-to-type");

//TODO:
//WTF Anyway? Needs detail look
TEST_REFRACT("schema", "array-restricted-to-types");

//TODO: JSON Rendering incorrect?
//WTF Anyway? Needs detail look
TEST_REFRACT("schema", "array-restricted-to-types-complex");

TEST_REFRACT("schema", "object-fixed");
TEST_REFRACT("schema", "object-fixed-values");

//TODO: JSON Rendering incorrect?
TEST_REFRACT("schema", "object-fixed-optional");

//TODO: Why content differ when it is the same?
TEST_REFRACT("schema", "object-complex");

//TODO: JSON Rendering incorrect?
//WTF Anyway? Needs detail look
TEST_REFRACT("schema", "object-very-complex");

//TODO: What exactly is going on?
TEST_REFRACT("schema", "variable-property");

// FIXME ... what should this do?
// boutique has empty result.
TEST_REFRACT("schema", "one-of");
TEST_REFRACT("schema", "one-of-complex");
TEST_REFRACT("schema", "one-of-properties");


