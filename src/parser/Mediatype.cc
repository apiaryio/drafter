//
//  parser/Mediatype.cc
//  apib::parser
//
//  Created by Jiri Kratochvil on 27/08/2019
//  Copyright (c) 2019 Apiary Inc. All rights reserved.
//

#include "Mediatype.h"
#include <cctype>

namespace apib
{
    namespace parser
    {

        namespace mediatype
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

            bool operator==(const state& lhs, const state& rhs)
            {
                using parameter = typename state::parameters_type::value_type;
                return iequal(lhs.type, rhs.type) && iequal(lhs.subtype, rhs.subtype) && iequal(lhs.suffix, rhs.suffix)
                    && lhs.parameters.size() == rhs.parameters.size()
                    && std::equal(lhs.parameters.cbegin(),
                           lhs.parameters.cend(),
                           rhs.parameters.cbegin(),
                           [](const parameter& lhs, const parameter& rhs) {
                               return iequal(lhs.first, rhs.first) && (lhs.second == rhs.second);
                           });
            }

            bool operator!=(const state& lhs, const state& rhs)
            {
                return !(lhs == rhs);
            }

            bool operator==(const state& lhs, const std::string& rhs)
            {
                std::string s;

                s.reserve(lhs.type.length() + lhs.subtype.length() + lhs.suffix.length() + 2);
                s.append(lhs.type);
                s.append("/");
                s.append(lhs.subtype);
                if (!lhs.suffix.empty()) {
                    s.append("+");
                    s.append(lhs.suffix);
                }

                return iequal(s, rhs);
            }

            bool operator!=(const state& lhs, const std::string& rhs)
            {
                return !(lhs == rhs);
            }

            bool operator==(const std::string& lhs, const state& rhs)
            {
                return operator==(rhs, lhs);
            }

            bool operator!=(const std::string& lhs, const state& rhs)
            {
                return !operator==(rhs, lhs);
            }

        }; // namespace mediatype
    };     // namespace parsers
};         // namespace apib
