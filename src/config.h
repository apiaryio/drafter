//
// vi:cin:et:sw=4 ts=4
//
//  config.h - part of drafter
//
//  Created by Jiri Kratochvil on 2015-02-13
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef DRAFTER_CONFIG_H
#define DRAFTER_CONFIG_H

#include <string>

#include "Serialize.h"

struct Config {
    std::string input;
    bool lineNumbers;
    bool validate;
    drafter::SerializeFormat format;
    drafter::ASTType astType;
    bool sourceMap;
    std::string output;
};

/**
 *  \brief parse commandline params
 *
 *  \param argc argc as received to main()
 *  \param argv argv as received to main()
 *  \param conf output - parsed parameters
 *
 *  side effect - can call exit() if wrong params are sent to program
 */
void ParseCommadLineOptions(int argc, const char *argv[], /** out */Config& conf);


#endif /* end of include guard: DRAFTER_CONFIG_H */
