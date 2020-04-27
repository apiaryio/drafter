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

    using NewLinesIndex = std::vector<size_t>;

    /** structure contains starting and ending position of a error/warning. */
    struct AnnotationPosition {
        size_t fromLine = 0;
        size_t fromColumn = 0;
        size_t toLine = 0;
        size_t toColumn = 0;
    };

    /**
     *  \brief Convert character index mapping to line and column number
     *  \return out Position of the given range as output
     *  \param linesEndIndex Vector containing indexes of end line characters
     *  \param range Character index mapping as input
     */
    const AnnotationPosition GetLineFromMap(const NewLinesIndex& linesEndIndex, const mdp::Range& range);

    /**
     *  \brief Given the source returns the length of all the lines in source as a vector
     *  \param source Source data
     *  \param out Vector containing indexes of all end line character in source
     */
    const NewLinesIndex GetLinesEndIndex(const std::string& source);

} // namespace drafter

#endif
