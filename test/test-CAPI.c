#include "../src/drafter.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../src/Version.h"

const char* source = "# My API\n## GET /message\n + Response 200 (text/plain)\n\n        Hello World\n";

/* Just partial we need no check full string */
const char* expected
    = "element: \"parseResult\"\ncontent:\n  -\n    element: \"category\"\n    meta:\n      classes:\n        element: "
      "\"array\"\n        content:\n          -\n            element: \"string\"\n            content: \"api\"\n      "
      "title:\n        element: \"string\"\n        content: \"My API\"\n";

void assert_true(bool flag) {
    if (!flag) {
        abort();
    }
}

int test_parse_and_serialize()
{
    drafter_result* result = NULL;
    drafter_parse_options parseOptions = { false };

    int status = drafter_parse_blueprint(source, &result, parseOptions);

    assert_true(status == 0);
    assert_true(result);

    drafter_serialize_options serializeOptions;
    serializeOptions.sourcemap = false;
    serializeOptions.format = DRAFTER_SERIALIZE_YAML;

    char* out = drafter_serialize(result, serializeOptions);
    assert_true(out);

    size_t len = strlen(expected);

    assert_true(strncmp(out, expected, len) == 0);

    drafter_free_result(result);
    free(out);

    return 0;
};

int test_parse_to_string()
{

    drafter_parse_options parseOptions = { false };
    drafter_serialize_options options;
    options.sourcemap = false;
    options.format = DRAFTER_SERIALIZE_YAML;

    char* result = 0;

    int status = drafter_parse_blueprint_to(source, &result, parseOptions, options);

    assert_true(status == 0);
    assert_true(result);

    size_t len = strlen(expected);
    assert_true(strncmp(result, expected, len) == 0);

    free(result);

    return 0;
};

int test_version()
{
    assert_true(drafter_version() != 0);
    assert_true(strcmp(drafter_version_string(), DRAFTER_VERSION_STRING) == 0);
    return 0;
}

const char* source_warning = "# My API\n## GET /message\n + Response 200 (text/plain)\n\n    Hello World\n";
const char* warning
    = "message-body asset is expected to be a pre-formatted code block, every of its line indented by exactly 8 spaces "
      "or 2 tabs";

int test_validation()
{
    drafter_parse_options parseOptions = { false };
    drafter_result* result = NULL;

    assert_true(drafter_check_blueprint(source, &result, parseOptions) == 0);

    int status = drafter_check_blueprint(source_warning, &result, parseOptions);
    assert_true(status == 0);
    assert_true(result != 0);

    drafter_serialize_options options;
    options.sourcemap = false;
    options.format = DRAFTER_SERIALIZE_YAML;

    char* out = drafter_serialize(result, options);
    assert_true(out);

    /* check if output contains required warning message */
    assert_true(strstr(out, warning) != 0);

    drafter_free_result(result);
    free(out);
    return 0;
}

const char* source_without_name = "# GET /\n+ Response 204\n";
const char* expected_without_name = "expected API name, e.g. '# <API Name>'";

int test_parse_to_string_requiring_name()
{
    drafter_parse_options parseOptions = { true };
    drafter_serialize_options options;
    options.sourcemap = false;
    options.format = DRAFTER_SERIALIZE_YAML;

    char* result = 0;

    int status = drafter_parse_blueprint_to(source_without_name, &result, parseOptions, options);

    assert_true(status != 0);
    assert_true(result);

    assert_true(strstr(result, expected_without_name) != 0);

    free(result);

    return 0;
};

const char* source_mson = "# GET /\n+ Response 200 (application/json)\n    + Attributes\n        + name: pavan";
const char* message_body = "\"messageBody\"";
const char* message_body_schema = "\"messageBodySchema\"";

int test_parse_to_string_without_assets()
{
    drafter_parse_options parseOptions = { false };
    drafter_serialize_options options;
    options.sourcemap = false;
    options.format = DRAFTER_SERIALIZE_YAML;

    char* result = 0;

    int status = drafter_parse_blueprint_to(source_mson, &result, parseOptions, options);

    assert_true(status == 0);
    assert_true(result);

    assert_true(strstr(result, message_body) == 0);
    assert_true(strstr(result, message_body_schema) == 0);

    free(result);

    return 0;
};

int test_parse_to_string_with_body()
{
    drafter_parse_options parseOptions = { false, true, false };
    drafter_serialize_options options;
    options.sourcemap = false;
    options.format = DRAFTER_SERIALIZE_YAML;

    char* result = 0;

    int status = drafter_parse_blueprint_to(source_mson, &result, parseOptions, options);

    assert_true(status == 0);
    assert_true(result);

    assert_true(strstr(result, message_body) != 0);
    assert_true(strstr(result, message_body_schema) == 0);

    free(result);

    return 0;
};

int test_parse_to_string_with_schema()
{
    drafter_parse_options parseOptions = { false, false, true };
    drafter_serialize_options options;
    options.sourcemap = false;
    options.format = DRAFTER_SERIALIZE_YAML;

    char* result = 0;

    int status = drafter_parse_blueprint_to(source_mson, &result, parseOptions, options);

    assert_true(status == 0);
    assert_true(result);

    assert_true(strstr(result, message_body) == 0);
    assert_true(strstr(result, message_body_schema) != 0);

    free(result);

    return 0;
};

int main()
{
    assert_true(test_parse_and_serialize() == 0);
    assert_true(test_parse_to_string() == 0);
    assert_true(test_version() == 0);
    assert_true(test_validation() == 0);
    assert_true(test_parse_to_string_requiring_name() == 0);
    assert_true(test_parse_to_string_without_assets() == 0);
    assert_true(test_parse_to_string_with_body() == 0);
    assert_true(test_parse_to_string_with_schema() == 0);
    return 0;
}
