//
//  ByteBuffer.h
//  markdownparser
//
//  Created by Zdenek Nemec on 4/18/14.
//  Copyright (c) 2014 Apiary Inc. All rights reserved.
//

#ifndef MARKDOWNPARSER_BYTEBUFFER_H
#define MARKDOWNPARSER_BYTEBUFFER_H

#include <string>
#include <vector>
#include <sstream>

namespace mdp
{

    /**
     *  \brief Source data byte buffer
     *
     *  Note this is a byte buffer, a sequence of
     *  UTF8 bytes note necessarily characters.
     */
    typedef std::string ByteBuffer;

    /** Byte buffer stream */
    typedef std::stringstream ByteBufferStream;

    /** A generic continuous range */
    struct Range {
        size_t location = 0;
        size_t length = 0;

        Range() = default;
        Range(size_t loc, size_t len) : location(loc), length(len) {}
    };

    /** Range of bytes */
    using BytesRange = Range;

    /** Range of characters */
    using CharactersRange = Range;

    /** A generic set of non-continuous of ranges */
    template <typename T>
    using RangeSet = std::vector<T>;

    /** Set of non-continuous byte ranges */
    typedef RangeSet<BytesRange> BytesRangeSet;

    /** Set of non-continuous character ranges */
    typedef RangeSet<CharactersRange> CharactersRangeSet;

    /** Map byte index into utf-8 chracter index */
    typedef std::vector<size_t> ByteBufferCharacterIndex;

    /** Fill character map - cache of characters positions */
    void BuildCharacterIndex(ByteBufferCharacterIndex& index, const ByteBuffer& byteBuffer);

    /** Convert ranges of bytes to ranges of characters */
    CharactersRangeSet BytesRangeSetToCharactersRangeSet(const BytesRangeSet& rangeSet, const ByteBuffer& byteBuffer);
    CharactersRangeSet BytesRangeSetToCharactersRangeSet(
        const BytesRangeSet& rangeSet, const ByteBufferCharacterIndex& index);

    /** Maps bytes range set to byte buffer */
    ByteBuffer MapBytesRangeSet(const BytesRangeSet& rangeSet, const ByteBuffer& byteBuffer);

    /** Append another range set to this one, merging continuous blocks */
    void mergeContinuous(RangeSet<Range>& lhs, const RangeSet<Range>& rhs);
}

#endif
