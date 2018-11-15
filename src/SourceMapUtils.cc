#include "SourceMapUtils.h"
#include <algorithm>

#include "utils/Utf8.h"

#include <iostream>

namespace drafter
{

    const AnnotationPosition GetLineFromMap(const NewLinesIndex& linesEndIndex, const mdp::Range& range)
    {
        // Finds starting line and column position
        AnnotationPosition out;
        if (linesEndIndex.empty()) { // to avoid std::prev() on empty
            return out;
        }

        auto annotationPositionIt = std::upper_bound(linesEndIndex.begin(), linesEndIndex.end(), range.location);

        if (annotationPositionIt != linesEndIndex.end()) {
            out.fromLine = std::distance(linesEndIndex.begin(), annotationPositionIt);
            out.fromColumn = range.location - *std::prev(annotationPositionIt) + 1;
        }

        // Finds ending line and column position
        annotationPositionIt
            = std::lower_bound(linesEndIndex.begin(), linesEndIndex.end(), range.location + range.length);

        //
        // FIXME: workaround for byte mapping
        // handle just case when position is after latest newline
        // remove once all sourceMaps will correctly send character ranges
        //

        if (linesEndIndex.back() < (range.location + range.length)) {
            out.toLine = linesEndIndex.size();
            out.toColumn = 1;
            return out;
        }

        // end of byte mapping workarround

        if (annotationPositionIt != linesEndIndex.end()) {
            out.toLine = std::distance(linesEndIndex.begin(), annotationPositionIt);
            out.toColumn = (range.location + range.length) - *(std::prev(annotationPositionIt));
        }

        return out;
    }

    const NewLinesIndex GetLinesEndIndex(const std::string& source)
    {

        NewLinesIndex out;

        out.push_back(0);

        utils::utf8::input_iterator<std::string::const_iterator> it(source);
        utils::utf8::input_iterator<std::string::const_iterator> e{ source.end(), source.end() };

        int i = 1;
        for (; it != e; ++it, ++i) {
            if (*it == '\n')
                out.push_back(i);
        }

        return out;
    }

} // namespace drafter
