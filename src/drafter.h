//
//  drafter.h
//  drafter
//
//  Created by Jiri Kratochvil on 2016-06-27
//  Copyright (c) 2016 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_H
#define DRAFTER_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(BUILDING_DRAFTER)
#define DEPRECATED
#endif

#if defined(_MSC_VER)
#if !defined(DEPRECATED)
#define DEPRECATED __declspec(deprecated)
#endif
#elif defined(__clang__) || defined(__GNUC__)
#if !defined(DEPRECATED)
#define DEPRECATED __attribute__((deprecated))
#endif
#else
#if !defined(DEPRECATED)
#define DEPRECATED
#endif
#endif

#if defined _WIN32 || defined __CYGWIN__
#if defined(DRAFTER_BUILD_SHARED) /* build dll */
#if defined(BUILDING_DRAFTER)
#define DRAFTER_API __declspec(dllexport)
#else
#define DRAFTER_API __declspec(dllimport)
#endif
#else               /* static library */
#define DRAFTER_API /* nothing */
#endif
#else
#if __GNUC__ >= 4
#define DRAFTER_API __attribute__((visibility("default")))
#else
#define DRAFTER_API
#endif

#endif

#ifndef __cplusplus
#include <stdbool.h>
typedef struct drafter_result drafter_result;
#else
namespace refract
{
    struct IElement;
}
typedef refract::IElement drafter_result;
#endif

/* Serialization formats, currently only YAML or JSON */
typedef enum
{
    DRAFTER_SERIALIZE_YAML = 0,
    DRAFTER_SERIALIZE_JSON
} drafter_format;

/* Parsing options
 * - requireBlueprintName : API has to have a name, if not it is a parsing error
 */
typedef struct {
    bool requireBlueprintName;
} drafter_parse_options;

/* Serialization options
 * - sourcemap : Include sourcemap in the serialized result
 * - format : Serialization format see above
 */
typedef struct {
    bool sourcemap;
    drafter_format format;
} drafter_serialize_options;

typedef enum
{
    DRAFTER_OK = 0,
    DRAFTER_EUNKNOWN = -1,
    DRAFTER_EINVALID_INPUT = -2,
    DRAFTER_EINVALID_OUTPUT = -3,
} drafter_error;

/* Parse API Blueprint and serialize it to given format.
 * Returns:
 * - 0 if everything went smooth.
 * - positive numbers if it encountered parsing errors.
 * - negative numbers if it failed to parse due the programming errors like invalid input.
 */
DRAFTER_API drafter_error drafter_parse_blueprint_to(const char* source,
    char** out,
    const drafter_parse_options parse_opts,
    const drafter_serialize_options serialize_opts);

/* Parse API Blueprint and return result, which is a opaque handle for
 * later use
 *
 * Returns:
 * - 0 if everything went smooth.
 * - positive numbers if it encountered parsing errors.
 * - negative numbers if it failed to parse due the programming errors like invalid input.
 */
DRAFTER_API drafter_error drafter_parse_blueprint(
    const char* source, drafter_result** out, const drafter_parse_options parse_opts);

/* Serialize result to given format, returns NULL if an error is encountered */
DRAFTER_API char* drafter_serialize(drafter_result* res, const drafter_serialize_options serialize_opts);

/* Free memory allocated for result handler */
DRAFTER_API void drafter_free_result(drafter_result* res);

/* Parse API Blueprint and return only annotations.
 * Returns:
 * - 0 if everything went smooth.
 * - positive numbers if it encountered parsing errors, which are described in the result
 * - negative numbers if it failed to parse due the programming errors like invalid input.
 */
DRAFTER_API drafter_error drafter_check_blueprint(
    const char* source, drafter_result** res, const drafter_parse_options parse_opts);

DRAFTER_API unsigned int drafter_version(void);

DRAFTER_API const char* drafter_version_string(void);

#ifdef __cplusplus
}
#endif

#endif // #ifndef DRAFTER_H
