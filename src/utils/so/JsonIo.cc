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
        void operator()(const Null& value, std::ostream& out, int indent = 0) const
        {
            out << "null";
        }

        void operator()(const True& value, std::ostream& out, int indent = 0) const
        {
            out << "true";
        }

        void operator()(const False& value, std::ostream& out, int indent = 0) const
        {
            out << "false";
        }

        void operator()(const String& value, std::ostream& out, int indent = 0) const
        {
            out << '"';
            escape_json_string( //
                value.data.begin(),
                value.data.end(),
                std::ostream_iterator<char>(out));
            out << '"';
        }

        void operator()(const Number& value, std::ostream& out, int indent = 0) const
        {
            if (std::isfinite(value.data)) {     // Finite
                out << value.data;               //
            } else if (std::isnan(value.data)) { // NaN
                out << "null";                   //
            } else if (value.data < 0) {         // -Infinity
                out << "-1e+9999";               //
            } else {                             // +Infinity
                out << "1e+9999";                //
            }
        }

        void operator()(const Object& value, std::ostream& out, int indent = 0) const
        {
            out << '{';
            int commas = value.data.size() - 1;
            for (const auto& m : value.data) {
                if (!Packed)
                    break_indent(out, indent + 1);

                out << '"' << m.first << "\":";

                if (!Packed)
                    out << ' ';

                visit(m.second, *this, out, indent + 1);

                if (commas > 0) {
                    out << ',';
                    --commas;
                }
            }
            if (!(Packed || value.data.empty()))
                break_indent(out, indent);
            out << '}';
        }

        void operator()(const Array& value, std::ostream& out, int indent = 0) const
        {
            out << '[';
            int commas = value.data.size() - 1;
            for (const auto& m : value.data) {
                if (!Packed)
                    break_indent(out, indent + 1);
                visit(m, *this, out, indent + 1);

                if (commas > 0) {
                    out << ',';
                    --commas;
                }
            }
            if (!(Packed || value.data.empty()))
                break_indent(out, indent);
            out << ']';
        }
    };
}

std::ostream& so::serialize_json(std::ostream& out, const Value& obj)
{
    visit(obj, json_printer<false>{}, out);
    return out;
}

std::ostream& so::serialize_json(std::ostream& out, const Value& obj, packed)
{
    visit(obj, json_printer<true>{}, out);
    return out;
}
