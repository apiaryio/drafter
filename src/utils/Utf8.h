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

namespace drafter
{
    namespace utils
    {
        namespace utf8
        {
            using codepoint = std::uint32_t;

            template <typename It>
            It serialize(codepoint c, It out)
            {
                using test_whether_iterator = typename It::iterator_category;
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

            template <typename It>
            class input_iterator
            {
                It p_;
                It e_;

            public:
                constexpr static codepoint replacement = 0xFFFD;

            public:
                using value_type = codepoint;
                using reference = const codepoint&;
                using pointer = const codepoint*;
                using iterator_category = std::input_iterator_tag;
                using difference_type = typename It::difference_type;

            public:
                template <typename ItT>
                constexpr input_iterator(ItT&& b, ItT&& e) noexcept : p_(std::forward<ItT>(b)), e_(std::forward<ItT>(e))
                {
                }

                template <typename Container>
                explicit constexpr input_iterator(const Container& e) noexcept : p_(e.begin()), e_(e.end())
                {
                }

                input_iterator(const input_iterator& other) = default;
                input_iterator(input_iterator&& other) = default;
                input_iterator& operator=(const input_iterator& other) = default;
                input_iterator& operator=(input_iterator&& other) = default;

                input_iterator() noexcept = default;

                friend void swap(input_iterator& lhs, input_iterator& rhs)
                {
                    using std::swap;
                    swap(lhs.p_, rhs.p_);
                    swap(lhs.e_, rhs.e_);
                }

                friend bool operator==(const input_iterator& lhs, const input_iterator& rhs)
                {
                    return lhs.p_ == rhs.p_;
                }

                friend bool operator!=(const input_iterator& lhs, const input_iterator& rhs)
                {
                    return !(lhs == rhs);
                }

            public:
                input_iterator& operator++()
                {
                    assert(p_ != e_);
                    codepoint first_byte = static_cast<unsigned char>(*p_);

                    if (first_byte < 0x80) {
                        ++p_;
                    } else if (first_byte < 0xE0) {
                        ++p_;
                        ++p_;
                    } else if (first_byte < 0xF0) {
                        ++p_;
                        ++p_;
                        ++p_;
                    } else if (first_byte < 0xF8) {
                        ++p_;
                        ++p_;
                        ++p_;
                        ++p_;
                    } else {
                        // invalid utf-8, increment just a byte (dereference yields replacement)
                        ++p_;
                    }

                    if (e_ < p_)
                        p_ = e_;

                    return *this;
                }

                input_iterator operator++(int)
                {
                    auto result = *this;
                    ++(*this);
                    return result;
                }

                value_type operator*() const
                {
                    assert(p_ != e_);

                    codepoint first_byte = 0;
                    first_byte |= reinterpret_cast<const unsigned char&>(*p_);

                    if (first_byte < 0x80) // single byte
                        return first_byte;

                    else if (first_byte < 0xE0) { // two bytes
                        if (e_ - p_ < 2)
                            return replacement; // expected more!

                        codepoint result = ((first_byte & 0x1F) << 6) //
                            | (static_cast<unsigned char>(p_[1]) & 0x3F);

                        return result < 0x80 ? replacement : result; // check for oversized

                    } else if (first_byte < 0xF0) { // three bytes
                        if (e_ - p_ < 3)
                            return replacement; // expected more!

                        codepoint result = ((first_byte & 0x0F) << 12)          //
                            | ((static_cast<unsigned char>(p_[1]) & 0x3F) << 6) //
                            | (static_cast<unsigned char>(p_[2]) & 0x3F);

                        if (result >= 0xD800 && result <= 0xDFFF) // surrogates not codepoints
                            return replacement;

                        return result < 0x800 ? replacement : result; // check for oversized

                    } else if (first_byte < 0xF8) { // four bytes
                        if (e_ - p_ < 4)
                            return replacement; // expected more!

                        codepoint result = ((first_byte & 0x07) << 18)           //
                            | ((static_cast<unsigned char>(p_[1]) & 0x3F) << 12) //
                            | ((static_cast<unsigned char>(p_[2]) & 0x3F) << 6)  //
                            | (static_cast<unsigned char>(p_[3]) & 0x3F);

                        return result < 0x1000 ? replacement : result; // check for oversized
                    }

                    // else invalid utf-8
                    return replacement;
                }

                pointer operator->() const
                {
                    return &**this;
                }
            };
        }
    }
}
#endif
