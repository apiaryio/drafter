//
//  MediaType.h
//  apib
//
//  Created by Thomas Jandecka on 07/04/2020
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//

#ifndef APIB_SYNTAX_MEDIA_TYPE_H
#define APIB_SYNTAX_MEDIA_TYPE_H

#include <string>
#include <vector>
#include <utility>

namespace apib
{
    namespace syntax
    {
        struct media_type {
            using parameter = std::pair<std::string, std::string>;

            std::string type;
            std::string subtype;
            std::string suffix;
            std::vector<parameter> parameters;
        };

    }

    bool isJSON(const syntax::media_type&) noexcept;
    bool isJSONSchema(const syntax::media_type&) noexcept;

}

#endif
