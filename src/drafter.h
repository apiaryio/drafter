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

//#if defined(BUILDING_DRAFTER)
#if defined(BUILDING_SNOWCRASH)
#   define DEPRECATED
#endif

#if defined(_MSC_VER)
#   if !defined(DEPRECATED)
#       define DEPRECATED __declspec(deprecated)
#   endif
#elif defined(__clang__) || defined(__GNUC__)
#   if !defined(DEPRECATED)
#       define DEPRECATED __attribute__((deprecated))
#   endif
#else
#   if !defined(DEPRECATED)
#       define DEPRECATED
#   endif
#endif

#ifndef DRAFTER_API
#  if defined _WIN32 || defined __CYGWIN__
#     if defined(DRAFTER_BUILD_SHARED) /* build dll */
#         define DRAFTER_API __declspec(dllexport)
#     elif !defined(DRAFTER_BUILD_STATIC) /* use dll */
#         define DRAFTER_API __declspec(dllimport)
#     else /* static library */
#         define DRAFTER_API /* nothing */
#     endif
#  else
#     if __GNUC__ >= 4
#         define DRAFTER_API __attribute__((visibility("default")))
#     else
#         define DRAFTER_API
#     endif
#  endif
#endif


#ifndef __cplusplus
#include <stdbool.h>
typedef struct drafter_result drafter_result;
#else
namespace refract {
    struct IElement;
}
typedef refract::IElement drafter_result;
#endif


typedef enum {
    DRAFTER_SERIALIZE_YAML = 0,
    DRAFTER_SERIALIZE_JSON
} drafter_format;

typedef struct {
    bool sourcemap;
    drafter_format format;
} drafter_options;

/* Parse API Blueprint and serialize it to given format.*/
DRAFTER_API int drafter_parse_blueprint_to(const char* source,
                                           char** out,
                                           const drafter_options options);

/* Parse API Blueprint and return result, which is a opaque handle for
 * later use*/
DRAFTER_API int drafter_parse_blueprint(const char* source, drafter_result** out);

/* Serialize result to given format*/
DRAFTER_API char* drafter_serialize(drafter_result *res, const drafter_options options);

/* Free memory alocated for result handler */
DRAFTER_API void drafter_free_result(drafter_result* result);

/* Parse API Blueprint and return only annotations, if NULL than
 * document is error and warning free.*/
DRAFTER_API drafter_result* drafter_check_blueprint(const char* source);

DRAFTER_API unsigned int drafter_version(void);

DRAFTER_API const char* drafter_version_string(void);

#ifdef __cplusplus
}
#endif

#endif // #ifndef DRAFTER_H
