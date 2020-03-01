//
//  format/MediatypeFmt.cc
//  apib::format
//
//  Created by Thomas Jandecka on 02/21/2020
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//

#include "MediatypeFmt.h"

// FIXME: avoid relative, cross-modular include by setting up build environment
//          to search for includes in `src`; then `#include <parser/Mediatype.h>`
#include "../parser/Mediatype.h"

using namespace apib::format;

namespace
{
    std::ostream& serialize_quote_escaped(std::ostream& out, const std::string& v)
    {
        for (const auto& c : v)
            if (c == '"')
                out << "\\\"";
            else
                out << c;
        return out;
    }

    std::ostream& serialize_quoted(std::ostream& out, const std::string& v)
    {
        return serialize_quote_escaped(out << '"', v) << '"';
    }

    bool is_restricted(char c) noexcept
    {
        for (const auto& r : { '!', '#', '$', '&', '^', '_', '-', '.' })
            if (c == r)
                return true;
        return false;
    }

    std::ostream& serialize_sanitized(std::ostream& out, const std::string& v)
    {
        using std::begin;
        using std::end;

        if (v.empty())
            return out;

        if (!std::isalnum(v.front()))
            return serialize_quoted(out, v);

        if (end(v) == find_if(begin(v), end(v), [](const char c) { //
                return !std::isalnum(c) && !is_restricted(c);
            }))
            return (out << v);

        return serialize_quoted(out, v);
    }
}

std::ostream& apib::format::operator<<(std::ostream& out, const apib::parser::mediatype::state& obj)
{
    if (obj.type.empty())
        return out;

    serialize_sanitized(out, obj.type);
    out << '/';

    serialize_sanitized(out, obj.subtype);

    if (!obj.suffix.empty()) {
        out << '+';
        serialize_sanitized(out, obj.suffix);
    }

    if (!obj.parameters.empty()) {
        out << ';';
        for (const auto& p : obj.parameters) {
            out << ' ';
            serialize_sanitized(out, std::get<0>(p));
            out << '=';
            serialize_sanitized(out, std::get<1>(p));
        }
    }

    return out;
}
