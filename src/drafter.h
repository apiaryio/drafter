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

#ifndef DRAFTER_API
#if defined _WIN32 || defined __CYGWIN__
#if defined(DRAFTER_BUILD_SHARED) /* build dll */
#define DRAFTER_API __declspec(dllexport)
#elif !defined(DRAFTER_BUILD_STATIC) /* use dll */
#define DRAFTER_API __declspec(dllimport)
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

/* Parse options
 */
typedef struct drafter_parse_options drafter_parse_options;

/* Allocate and initialise parse options
 *   @remark available parse options: name_required
 *   @return parse options with name_required: false
 */
DRAFTER_API drafter_parse_options* drafter_init_parse_options();

/* Deallocate parse options
 */
DRAFTER_API void drafter_free_parse_options(drafter_parse_options*);

/* Set name_required option
 *   @remark name_required: documents without an API name section are rejected
 */
DRAFTER_API void drafter_set_name_required(drafter_parse_options*);

/* Set skip_gen_bodies option
 *   @remark skip_gen_bodies: skip generating message body payloads
 */
DRAFTER_API void drafter_set_skip_gen_bodies(drafter_parse_options*);

/* Set skip_gen_body_schemas option
 *   @remark skip_gen_body_schemas: skip generating message body schema payloads
 */
DRAFTER_API void drafter_set_skip_gen_body_schemas(drafter_parse_options*);

/* Serialisation options
 */
typedef struct drafter_serialize_options drafter_serialize_options;

/* Allocate and initialise serialisation options
 *   @remark available serialisation options: sourcemaps_included, format
 *   @return serialisation options with sourcemaps_included: false, format: YAML
 */
DRAFTER_API drafter_serialize_options* drafter_init_serialize_options();

/* Deallocate serialisation options
 */
DRAFTER_API void drafter_free_serialize_options(drafter_serialize_options*);

/* Set sourcemaps_included option
 *   @remark sourcemaps_included: source maps are not filtered from non-Annotations
 */
DRAFTER_API void drafter_set_sourcemaps_included(drafter_serialize_options*);

/* Set format option
 *   @remark format: API Elements serialisation format (YAML|JSON)
 */
DRAFTER_API void drafter_set_format(drafter_serialize_options*, drafter_format);

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
    const drafter_parse_options* parse_opts,
    const drafter_serialize_options* serialize_opts);

/* Parse API Blueprint and return result, which is a opaque handle for
 * later use
 *
 * Returns:
 * - 0 if everything went smooth.
 * - positive numbers if it encountered parsing errors.
 * - negative numbers if it failed to parse due the programming errors like invalid input.
 */
DRAFTER_API drafter_error drafter_parse_blueprint(
    const char* source, drafter_result** out, const drafter_parse_options* parse_opts);

/* Serialize result to given format, returns NULL if an error is encountered */
DRAFTER_API char* drafter_serialize(drafter_result* res, const drafter_serialize_options* serialize_opts);

/* Free memory allocated for result handler */
DRAFTER_API void drafter_free_result(drafter_result* res);

/* Parse API Blueprint and return only annotations.
 * Returns:
 * - 0 if everything went smooth.
 * - positive numbers if it encountered parsing errors, which are described in the result
 * - negative numbers if it failed to parse due the programming errors like invalid input.
 */
DRAFTER_API drafter_error drafter_check_blueprint(
    const char* source, drafter_result** res, const drafter_parse_options* parse_opts);

DRAFTER_API unsigned int drafter_version(void);

DRAFTER_API const char* drafter_version_string(void);

#ifdef __cplusplus
}
#endif

#endif // #ifndef DRAFTER_H
