
#include "draftertest.h"

using namespace draftertest;

TEST_REFRACT("circular", "simple");
TEST_REFRACT("circular", "array");
TEST_REFRACT("circular", "cross");
TEST_REFRACT("circular", "embed");
TEST_REFRACT("circular", "enum");
TEST_REFRACT("circular", "array-in-object");

TEST_REFRACT("circular", "mixin-identity");
TEST_REFRACT("circular", "mixin-member");
TEST_REFRACT("circular", "mixin-embed");
TEST_REFRACT("circular", "mixin-cross");
TEST_REFRACT("circular", "mixed");
