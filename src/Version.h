//
//  version.h
//  drafter
//
//  Created by Jiri Kratochvil on 2015-02-13
//  Attribution Notice:
//  This work might use parts of Node.js `node_version.h`
//  https://github.com/joyent/node
//
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_VERSION_H
#define DRAFTER_VERSION_H

#define DRAFTER_MAJOR_VERSION 0
#define DRAFTER_MINOR_VERSION 1
#define DRAFTER_PATCH_VERSION 9

#define DRAFTER_VERSION_IS_RELEASE 1

#ifndef DRAFTER_STRINGIFY
#   define DRAFTER_STRINGIFY(n) DRAFTER_STRINGIFY_HELPER(n)
#   define DRAFTER_STRINGIFY_HELPER(n) #n
#endif

#define DRAFTER_VERSION_STRING_HELPER "v" \
                                        DRAFTER_STRINGIFY(DRAFTER_MAJOR_VERSION) "." \
                                        DRAFTER_STRINGIFY(DRAFTER_MINOR_VERSION) "." \
                                        DRAFTER_STRINGIFY(DRAFTER_PATCH_VERSION)

#if DRAFTER_VERSION_IS_RELEASE
#   define DRAFTER_VERSION_STRING DRAFTER_VERSION_STRING_HELPER
#else
#   define DRAFTER_VERSION_STRING DRAFTER_VERSION_STRING_HELPER "-pre"
#endif

#endif
