//
//  utils/so/ValueIo.cc
//  librefract
//
//  Created by Thomas Jandecka on 23/01/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "JsonIo.h"

#include <algorithm>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <iterator>
#include <string>
#include <tuple>
#include <array>

using namespace drafter;
using namespace utils;
using namespace so;

namespace
{
    template <typename It>
    It json_utf_char(It out, unsigned int c)
    {
        char u_sym_buf[7];
        std::snprintf(u_sym_buf, 7, "\\u%04x", c);
        return std::copy( //
            u_sym_buf,
            u_sym_buf + 6,
            out);
    }

    template <typename ItI, typename ItO>
    ItO escape_json_string(ItI b, ItI e, ItO out)
    {
        for (; b != e; ++b) {

            char byte = *b;
            switch (byte) {
                case '\"':
                    *out = '\\';
                    ++out;
                    *out = '"';
                    ++out;
                    break;
                case '\\':
                    *out = '\\';
                    ++out;
                    *out = '\\';
                    ++out;
                    break;
                case '\b':
                    *out = '\\';
                    ++out;
                    *out = 'b';
                    ++out;
                    break;
                case '\f':
                    *out = '\\';
                    ++out;
                    *out = 'f';
                    ++out;
                    break;
                case '\n':
                    *out = '\\';
                    ++out;
                    *out = 'n';
                    ++out;
                    break;
                case '\r':
                    *out = '\\';
                    ++out;
                    *out = 'r';
                    ++out;
                    break;
                case '\t':
                    *out = '\\';
                    ++out;
                    *out = 't';
                    ++out;
                    break;
                default: {
                    // unsigned int representation
                    std::uint8_t c = static_cast<std::uint8_t>(byte);
                    if (c > 0x1f) { // non-control
                        *out = byte;
                        ++out;
                    } else { // escaped control sequences
                        out = json_utf_char(out, c);
                    }
                }
            }
        }
        return out;
    }

    std::ostream& break_indent(std::ostream& out, int indent)
    {
        out << '\n';
        for (; indent > 0; --indent)
            out << "  ";
        return out;
    }

    template <bool Packed = true>
    struct json_printer final {
        std::ostream& out;
        const int indent;

        void operator()(const Null& value) const
        {
            out << "null";
        }

        void operator()(const True& value) const
        {
            out << "true";
        }

        void operator()(const False& value) const
        {
            out << "false";
        }

        void operator()(const String& value) const
        {
            out << '"';
            escape_json_string( //
                value.data.begin(),
                value.data.end(),
                std::ostream_iterator<char>(out));
            out << '"';
        }

        void operator()(const Number& value) const
        {
            out << value.data;
        }

        void operator()(const Object& value) const;
        void operator()(const Array& value) const;
    };

    template <bool Packed>
    void visit(const Value& obj, std::ostream& out, int indent = 0)
    {
        mpark::visit(json_printer<Packed>{ out, indent }, obj);
    }

    template <bool Packed>
    void json_printer<Packed>::operator()(const Object& value) const
    {
        out << '{';
        int commas = value.data.size() - 1;
        for (const auto& m : value.data) {
            if (!Packed)
                break_indent(out, indent + 1);

            out << '"';
            escape_json_string(m.first.begin(), m.first.end(), std::ostream_iterator<char>(out));
            out << "\":";

            if (!Packed)
                out << ' ';

            visit<Packed>(m.second, out, indent + 1);

            if (commas > 0) {
                out << ',';
                --commas;
            }
        }
        if (!(Packed || value.data.empty()))
            break_indent(out, indent);
        out << '}';
    }

    template <bool Packed>
    void json_printer<Packed>::operator()(const Array& value) const
    {
        out << '[';
        int commas = value.data.size() - 1;
        for (const auto& m : value.data) {
            if (!Packed)
                break_indent(out, indent + 1);
            visit<Packed>(m, out, indent + 1);

            if (commas > 0) {
                out << ',';
                --commas;
            }
        }
        if (!(Packed || value.data.empty()))
            break_indent(out, indent);
        out << ']';
    }
} // namespace

std::ostream& so::serialize_json(std::ostream& out, const Value& obj)
{
    visit<false>(obj, out);
    return out;
}

std::ostream& so::serialize_json(std::ostream& out, const Value& obj, packed)
{
    visit<true>(obj, out);
    return out;
}
