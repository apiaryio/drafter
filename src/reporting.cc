//
//  reporting.cc
//  drafter
//
//  Created by Jiri Kratochvil on 2015-02-13
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "reporting.h"

#include <algorithm>
#include <iostream>

namespace sc = snowcrash;

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
void GetLineFromMap(const std::vector<size_t>& linesEndIndex,
                    const mdp::Range& range,
                    AnnotationPosition& out)
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
    annotationPositionIt = std::lower_bound(linesEndIndex.begin(), linesEndIndex.end(), range.location + range.length) - 1;

    if (annotationPositionIt != linesEndIndex.end()) {

        out.toLine = std::distance(linesEndIndex.begin(), annotationPositionIt) + 1;
        out.toColumn = (range.location + range.length) - *annotationPositionIt + 1;

        if (*(annotationPositionIt + 1) == (range.location + range.length)) {
            out.toColumn--;
        }
    }
}

/**
 *  \brief Given the source returns the length of all the lines in source as a vector
 *  \param source Source data
 *  \param out Vector containing indexes of all end line character in source
 */
void GetLinesEndIndex(const std::string& source,
                      std::vector<size_t>& out)
{

    out.push_back(0);

    for (size_t i = 0; i < source.length(); i++) {

        if (source[i] == '\n') {
            out.push_back(i + 1);
        }
    }
}

/**
 *  \brief Print Markdown source annotation.
 *  \param prefix A string prefix for the annotation
 *  \param annotation An annotation to print
 *  \param source Source data
 *  \param isUseLineNumbers True if the annotations needs to be printed by line and column number
 */
void PrintAnnotation(const std::string& prefix,
                     const snowcrash::SourceAnnotation& annotation,
                     const std::string& source,
                     const bool isUseLineNumbers)
{

    std::cerr << prefix;

    if (annotation.code != sc::SourceAnnotation::OK) {
        std::cerr << " (" << annotation.code << ") ";
    }

    if (!annotation.message.empty()) {
        std::cerr << " " << annotation.message;
    }

    std::vector<size_t> linesEndIndex;

    if (isUseLineNumbers) {
        GetLinesEndIndex(source, linesEndIndex);
    }

    if (!annotation.location.empty()) {

        for (mdp::CharactersRangeSet::const_iterator it = annotation.location.begin();
             it != annotation.location.end();
             ++it) {

            if (isUseLineNumbers) {

                AnnotationPosition annotationPosition;
                GetLineFromMap(linesEndIndex, *it, annotationPosition);

                std::cerr << "; line " << annotationPosition.fromLine << ", column " << annotationPosition.fromColumn;
                std::cerr << " - line " << annotationPosition.toLine << ", column " << annotationPosition.toColumn;
            }
            else {

                std::cerr << ((it == annotation.location.begin()) ? " :" : ";");
                std::cerr << it->location << ":" << it->length;
            }
        }
    }

    std::cerr << std::endl;
}

/**
 *  \brief Print parser report to stderr.
 *  \param report A parser report to print
 *  \param source Source data
 *  \param isUseLineNumbers True if the annotations needs to be printed by line and column number
 */
void PrintReport(const snowcrash::Report& report,
                 const std::string& source,
                 const bool isUseLineNumbers)
{

    std::cerr << std::endl;

    if (report.error.code == sc::Error::OK) {
        std::cerr << "OK.\n";
    }
    else {
        PrintAnnotation("error:", report.error, source, isUseLineNumbers);
    }

    for (snowcrash::Warnings::const_iterator it = report.warnings.begin(); it != report.warnings.end(); ++it) {
        PrintAnnotation("warning:", *it, source, isUseLineNumbers);
    }
}
