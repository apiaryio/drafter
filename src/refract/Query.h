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

namespace refract
{

    namespace query {

        template <typename T>
        class Is {

        public:

            bool operator()(const IElement& e) {
                return false;
            }

            bool operator()(const T& e) {
                return true;
            }
        };

        class ByName {
            const std::string& name;

        public:

            ByName(const std::string& name) : name(name) {}

            bool operator()(const IElement& e) {
                return false;
                //return e.element() == name;
            }
        };

    };

}; // namespace refract

#endif // #ifndef REFRACT_QUERY_H
