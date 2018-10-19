#include "cparse.h"
#include <drafter.h>
#include <string.h>

int c_parse(const char* source,
            bool requireBlueprintName,
            bool sourcemap,
            char** result)
{
    drafter_parse_options options = {requireBlueprintName};

    return drafter_parse_blueprint_to(source,
                                      result,
                                      options,
                                      {sourcemap, DRAFTER_SERIALIZE_JSON});
}

int c_validate(const char *source,
               bool requireBlueprintName,
               char **result)
{
    drafter_result *res = NULL;
    drafter_parse_options options = {requireBlueprintName};

    int ret = drafter_check_blueprint(source, &res, options);

    if (NULL != res) {
        *result = drafter_serialize(res, {true, DRAFTER_SERIALIZE_JSON});
        drafter_free_result(res);
        ret = 1;
    }

    return ret;
}
