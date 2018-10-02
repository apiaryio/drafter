//
//  SourceMapUtils.h
//  librafter
//
//  Created by Jiri Kratochvil on 2018-10-02
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_SOURCEMAPUTILS_H
#define DRAFTER_SOURCEMAPUTILS_H

#include "SourceAnnotation.h"

namespace drafter
{

    typedef std::vector<size_t> NewLinesIndex;

    /** structure contains starting and ending position of a error/warning. */
    struct AnnotationPosition {
        size_t fromLine;
        size_t fromColumn;
        size_t toLine;
        size_t toColumn;
    };

    /**
     *  \brief Convert character index mapping to line and column number
     *  \param linesEndIndex Vector containing indexes of end line characters
     *  \param range Character index mapping as input
     *  \param out Position of the given range as output
     */
    void GetLineFromMap(const std::vector<size_t>& linesEndIndex, const mdp::Range& range, AnnotationPosition& out);

    /**
     *  \brief Given the source returns the length of all the lines in source as a vector
     *  \param source Source data
     *  \param out Vector containing indexes of all end line character in source
     */
    void GetLinesEndIndex(const std::string& source, NewLinesIndex& out);

} // namespace drafter

#endif
