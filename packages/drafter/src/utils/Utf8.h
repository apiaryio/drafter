//
//  utils/Utf8.h
//  librefract
//
//  Created by Thomas Jandecka on 09/02/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_UTILS_UTF8_H
#define DRAFTER_UTILS_UTF8_H

#include <cstdint>
#include <cassert>
#include <iterator>
#include <utility>
#include "Utils.h"

namespace drafter
{
    namespace utils
    {
        namespace utf8
        {

            using codepoint = std::uint32_t;

            constexpr codepoint replacement = 0xFFFD;
            constexpr codepoint invalid = 0xFFFFFFFF;

            ///
            /// Decode utf8 codepoint from octet sequence
            ///
            /// @param  first   iterator to begin of octet sequence to be decoded from
            /// @param  last    iterator to end of octet sequence to be decoded from
            ///
            /// @returns    iff first equals last, then (0xFFFFFFFF, last) ; else
            ///             iff invalid utf8 sequence, then (0xFFFD, iterator to begin of next Utf8 character)
            ///             otherwise (codepoint, iterator to begin of next Utf8 character)
            ///
            template <typename It>
            typename std::enable_if<is_iterator<It>::value, std::pair<codepoint, It> >::type //
            decode_one(It first, It last)
            {
                if (first == last)
                    return { invalid, last };

                codepoint cp = 0;
                cp |= reinterpret_cast<const unsigned char&>(*first);

                if (cp < 0x80) { // single byte
                    std::advance(first, 1);

                    return { cp, first };
                } else if (cp < 0xE0) { // two bytes
                    if (last - first < 2) {
                        return { replacement, last }; // expected more!
                    }

                    codepoint result = ((cp & 0x1F) << 6) //
                        | (static_cast<unsigned char>(first[1]) & 0x3F);
                    std::advance(first, 2);

                    return { result < 0x80 ? replacement : result, first }; // check for oversized

                } else if (cp < 0xF0) { // three bytes
                    if (last - first < 3) {
                        return { replacement, last }; // expected more!
                    }

                    codepoint result = ((cp & 0x0F) << 12)                     //
                        | ((static_cast<unsigned char>(first[1]) & 0x3F) << 6) //
                        | (static_cast<unsigned char>(first[2]) & 0x3F);
                    std::advance(first, 3);

                    if (result >= 0xD800 && result <= 0xDFFF) // surrogates not codepoints
                        return { replacement, first };

                    return { result < 0x800 ? replacement : result, first }; // check for oversized

                } else { // four bytes
                    if (last - first < 4)
                        return { replacement, last }; // expected more!

                    codepoint result = ((cp & 0x07) << 18)                      //
                        | ((static_cast<unsigned char>(first[1]) & 0x3F) << 12) //
                        | ((static_cast<unsigned char>(first[2]) & 0x3F) << 6)  //
                        | (static_cast<unsigned char>(first[3]) & 0x3F);
                    std::advance(first, 4);

                    // check for oversized
                    if (result < 0x10000)
                        return { replacement, first };

                    // check for overlong sequences
                    if (result > 0x10FFFF)
                        return { replacement, first };

                    return { result, first };
                }
            }

            ///
            /// Encode utf8 codepoint to octet sequence
            ///
            /// @param  c   utf8 codepoint to be serialized
            /// @param  out iterator to start of output sequence
            ///
            /// @returns    iterator to after where last octet was written to
            ///
            template <typename It>
            typename std::enable_if<is_iterator<It>::value, It>::type //
            encode(codepoint c, It out)
            {
                using byte = const std::uint8_t;

                if (c < 0x80) { // single byte
                    byte b1 = c;
                    *out = reinterpret_cast<const char&>(b1);
                    ++out;

                } else if (c < 0x800) { // two bytes
                    {
                        byte b1 = ((c >> 6) & 0b00011111) | 0b11000000;
                        *out = reinterpret_cast<const char&>(b1);
                        ++out;
                    }

                    {
                        byte b2 = (c & 0b00111111) | 0b10000000;
                        *out = reinterpret_cast<const char&>(b2);
                        ++out;
                    }

                } else if (c < 0x10000) { // three bytes
                    {
                        byte b1 = ((c >> 12) & 0b00001111) | 0b11100000;
                        *out = reinterpret_cast<const char&>(b1);
                        ++out;
                    }

                    {
                        byte b2 = ((c >> 6) & 0b00111111) | 0b10000000;
                        *out = reinterpret_cast<const char&>(b2);
                        ++out;
                    }

                    {
                        byte b3 = (c & 0b00111111) | 0b10000000;
                        *out = reinterpret_cast<const char&>(b3);
                        ++out;
                    }

                } else if (c < 0x110000) { // four bytes
                    {
                        byte b1 = ((c >> 18) & 0b00000111) | 0b11110000;
                        *out = reinterpret_cast<const char&>(b1);
                        ++out;
                    }

                    {
                        byte b2 = ((c >> 12) & 0b00111111) | 0b10000000;
                        *out = reinterpret_cast<const char&>(b2);
                        ++out;
                    }

                    {
                        byte b3 = ((c >> 6) & 0b00111111) | 0b10000000;
                        *out = reinterpret_cast<const char&>(b3);
                        ++out;
                    }

                    {
                        byte b4 = (c & 0b00111111) | 0b10000000;
                        *out = reinterpret_cast<const char&>(b4);
                        ++out;
                    }

                } else {
                    assert(false); // invalid codepoint
                }
                return out;
            }

            ///
            /// Validating utf8 input iterator defined above
            ///     a octet sequence with iterator It
            ///
            /// OPTIM: naively buffers current codepoint; benchmark a version
            ///     without buffering
            ///
            template <typename It>
            class input_iterator
            {

            private:
                It e_;
                std::pair<codepoint, It> cp_and_next_;

            public:
                using value_type = codepoint;
                using reference = const codepoint&;
                using pointer = const codepoint*;
                using iterator_category = std::input_iterator_tag;
                using difference_type = typename It::difference_type;

            public:
                template <typename ItT>
                constexpr input_iterator(ItT&& first, ItT&& last) noexcept //
                    : e_(std::forward<ItT>(last)),
                      cp_and_next_(decode_one(first, last))
                {
                }

                template <typename Container>
                explicit constexpr input_iterator(const Container& last) noexcept //
                    : e_(last.end()),
                      cp_and_next_(decode_one(last.begin(), last.end()))
                {
                }

                input_iterator(const input_iterator& other) = default;
                input_iterator(input_iterator&& other) = default;
                input_iterator& operator=(const input_iterator& other) = default;
                input_iterator& operator=(input_iterator&& other) = default;

                friend void swap(input_iterator& lhs, input_iterator& rhs)
                {
                    using std::swap;
                    swap(lhs.e_, rhs.e_);
                    swap(lhs.cp_and_next_, rhs.cp_and_next_);
                }

                friend bool operator==(const input_iterator& lhs, const input_iterator& rhs)
                {
                    return lhs.cp_and_next_ == rhs.cp_and_next_;
                }

                friend bool operator!=(const input_iterator& lhs, const input_iterator& rhs)
                {
                    return !(lhs == rhs);
                }

            public:
                input_iterator& operator++()
                {
                    cp_and_next_ = decode_one(cp_and_next_.second, e_);
                    return *this;
                }

                input_iterator operator++(int)
                {
                    auto result = *this;
                    ++(*this);
                    return result;
                }

                reference operator*() const noexcept
                {
                    return cp_and_next_.first;
                }

                pointer operator->() const
                {
                    return &cp_and_next_.first;
                }
            };
        }
    }
}
#endif
