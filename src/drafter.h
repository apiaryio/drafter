//
//  drafter.h
//
//  drafter
//
//  Created by Jiri Kratochvil on 2015-03-05
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_H
#define DRAFTER_H

#include "snowcrash.h"


/**
 *  API Blueprint Parser Interface
 *  ------------------------------
 *
 *  This is the parser's entry point.
 *
 *  For Snow Crash users, this is the only interface to use.
 *
 *  For binding writers, this is the point to start wrapping.
 *  Refer to https://github.com/apiaryio/snowcrash/wiki/Writing-a-binding
 *  for details on how to write a Snow Crash binding.
 */

namespace drafter {

    /**
     *  \brief Parse the source data into a blueprint abstract source tree (AST).
     *
     *  \param source       A textual source data to be parsed.
     *  \param options      Parser options. Use 0 for no additional options.
     *  \param out          Output buffer to store parsing result into.
     *  \return Error status code. Zero represents success, non-zero a failure.
     */
    int parse_blueprint(const mdp::ByteBuffer& source,
              snowcrash::BlueprintParserOptions options,
              const snowcrash::ParseResultRef<snowcrash::Blueprint>& out);

}

#endif // #ifndef DRAFTER_H
