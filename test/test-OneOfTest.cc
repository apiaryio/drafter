
#include "draftertest.h"

using namespace draftertest;

TEST_REFRACT("oneof", "simple");
TEST_REFRACT("oneof", "object");
TEST_REFRACT("oneof", "multi-properties");
TEST_REFRACT("oneof", "expanded");
TEST_REFRACT("oneof", "required");
TEST_REFRACT("oneof", "required2");

// FIXME: invalid JSON Schema
TEST_REFRACT("oneof", "multi-oneof");
