#include "draftertest.h"

using namespace draftertest;

TEST_REFRACT_SOURCE_MAP("api", "description");
TEST_REFRACT_SOURCE_MAP("api", "metadata");
TEST_REFRACT_SOURCE_MAP("api", "resource-group");
TEST_REFRACT_SOURCE_MAP("api", "data-structure");
TEST_REFRACT_SOURCE_MAP("api", "resource");
TEST_REFRACT_SOURCE_MAP("api", "action");
TEST_REFRACT_SOURCE_MAP("api", "advanced-action");
TEST_REFRACT_SOURCE_MAP("api", "relation");
TEST_REFRACT_SOURCE_MAP("api", "resource-attributes");
TEST_REFRACT_SOURCE_MAP("api", "action-attributes");
TEST_REFRACT_SOURCE_MAP("api", "payload-attributes");
TEST_REFRACT_SOURCE_MAP("api", "transaction");
TEST_REFRACT_SOURCE_MAP("api", "headers");
TEST_REFRACT_SOURCE_MAP("api", "resource-parameters");
TEST_REFRACT_SOURCE_MAP("api", "action-parameters");
TEST_REFRACT_SOURCE_MAP("api", "request-parameters");
TEST_REFRACT_SOURCE_MAP("api", "request-only");
TEST_REFRACT_SOURCE_MAP("api", "asset");

#if ! defined (_MSC_VER) || ! defined (_DEBUG)
TEST_REFRACT_SOURCE_MAP("api", "xml-body");
#endif

TEST_REFRACT_SOURCE_MAP("api", "schema-body");

#if ! defined (_MSC_VER) || ! defined (_DEBUG)
TEST_REFRACT_SOURCE_MAP("api", "schema-custom");
#endif

TEST_REFRACT_SOURCE_MAP("api", "mson");
TEST_REFRACT_SOURCE_MAP("api", "attributes-references");
TEST_REFRACT_SOURCE_MAP("api", "action-request-attributes");

#if ! defined (_MSC_VER) || ! defined (_DEBUG)
TEST_REFRACT_SOURCE_MAP("parse-result", "simple");
TEST_REFRACT_SOURCE_MAP("parse-result", "warning");
TEST_REFRACT_SOURCE_MAP("parse-result", "warnings");
#endif

TEST_REFRACT_SOURCE_MAP("parse-result", "error-warning");
TEST_REFRACT_SOURCE_MAP("parse-result", "blueprint");
TEST_REFRACT_SOURCE_MAP("parse-result", "mson");

TEST_REFRACT_SOURCE_MAP("mson", "type-attributes");

#if ! defined (_MSC_VER) || ! defined (_DEBUG)
TEST_REFRACT_SOURCE_MAP("mson", "type-attributes-payload");
#endif

TEST_REFRACT_SOURCE_MAP("api", "issue-386");
