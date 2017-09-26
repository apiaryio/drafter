//
//  test-RenderTest.cc
//  drafter
//
//  Created by Vilibald Wanča on 19/11/15.
//  Copyright (c) 2015, 2016 Apiary Inc. All rights reserved.
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
TEST_REFRACT("schema", "enum-with-type");
TEST_REFRACT("schema", "default-section");
TEST_REFRACT("schema", "enum-of-strings");
TEST_REFRACT("schema", "enum-containing-object");
TEST_REFRACT("schema", "enum-containing-enum");
TEST_REFRACT("schema", "enum-containing-sample");
TEST_REFRACT("schema", "enum-nullable");
TEST_REFRACT("schema", "sample");
TEST_REFRACT("schema", "sample-complex");
TEST_REFRACT("schema", "sample-inline-attribute");
TEST_REFRACT("schema", "sample-fixed");

// FIXME: Discuss it with hj, as current drafter seems legit
// boutique version of schema
//   "content": "{\n  \"$schema\": \"http://json-schema.org/draft-04/schema#\",\n  \"type\": \"object\",\n
//   \"properties\": {\n    \"list\": {}\n  }\n}"
TEST_REFRACT("schema", "sample-inline-variable");

TEST_REFRACT("schema", "boolean-literal");
TEST_REFRACT("schema", "string-literal");
TEST_REFRACT("schema", "number-literal");
TEST_REFRACT("schema", "array-fixed");
TEST_REFRACT("schema", "array-fixed-inline");
TEST_REFRACT("schema", "array-fixed-inline-samples");
TEST_REFRACT("schema", "array-fixed-samples");
TEST_REFRACT("schema", "array-fixed-types-only");
TEST_REFRACT("schema", "array-fixed-type");
TEST_REFRACT("schema", "array-fixed-type-attributes");
TEST_REFRACT("schema", "array-fixed-type-object");

// FIXME: Discuss it with hj, as current drafter seems legit
// boutique version of schema
//"content": "{\n  \"$schema\": \"http://json-schema.org/draft-04/schema#\",\n  \"type\": \"object\",\n  \"properties\":
//{\n    \"tags\": {\n      \"type\": \"array\",\n      \"items\": [\n        {\n          \"type\": \"string\",\n
//\"enum\": [\n           \"hello\"\n          ]\n        },\n        {\n          \"type\": \"number\"\n        },\n
//{\n          \"type\": \"object\",\n          \"properties\": {\n            \"name\": {\n              \"type\":
//\"string\"\n            },\n            \"color\": {\n              \"type\": \"string\",\n              \"enum\":
//[\"white\"]\n            },\n            \"description\": {\n              \"type\": \"string\"\n            }\n
//},\n          \"additionalProperties\": false,\n          \"required\": [\"name\", \"color\", \"description\"]\n
//},\n        {\n          \"type\": \"string\",\n          \"enum\": [\"world\"]\n        }\n      ]\n    }\n  },\n
//\"required\": [\"tags\"]\n}"
TEST_REFRACT("schema", "array-fixed-samples-complex");

TEST_REFRACT("schema", "array-restricted-to-type");
TEST_REFRACT("schema", "array-restricted-to-types");
TEST_REFRACT("schema", "array-restricted-to-types-complex");

TEST_REFRACT("schema", "object-fixed");
TEST_REFRACT("schema", "object-fixed-values");
TEST_REFRACT("schema", "object-fixed-optional");
TEST_REFRACT("schema", "object-fixed-type");
TEST_REFRACT("schema", "object-fixed-type-named-type");
TEST_REFRACT("schema", "object-fixed-type-values");
TEST_REFRACT("schema", "object-complex");
TEST_REFRACT("schema", "object-very-complex");
TEST_REFRACT("schema", "object-override");

TEST_REFRACT("schema", "mixin-simple");

TEST_REFRACT("schema", "variable-property");
TEST_REFRACT("schema", "variable-property2");
TEST_REFRACT("schema", "variable-property-fixed-type");
TEST_REFRACT("schema", "variable-property-fixed-type2");

// FIXME: One Of - has non valid implementation for
// multiple "One Of" in one object
// e.g
// ```
// # Test
// + One of
//     + m1
//     + m2
// + One of
//     + m3
//     + m4
//```
// we are looking for solution how to correctly define Schema,
// But currently is this solution "good enough"
//
TEST_REFRACT("schema", "one-of");
TEST_REFRACT("schema", "one-of-complex");
TEST_REFRACT("schema", "one-of-properties");

TEST_REFRACT("schema", "issue-493-multiple-same-required");
