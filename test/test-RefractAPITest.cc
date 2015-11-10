#include "draftertest.h"

using namespace draftertest;

TEST_REFRACT("api", "description");
TEST_REFRACT("api", "metadata");
TEST_REFRACT("api", "resource-group");
TEST_REFRACT("api", "data-structure");
TEST_REFRACT("api", "resource");
TEST_REFRACT("api", "action");
TEST_REFRACT("api", "advanced-action");
TEST_REFRACT("api", "relation");
TEST_REFRACT("api", "resource-attributes");
TEST_REFRACT("api", "action-attributes");
TEST_REFRACT("api", "payload-attributes");
TEST_REFRACT("api", "transaction");
TEST_REFRACT("api", "headers");
TEST_REFRACT("api", "resource-parameters");
TEST_REFRACT("api", "action-parameters");
TEST_REFRACT("api", "request-only");
TEST_REFRACT("api", "asset");
TEST_REFRACT("api", "attributes-references");
TEST_REFRACT("api", "action-request-attributes");

// regressions
TEST_REFRACT("api", "attributes-array")
