//
//  ContentTypeMatcher.h
//  drafter
//
//  Created by Jiri Kratochvil on 04/08/2019
//  Copyright (c) 2019 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_CONTENTYPEMATCHER_H
#define DRAFTER_CONTENTYPEMATCHER_H

#include "parser/Mediatype.h"

namespace drafter
{

    namespace pegtl = tao::pegtl;

    const char* const JSONSchemaContentType = "application/schema+json";

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

    struct JSONContentTypeComparator {
        bool operator==(const parser::mediatype::state& other) const
        {
            return iequal(other.type, "application")
                && ((other.suffix.empty() && iequal(other.subtype, "json"))
                       || (iequal(other.suffix, "json") && !iequal(other.subtype, "schema")));
        }
    };

    struct ContentTypeMatcher {
        template <typename Comparable>
        bool operator()(const Comparable& mediaType, const std::string& input) const
        {
            using namespace parser::mediatype;
            using namespace tao;

            state result;
            pegtl::memory_input<> in(input, "");
            try {
                return pegtl::parse<match_grammar, action>(in, result) && mediaType == result;
            } catch (pegtl::parse_error& e) {
                // do nothing
            }
            return false;
        }
    };

    auto IsJSONSchemaContentType
        = [](const std::string& contentType) { return ContentTypeMatcher{}(JSONSchemaContentType, contentType); };

    auto IsJSONContentType
        = [](const std::string& contentType) { return ContentTypeMatcher{}(JSONContentTypeComparator{}, contentType); };
}

#endif // DRAFTER_CONTENTYPEMATCHER_H
