#include "../src/drafter.h"

#include <assert.h>
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

int test_parse_and_serialize()
{
    drafter_result* result = NULL;
    drafter_parse_options parseOptions = { false };

    int status = drafter_parse_blueprint(source, &result, parseOptions);

    assert(status == 0);
    assert(result);

    drafter_serialize_options serializeOptions;
    serializeOptions.sourcemap = false;
    serializeOptions.format = DRAFTER_SERIALIZE_YAML;

    char* out = drafter_serialize(result, serializeOptions);
    assert(out);

    size_t len = strlen(expected);

    assert(strncmp(out, expected, len) <= 0);

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

    assert(status == 0);
    assert(result);

    size_t len = strlen(expected);
    assert(strncmp(result, expected, len) <= 0);

    free(result);

    return 0;
};

int test_version()
{
    assert(drafter_version() != 0);
    assert(strcmp(drafter_version_string(), DRAFTER_VERSION_STRING) == 0);
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

    assert(drafter_check_blueprint(source, &result, parseOptions) == 0);

    int status = drafter_check_blueprint(source_warning, &result, parseOptions);
    assert(status == 0);
    assert(result != 0);

    drafter_serialize_options options;
    options.sourcemap = false;
    options.format = DRAFTER_SERIALIZE_YAML;

    char* out = drafter_serialize(result, options);
    assert(out);

    /* check if output contains required warning message */
    assert(strstr(out, warning) != 0);

    drafter_free_result(result);
    free(out);
    return 0;
}

int main()
{
    assert(test_parse_and_serialize() == 0);
    assert(test_parse_to_string() == 0);
    assert(test_version() == 0);
    assert(test_validation() == 0);
    return 0;
}
