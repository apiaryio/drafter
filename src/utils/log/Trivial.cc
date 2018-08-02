//
//  utils/log/Trivial.cc
//  librefract
//
//  Created by Thomas Jandecka on 16/02/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "Trivial.h"

using namespace drafter;
using namespace utils;
using namespace log;

trivial_log& trivial_log::instance()
{
    static trivial_log instance_{ "drafter.log" };
    return instance_;
}

trivial_log::trivial_log(const char* file) : out_(file), enabled_(false) {}

const char* log::severity_to_str(severity s)
{
    switch (s) {
        case debug:
            return "DEBUG";
        case info:
            return "INFO ";
        case warning:
            return "WARN ";
        case error:
            return "ERROR";
        default:
            return "";
    }
}
