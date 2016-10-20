//
//  drafter_private.h
//  drafter
//
//  Created by Pavan Kumar Sunkara on 20/10/16.
//  Copyright © 2016 Apiary. All rights reserved.
//

//TODO: This should be merged with drafter.h in next major release
#ifndef DRAFTER_PRIVATE_H
#define DRAFTER_PRIVATE_H

#include "drafter.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool requireBlueprintName;
} drafter_parse_options;

DRAFTER_API int drafter_parse_blueprint_with_options(const char* source,
                                                     drafter_result** out,
                                                     const drafter_parse_options options);

DRAFTER_API drafter_result* drafter_check_blueprint_with_options(const char* source,
                                                                 const drafter_parse_options options);

#ifdef __cplusplus
}
#endif

#endif // #ifndef DRAFTER_PRIVATE_H
