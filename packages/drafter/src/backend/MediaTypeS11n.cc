//
//  backend/MediaTypeS11n.cc
//  apib::backend
//
//  Created by Thomas Jandecka on 02/21/2020
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//

#include "MediaTypeS11n.h"

#include <cctype>
// FIXME: avoid relative, cross-modular include by setting up build environment
//          to search for includes in `src`; then `#include <parser/Mediatype.h>`
#include "../parser/Mediatype.h"

using namespace apib::backend;

namespace
{
    std::ostream& s8_qtext(std::ostream& out, const std::string& v)
    {
        for (const auto& c : v)
            if (c == '\\' || c == '"')
                out << '\\' << c;
            else if (c == '\r')
                out << "\\r";
            else
                out << c;
        return out;
    }

    std::ostream& s8_quoted_string(std::ostream& out, const std::string& v)
    {
        return s8_qtext(out << '"', v) << '"';
    }

    bool is_tspecial(char c) noexcept
    {
        for (const auto& r : { '(', ')', '<', '>', '@', ',', ';', ':', '\\', '"', '/', '[', ']', '?', '=' }) {
            if (c == r)
                return true;
        }
        return false;
    }

    std::ostream& s8_value(std::ostream& out, const std::string& v)
    {
        using std::begin;
        using std::end;

        if (end(v) == find_if(begin(v), end(v), [](const char c) { //
                return is_tspecial(c) || std::isspace(c) || std::iscntrl(c);
            }))
            return (out << v);

        return s8_quoted_string(out, v);
    }
}

std::ostream& apib::backend::operator<<(std::ostream& out, const apib::parser::mediatype::state& obj)
{
    if (obj.type.empty() || obj.subtype.empty())
        return out;

    out << obj.type << '/' << obj.subtype;

    if (!obj.suffix.empty()) {
        out << '+' << obj.suffix;
    }

    if (!obj.parameters.empty()) {
        for (const auto& p : obj.parameters) {
            const auto& key = std::get<0>(p);
            if (!key.empty()) {
                out << "; " << std::get<0>(p) << '=';
                s8_value(out, std::get<1>(p));
            }
        }
    }

    return out;
}
