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

            bool is;

        public:

            Is() : is(false) {}

            void operator()(const IElement& e) {
                is = false;
            }

            void operator()(const T& e) {
                is = true;
            }

            operator bool() const {
                return is;
            }
        };

    };

}; // namespace refract

#endif // #ifndef REFRACT_QUERY_H
