#include "../src/drafter.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "../src/Version.h"

const char* source = "# My API\n## GET /message\n + Response 200 (text/plain)\n\n        Hello World\n";

/* Just partial we need no check full string */
const char* expected = "element: \"parseResult\"\ncontent:\n  -\n    element: \"category\"\n    meta:\n      classes:\n        - \"api\"\n      title: \"My API\"\n";

int test_parse_and_serialize() {
    drafter_result* result = 0;

    int status = drafter_parse_blueprint(source, &result);

    assert(status == 0);
    assert(result);

    drafter_options options;
    options.sourcemap = false;
    options.format = DRAFTER_SERIALIZE_YAML;

    char* out = drafter_serialize(result, options);
    assert(out);

    int len = strlen(expected);

    assert(strncmp(out, expected, len) == 0);

    drafter_free_result(result);
    free(out);

    return 0;
};

int test_parse_to_string() {

    drafter_options options;
    options.sourcemap = false;
    options.format = DRAFTER_SERIALIZE_YAML;

    char* result = 0;

    int status = drafter_parse_blueprint_to(source, &result, options);

    assert(status == 0);
    assert(result);

    int len = strlen(expected);
    assert(strncmp(result, expected, len) == 0);

    free(result);

    return 0;
};



int test_version() {
    assert(drafter_version() != 0);
    assert(strcmp(drafter_version_string(), DRAFTER_VERSION_STRING) == 0);
    return 0;
}

const char* source_warn = "# My API\n## GET /message\n + Response 200 (text/plain)\n\n    Hello World\n";
const char* warn = "message-body asset is expected to be a pre-formatted code block, every of its line indented by exactly 8 spaces or 2 tabs";

int test_validation() {
    assert(drafter_check_blueprint(source) == 0);

    drafter_result* result = drafter_check_blueprint(source_warn);
    assert(result != 0);

    drafter_options options;
    options.sourcemap = false;
    options.format = DRAFTER_SERIALIZE_YAML;

    char* out = drafter_serialize(result, options);
    assert(out);

    /* check if output contains required warning message */
    assert(strstr(out, warn) != 0);

    drafter_free_result(result);
    free(out);
    return 0;
}

int main() {
    assert(test_parse_and_serialize() == 0);
    assert(test_parse_to_string() == 0);
    assert(test_version() == 0);
    assert(test_validation() == 0);
    return 0;
}
