//
//  MediaType.cc
//  apib
//
//  Created by Thomas Jandecka on 07/04/2020
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//

#include <apib/syntax/MediaType.h>

#include <cctype>

using namespace apib::syntax;

namespace
{
    bool iequal(const std::string& lhs, const std::string& rhs) noexcept
    {
        return (lhs.size() == rhs.size())
            && (std::equal( //
                lhs.cbegin(),
                lhs.cend(),
                rhs.cbegin(),
                [](char lhs, char rhs) { //
                    return std::tolower(lhs) == std::tolower(rhs);
                }));
    }

    const std::string& syntaxType(const media_type& t) noexcept
    {
        if (t.suffix.empty())
            return t.subtype;
        return t.suffix;
    }

}

namespace
{
    const char* APPLICATION_TYPE = "application";
    const char* JSON_SUBTYPE = "json";
    const char* SCHEMA_SUBTYPE = "schema";
}

using namespace apib::syntax;

bool apib::isJSON(const media_type& t) noexcept
{
    return iequal(t.type, APPLICATION_TYPE) //
        && iequal(syntaxType(t), JSON_SUBTYPE);
}

bool apib::isJSONSchema(const media_type& t) noexcept
{
    return iequal(t.type, APPLICATION_TYPE)  //
        && iequal(t.subtype, SCHEMA_SUBTYPE) //
        && iequal(t.suffix, JSON_SUBTYPE);
}
