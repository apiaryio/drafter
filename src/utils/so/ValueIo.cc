//
//  utils/so/ValueIo.cc
//  librefract
//
//  Created by Thomas Jandecka on 23/01/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "ValueIo.h"

#include <iostream>
#include <algorithm>
#include <iterator>

using namespace drafter;
using namespace utils;
using namespace so;

namespace
{
    template <typename ItI, typename ItO, typename IsEscaped, typename EscT = char>
    ItO escape(ItI b, ItI e, ItO out, IsEscaped&& is_escaped, EscT escaper = '\\')
    {
        for (; b != e; ++b) {
            if (is_escaped(*b)) {
                out = escaper;
                ++out;
            }
            out = *b;
            ++out;
        }
        return b;
    }

    template <typename NamedType>
    struct printer final {

        void operator()(const Null& value, std::ostream& out) const
        {
            out << "null";
        }

        void operator()(const True& value, std::ostream& out) const
        {
            out << "true";
        }

        void operator()(const False& value, std::ostream& out) const
        {
            out << "false";
        }

        void operator()(const String& value, std::ostream& out) const
        {
            out << '"' << value.data << '"'; // TODO XXX escape
        }

        void operator()(const Number& value, std::ostream& out) const
        {
            out << value.data; // TODO XXX probably wrong
        }

        void operator()(const Object& value, std::ostream& out) const
        {
            out << '{';
            int commas = value.data.size() - 1;
            for (const auto& m : value.data) {
                out << '"' << m.first << "\":";
                out << NamedType{ m.second };

                if (commas > 0) {
                    out << ',';
                    --commas;
                }
            }
            out << '}';
        }

        void operator()(const Array& value, std::ostream& out) const
        {
            out << '[';

            std::transform( //
                value.data.begin(),
                value.data.end() - 1,
                std::ostream_iterator<NamedType>{ out, "," },
                [&out](const Value& member) { //
                    return NamedType{ member };
                });

            out << NamedType{ value.data.back() };

            out << ']';
        }
    };
}

std::ostream& so::operator<<(std::ostream& out, const JsonValue& obj)
{
    visit(obj.value, printer<JsonValue>{}, out);
    return out;
}

std::ostream& so::operator<<(std::ostream& out, const YamlValue& obj)
{
    assert(false);
    return out;
}
