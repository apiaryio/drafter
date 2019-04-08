//
//  utils/so/YamlIo.cc
//  librefract
//
//  Created by Thomas Jandecka on 07/02/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "YamlIo.h"

#include <algorithm>
#include <cassert>
#include <exception>
#include <iostream>
#include <iterator>
#include <mpark/variant.hpp>
#include "../Utf8.h"
#include "../Utils.h"

using namespace drafter;
using namespace utils;
using namespace so;

namespace
{
    using namespace utf8;

    bool is_alphanum_dash(const std::string& str)
    {
        for (char c : str)
            if (c < 'A' || c > 'z' || (c > 'Z' && c < 'a') || c == '-')
                return false;
        return true;
    }

    bool is_yaml_printable(const codepoint& c)
    {
        return (c == 0x9)                                  //
            || (c == 0xA)                                  //
            || (c == 0xD)                                  //
            || (0x20 <= c && c <= 0x7E)                    //
            || (c == 0x85)                                 //
            || (0xA0 <= c && c <= 0xD7FF)                  //
            || (0xE000 <= c && c <= 0xFFFD && c != 0xFEFF) //
            || (0x10000 <= c && c <= 0x10FFFF);
    }

    template <typename It>
    It serialize_escaped(codepoint c, It out)
    {
        if (c < 0x100) { // 8-bit
            char u_sym_buf[5];
            std::snprintf(u_sym_buf, 5, "\\x%02X", c);
            return std::copy(u_sym_buf, u_sym_buf + 4, out);
        } else if (c < 0x10000) { // 16-bit
            char u_sym_buf[7];
            std::snprintf(u_sym_buf, 7, "\\u%04X", c);
            return std::copy(u_sym_buf, u_sym_buf + 6, out);
        } else { // 32-bit
            char u_sym_buf[11];
            std::snprintf(u_sym_buf, 11, "\\U%08X", c);
            return std::copy(u_sym_buf, u_sym_buf + 10, out);
        }
    }

    template <typename It>
    It escape_sequence(char c, It out)
    {
        constexpr char ESCAPE = '\\';

        *out = ESCAPE;
        ++out;
        *out = c;
        ++out;
        return out;
    }

    template <typename ItI, typename ItO>
    ItO escape_yaml_string(ItI b, ItI e, ItO out)
    {
        using utf8_iterator = input_iterator<ItI>;

        utf8_iterator p{ b, e };
        const utf8_iterator end{ e, e };

        for (; p != end; ++p) {
            codepoint c = *p;
            switch (c) {
                case 0x0000: // null
                    out = escape_sequence('0', out);
                    break;
                case 0x0007: // bell
                    out = escape_sequence('a', out);
                    break;
                case 0x0008: // backspace
                    out = escape_sequence('b', out);
                    break;
                case 0x0009: // horizontal tab
                    out = escape_sequence('t', out);
                    break;
                case 0x000A: // line feed
                    out = escape_sequence('n', out);
                    break;
                case 0x000B: // vertical tab
                    out = escape_sequence('v', out);
                    break;
                case 0x000C: // form feed
                    out = escape_sequence('f', out);
                    break;
                case 0x000D: // carriage return
                    out = escape_sequence('r', out);
                    break;
                case 0x001B: // escape
                    out = escape_sequence('e', out);
                    break;
                // case 0x0020: // space
                //    out = escape_sequence('', out);
                //    break;
                case 0x0022: // double quote
                    out = escape_sequence('"', out);
                    break;
                // case 0x002F: // slash
                //    out = escape_sequence('/', out);
                //    break;
                case 0x005C: // back slash
                    out = escape_sequence('\\', out);
                    break;
                case 0x0085: // utf next line
                    out = escape_sequence('N', out);
                    break;
                case 0x00A0: // utf non-breaking space
                    out = escape_sequence('_', out);
                    break;
                case 0x2028: // utf line separator
                    out = escape_sequence('L', out);
                    break;
                case 0x2029: // utf paragraph separator
                    out = escape_sequence('P', out);
                    break;
                default: {
                    if (is_yaml_printable(c)) {
                        out = encode(c, out);
                    } else {
                        out = serialize_escaped(c, out);
                    }
                }
            }
        }
        return out;
    }

    std::ostream& serialize_yaml(std::ostream& out, const std::string& obj)
    {
        out << '"';
        escape_yaml_string( //
            obj.begin(),
            obj.end(),
            std::ostream_iterator<char>(out));
        out << '"';
        return out;
    }

    std::ostream& do_indent(std::ostream& out, int indent)
    {
        for (; indent > 0; --indent)
            out << "  ";
        return out;
    }

    struct yaml_printer final {
        std::ostream& out;
        const int indent;

        void operator()(const Null& value) const
        {
            if (indent > 0)
                out << ' ';

            out << "null";
        }

        void operator()(const True& value) const
        {
            if (indent > 0)
                out << ' ';

            out << "true";
        }

        void operator()(const False& value) const
        {
            if (indent > 0)
                out << ' ';

            out << "false";
        }

        void operator()(const String& value) const
        {
            if (indent > 0)
                out << ' ';

            serialize_yaml(out, value.data);
        }

        void operator()(const Number& value) const
        {
            if (indent > 0)
                out << ' ';

            out << value.data;
        }

        void operator()(const Object& value) const;
        void operator()(const Array& value) const;
    };

    void visit(const Value& obj, std::ostream& out, int indent = 0)
    {
        mpark::visit(yaml_printer{ out, indent }, obj);
    }

    void yaml_printer::operator()(const Object& value) const
    {
        if (value.data.empty()) {
            if (indent > 0)
                out << ' ';
            out << "{}";
            return;
        }

        if (indent > 0)
            out << '\n';

        int newlines = value.data.size() - 1;
        for (const auto& m : value.data) {
            do_indent(out, indent);

            // for clearer, unescaped reading
            if (is_alphanum_dash(m.first))
                out << m.first;
            else
                serialize_yaml(out, m.first);

            out << ":";

            visit(m.second, out, indent + 1);

            if (newlines > 0) {
                out << '\n';
                --newlines;
            }
        }
    }

    void yaml_printer::operator()(const Array& value) const
    {
        if (value.data.empty()) {
            if (indent > 0)
                out << ' ';
            out << "[]";
            return;
        }

        if (indent > 0)
            out << '\n';

        int newlines = value.data.size() - 1;
        for (const auto& m : value.data) {
            do_indent(out, indent);

            out << '-';

            visit(m, out, indent + 1);

            if (newlines > 0) {
                out << '\n';
                --newlines;
            }
        }
    }
} // namespace

std::ostream& so::serialize_yaml(std::ostream& out, const Value& obj)
{
    visit(obj, out);
    return out;
}
