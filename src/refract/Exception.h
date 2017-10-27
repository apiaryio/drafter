//
//  refract/Exception.h
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_EXCEPTION_H
#define REFRACT_EXCEPTION_H

#include <stdexcept>

namespace refract
{

    struct LogicError : std::logic_error {
        explicit LogicError(const std::string& msg) : std::logic_error(msg) {}
    };

    // will be removed in future
    struct NotImplemented : std::runtime_error {
        explicit NotImplemented(const std::string& msg) : std::runtime_error(msg) {}
    };

    struct Deprecated : std::logic_error {
        explicit Deprecated(const std::string& msg) : std::logic_error(msg) {}
    };

}; // namespace refract

#endif // #ifndef REFRACT_EXCEPTION_H
