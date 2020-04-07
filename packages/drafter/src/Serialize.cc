//
//  Serialize.cc
//  drafter
//
//  Created by Zdenek Nemec on 5/3/13.
//  Copyright (c) 2013 Apiary Inc. All rights reserved.
//

#include "Serialize.h"
#include "StringUtility.h"

#include <cstdlib>

using namespace drafter;
using namespace refract;

template <>
std::pair<bool, dsd::Boolean> drafter::LiteralTo<dsd::Boolean>(const mson::Literal& literal)
{
    if (literal == "true" || literal == "false") {
        return std::make_pair(true, dsd::Boolean{ literal == SerializeKey::True });
    }
    return std::make_pair(false, dsd::Boolean{});
}

namespace
{
    ///
    /// Regular expression matching a ECMA-404 number.
    ///
    /// > A number is a sequence of decimal digits with no superfluous
    /// > leading zero. It may have a preceding minus sign (U+002D).
    /// > It may have a fractional part prefixed by a decimal point (U+002E).
    /// > It may have an exponent, prefixed by e (U+0065) or E (U+0045) and
    /// > optionally + (U+002B) or – (U+002D) . The digits are the code points
    /// > U+0030 through U+0039.
    ///
    // clang-format off
    // const std::regex json_number_expression(R"REGEX(-?(?:0|[1-9]\d*)(?:\.\d+)?(?:[eE][+-]?\d+)?)REGEX");
    // clang-format on

    template <typename It, typename Predicate>
    It many(It begin, It end, Predicate predicate)
    {
        for (; begin != end; ++begin)
            if (!predicate(*begin))
                break;
        return begin;
    }

    template <typename It, typename Predicate>
    It optional(It begin, It end, Predicate predicate)
    {
        if (begin != end && predicate(*begin))
            ++begin;
        return begin;
    }

    template <typename It>
    bool isValidNumber(It b, It e)
    {
        // >
        // > It may have a preceding minus sign (U+002D).
        static_assert(0x2D == '-', "");
        b = optional(b, e, [](char c) { return c == '-'; });

        // >
        // > The digits are the code points U+0030 through U+0039.
        static_assert(0x30 == '0', "");
        static_assert(0x39 == '9', "");
        const auto is_digit = [](char c) -> bool { return c >= '0' && c <= '9'; };

        // >
        // > A number is a sequence of decimal digits with no superfluous
        // > leading zero.
        if (b == e)
            return false;

        if (*b == '0') {
            ++b;
            if (b == e)
                return true;
        } else {
            if (!is_digit(*b))
                return false;

            b = many(b, e, is_digit);
            if (b == e)
                return true;
        }

        // >
        // > It may have a fractional part prefixed by a decimal point (U+002E).
        static_assert(0x2E == '.', "");
        if (*b == '.') {
            ++b;
            if (b == e)
                return false;

            if (!is_digit(*b))
                return false;

            b = many(b, e, is_digit);
            if (b == e)
                return true;
        }

        // >
        // > It may have an exponent, prefixed by e (U+0065) or E (U+0045) and
        // > optionally + (U+002B) or – (U+002D)
        static_assert(0x65 == 'e', "");
        static_assert(0x45 == 'E', "");
        if (*b == 'e' || *b == 'E') {
            ++b;

            static_assert(0x2B == '+', "");
            static_assert(0x2D == '-', "");
            b = optional(b, e, [](char c) { return c == '+' || c == '-'; });
            if (b == e)
                return false;

            if (!is_digit(*b))
                return false;

            b = many(b, e, is_digit);
            if (b == e)
                return true;
        }

        return false;
    }
} // namespace

template <>
std::pair<bool, dsd::Number> drafter::LiteralTo<dsd::Number>(const mson::Literal& literal)
{
    using std::begin;
    using std::end;

    // ignore spaces to the right
    auto match_end = end(literal);
    for (; match_end != begin(literal); --match_end) {
        if (!std::isspace(*(match_end - 1)))
            break;
    }

    if (isValidNumber(begin(literal), match_end)) {
        return { true, dsd::Number{ std::string(begin(literal), match_end) } };
    }

    return { false, dsd::Number{} };
}

template <>
std::pair<bool, dsd::String> drafter::LiteralTo<dsd::String>(const mson::Literal& literal)
{
    return std::make_pair(!literal.empty(), dsd::String{ literal });
}
