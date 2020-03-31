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

    bool is_restricted(char c) noexcept
    {
        for (const auto& r : { '!', '#', '$', '&', '^', '_', '-', '.', '+' }) {
            if (c == r)
                return true;
        }
        return false;
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

        if (v.empty())
            return out;

        if (end(v) == find_if(begin(v), end(v), [](const char c) { //
                return is_tspecial(c) || std::isspace(c) || std::iscntrl(c);
            }))
            return (out << v);

        return s8_quoted_string(out, v);
    }

    std::ostream& s8_restricted_name(std::ostream& out, const std::string& v)
    {
        using std::begin;
        using std::end;

        if (v.empty())
            return out;

        if (!std::isalnum(v.front()))
            return s8_quoted_string(out, v);

        if (end(v) == find_if(begin(v), end(v), [](const char c) { //
                return !std::isalnum(c) && !is_restricted(c);
            }))
            return (out << v);

        return s8_quoted_string(out, v);
    }
}

std::ostream& apib::backend::operator<<(std::ostream& out, const apib::parser::mediatype::state& obj)
{
    if (obj.type.empty() || obj.subtype.empty())
        return out;

    s8_restricted_name(out, obj.type);
    out << '/';

    s8_restricted_name(out, obj.subtype);

    if (!obj.suffix.empty()) {
        out << '+';
        s8_restricted_name(out, obj.suffix);
    }

    if (!obj.parameters.empty()) {
        for (const auto& p : obj.parameters) {
            out << "; ";
            s8_restricted_name(out, std::get<0>(p));
            out << '=';
            s8_value(out, std::get<1>(p));
        }
    }

    return out;
}
