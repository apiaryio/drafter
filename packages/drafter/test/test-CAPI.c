#include "../src/drafter.h"

#include "ctesting.h"

#include <string.h>
#include <stdlib.h>

#if defined CMAKE_BUILD_TYPE
#include "../src/Version.h"
#endif

const char* source = "# My API\n## GET /message\n + Response 200 (text/plain)\n\n        Hello World\n";

/* Just partial we need no check full string */
const char* expected
    = "element: \"parseResult\"\ncontent:\n  -\n    element: \"category\"\n    meta:\n      classes:\n        element: "
      "\"array\"\n        content:\n          -\n            element: \"string\"\n            content: \"api\"\n      "
      "title:\n        element: \"string\"\n        content: \"My API\"\n";

int test_parse_and_serialize()
{
    drafter_result* result = NULL;

    drafter_parse_options* parseOptions = drafter_init_parse_options();
    int status = drafter_parse_blueprint(source, &result, parseOptions);
    drafter_free_parse_options(parseOptions);

    REQUIRE(status == 0);
    REQUIRE(result);

    drafter_serialize_options* serializeOptions = drafter_init_serialize_options();
    char* out = drafter_serialize(result, serializeOptions);
    drafter_free_serialize_options(serializeOptions);

    REQUIRE(out);

    size_t len = strlen(expected);

    REQUIRE(strncmp(out, expected, len) == 0);

    drafter_free_result(result);
    free(out);

    return 0;
};

int test_parse_to_string()
{

    char* result = NULL;

    const int status = drafter_parse_blueprint_to(source, &result, NULL, NULL);

    REQUIRE(status == 0);
    REQUIRE(result);

    const size_t len = strlen(expected);
    REQUIRE(strncmp(result, expected, len) == 0);

    free(result);

    return 0;
};

int test_version()
{
#if defined CMAKE_BUILD_TYPE
    REQUIRE(drafter_version() != 0);
    REQUIRE(strcmp(drafter_version_string(), DRAFTER_VERSION_STRING) == 0);
#endif
    return 0;
}

const char* source_warning = "# My API\n## GET /message\n + Response 200 (text/plain)\n\n    Hello World\n";
const char* warning
    = "message-body asset is expected to be a pre-formatted code block, every of its line indented by exactly 8 spaces "
      "or 2 tabs";

int test_validation()
{
    drafter_parse_options* parseOptions = drafter_init_parse_options();
    drafter_result* result = NULL;

    REQUIRE(drafter_check_blueprint(source, &result, parseOptions) == 0);

    int status = drafter_check_blueprint(source_warning, &result, parseOptions);
    drafter_free_parse_options(parseOptions);

    REQUIRE(status == 0);
    REQUIRE(result != 0);

    drafter_serialize_options* options = drafter_init_serialize_options();
    char* out = drafter_serialize(result, options);
    drafter_free_serialize_options(options);

    REQUIRE(out);

    /* check if output contains required warning message */
    REQUIRE_INCLUDES(warning, out);

    drafter_free_result(result);
    free(out);
    return 0;
}

int test_validation_default()
{
    REQUIRE(DRAFTER_OK == drafter_check_blueprint(source, NULL, NULL));
    return 0;
}

int test_blueprint_to_serialized_elements_default()
{
    REQUIRE(DRAFTER_OK == drafter_parse_blueprint_to(source, NULL, NULL, NULL));
    return 0;
}

int test_blueprint_to_elements_default()
{
    REQUIRE(DRAFTER_OK == drafter_parse_blueprint(source, NULL, NULL));
    return 0;
}

const char* source_without_name = "# GET /\n+ Response 204\n";
const char* expected_without_name = "expected API name, e.g. '# <API Name>'";

int test_parse_to_string_requiring_name()
{
    char* result = 0;

    drafter_parse_options* parseOptions = drafter_init_parse_options();
    drafter_set_name_required(parseOptions);

    drafter_serialize_options* options = drafter_init_serialize_options();

    int status = drafter_parse_blueprint_to(source_without_name, &result, parseOptions, options);

    drafter_free_serialize_options(options);
    drafter_free_parse_options(parseOptions);

    REQUIRE(status != 0);
    REQUIRE(result);

    REQUIRE_INCLUDES(expected_without_name, result);

    free(result);

    return 0;
};

const char* apib_with_attrs_no_body_nor_schema
    = "# Data Structures\n\
## User\n\
+ username: pksunkara\n\
\n\
## Org\n\
+ name: Apiary\n\
\n\
# Group Example\n\
# GET /\n\
+ Response 200 (application/json)\n\
    + Attributes (array[User, Org])";

int test_parse_to_string_skip_body_gen()
{
    char* result = 0;

    drafter_parse_options* pOpts = drafter_init_parse_options();
    drafter_set_skip_gen_bodies(pOpts);

    int status = drafter_parse_blueprint_to( //
        apib_with_attrs_no_body_nor_schema,
        &result,
        pOpts,
        NULL);

    drafter_free_parse_options(pOpts);

    REQUIRE(result);

    REQUIRE_EXCLUDES("\"messageBody\"", result);
    REQUIRE_INCLUDES("\"messageBodySchema\"", result);
    REQUIRE(status == 0);

    free(result);
}

int test_parse_to_string_skip_body_schema_gen()
{
    char* result = 0;

    drafter_parse_options* pOpts = drafter_init_parse_options();
    drafter_set_skip_gen_body_schemas(pOpts);

    int status = drafter_parse_blueprint_to( //
        apib_with_attrs_no_body_nor_schema,
        &result,
        pOpts,
        NULL);

    drafter_free_parse_options(pOpts);

    REQUIRE(result);

    REQUIRE_INCLUDES("\"messageBody\"", result);
    REQUIRE_EXCLUDES("\"messageBodySchema\"", result);

    REQUIRE(status == 0);

    free(result);
}

int main()
{
    REQUIRE(test_parse_and_serialize() == 0);
    REQUIRE(test_parse_to_string() == 0);
    REQUIRE(test_version() == 0);
    REQUIRE(test_validation() == 0);
    REQUIRE(test_parse_to_string_requiring_name() == 0);
    REQUIRE(test_validation_default() == 0);
    REQUIRE(test_blueprint_to_serialized_elements_default() == 0);
    REQUIRE(test_blueprint_to_elements_default() == 0);
    test_parse_to_string_skip_body_gen();
    test_parse_to_string_skip_body_schema_gen();

    return 0;
}
