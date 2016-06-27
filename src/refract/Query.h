//
//  refract/Query.h
//  librefract
//
//  Created by Jiri Kratochvil on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_QUERY_H
#define REFRACT_QUERY_H

#include "ElementFwd.h"

#include <functional>
#include <string>

namespace refract
{

    namespace query {

        typedef std::function<bool(const IElement&)> Query;

        class Element {
            const std::string name;

        public:

            Element(const std::string& name) : name(name) {}

            bool operator()(const IElement& e);

        };

    };

}; // namespace refract

#endif // #ifndef REFRACT_QUERY_H
