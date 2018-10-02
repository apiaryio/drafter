#include "SourceMapUtils.h"
#include <algorithm>

namespace drafter
{

    void GetLineFromMap(const std::vector<size_t>& linesEndIndex, const mdp::Range& range, AnnotationPosition& out)
    {

        std::vector<size_t>::const_iterator annotationPositionIt;

        out.fromLine = 0;
        out.fromColumn = 0;
        out.toLine = 0;
        out.toColumn = 0;

        // Finds starting line and column position
        annotationPositionIt = std::upper_bound(linesEndIndex.begin(), linesEndIndex.end(), range.location) - 1;

        if (annotationPositionIt != linesEndIndex.end()) {

            out.fromLine = std::distance(linesEndIndex.begin(), annotationPositionIt) + 1;
            out.fromColumn = range.location - *annotationPositionIt + 1;
        }

        // Finds ending line and column position
        annotationPositionIt
            = std::lower_bound(linesEndIndex.begin(), linesEndIndex.end(), range.location + range.length) - 1;

        if (annotationPositionIt != linesEndIndex.end()) {

            out.toLine = std::distance(linesEndIndex.begin(), annotationPositionIt) + 1;
            out.toColumn = (range.location + range.length) - *annotationPositionIt + 1;

            if (*(annotationPositionIt + 1) == (range.location + range.length)) {
                out.toColumn--;
            }
        }
    }

    void GetLinesEndIndex(const std::string& source, std::vector<size_t>& out)
    {

        out.push_back(0);

        for (size_t i = 0; i < source.length(); i++) {

            if (source[i] == '\n') {
                out.push_back(i + 1);
            }
        }
    }

} // namespace drafter
