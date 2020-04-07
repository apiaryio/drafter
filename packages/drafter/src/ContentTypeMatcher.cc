//
//  ContentTypeMatcher.cc
//  drafter
//
//  Created by Jiri Kratochvil on 27/08/2019
//  Copyright (c) 2019 Apiary Inc. All rights reserved.
//

#include "ContentTypeMatcher.h"
#include "parser/Mediatype.h"
#include <cctype>

namespace drafter
{
    namespace
    {
        bool iequal(const std::string& lhs, const std::string& rhs)
        {
            using chr = typename std::string::value_type;
            return (lhs.size() == rhs.size())
                && (std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), [](const chr& lhs, const chr& rhs) {
                       return std::tolower(lhs) == std::tolower(rhs);
                   }));
        }
    }

    apib::parser::mediatype::state parseMediaType(const std::string& input) {
            using namespace apib::parser::mediatype;
            namespace pegtl = tao::pegtl;

            state result;
            pegtl::memory_input<> in(input, "");
            pegtl::parse<pegtl::try_catch<match_grammar>, action>(in, result);
            return result;
    };

    bool IsJSONContentType(const apib::parser::mediatype::state& mediaType) {
            return iequal(mediaType.type, "application")
                && ((mediaType.suffix.empty() && iequal(mediaType.subtype, "json"))
                       || (iequal(mediaType.suffix, "json") && !iequal(mediaType.subtype, "schema")));
    }

    bool IsJSONSchemaContentType(const apib::parser::mediatype::state& mediaType) {
        return mediaType == JSONSchemaContentType;
    };


    struct JSONContentTypeComparator {
        bool operator==(const apib::parser::mediatype::state& other) const
        {
            return iequal(other.type, "application")
                && ((other.suffix.empty() && iequal(other.subtype, "json"))
                       || (iequal(other.suffix, "json") && !iequal(other.subtype, "schema")));
        }
    };
}
