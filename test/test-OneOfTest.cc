
#include "draftertest.h"

using namespace draftertest;

TEST_REFRACT("oneof", "simple");
TEST_REFRACT("oneof", "object");
TEST_REFRACT("oneof", "multi-properties");


// FIXME: invalid JSON Schema
TEST_REFRACT("oneof", "multi-oneof");
