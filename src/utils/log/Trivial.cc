//
//  utils/log/Trivial.cc
//  librefract
//
//  Created by Thomas Jandecka on 16/02/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "Trivial.h"

#ifdef LOGGING
#include <fstream>
#endif

using namespace drafter;
using namespace utils;
using namespace log;

namespace
{
    bool enough_severity(severity s)
    {
        switch (s) {
            case debug:
#ifdef DEBUG
                return true;
#else
                return false;
#endif
            default:
                return true;
        }
    }
} // namespace

trivial_log& trivial_log::instance()
{
    static trivial_log instance_;
    return instance_;
}

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

trivial_entry::trivial_entry(trivial_log& log, severity svrty, size_t line, const char* file)
    : log_(log), severity_(svrty), log_lock_(log_.mtx())
{
    if (enough_severity(severity_))
        if (auto* out = log_.out()) {
            *out << '[' << severity_to_str(svrty) << "]";
            *out << '[' << std::this_thread::get_id() << "]";
            *out << '[' << file << ':' << line << "] ";
        }
}

trivial_entry::~trivial_entry()
{
    if (enough_severity(severity_))
        if (auto* out = log_.out()) {
            *out << '\n'; // TODO @tjanc@ could throw
        }
}

std::mutex& trivial_log::mtx() const
{
    return write_mtx_;
}

std::ostream* trivial_log::out()
{
    return out_;
}

void trivial_log::enable()
{
    std::lock_guard<std::mutex> lock(write_mtx_);
#ifdef LOGGING
    static std::ofstream log_file_{ "drafter.log" };
    out_ = &log_file_;
#endif
}
