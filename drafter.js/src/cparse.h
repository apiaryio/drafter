#ifndef CPARSE_H
#define CPARSE_H 1

#ifdef __cplusplus
extern "C" {
#endif

int c_parse(const char* source,
            bool requireBlueprintName,
            bool sourcemap,
            char** result);

int c_validate(const char *source,
               bool requireBlueprintName,
               char **result);

#ifdef __cplusplus
}
#endif

#endif
