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

#include "refract/Element.h"
#include "refract/FilterVisitor.h"
#include "refract/Query.h"
#include "refract/Iterate.h"
#include "refract/TypeQueryVisitor.h"

#include "refract/VisitorUtils.h"

namespace sc = snowcrash;

using namespace refract;

namespace
{
    std::ostream& operator<<(std::ostream& out, const dsd::String& obj)
    {
        out << '"';
        out << obj.get();
        out << '"';
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const dsd::Number& obj)
    {
        out << obj.get();
        return out;
    }

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

    /**
     *  \brief Given the source returns the length of all the lines in source as a vector
     *  \param source Source data
     *  \param out Vector containing indexes of all end line character in source
     */
    void GetLinesEndIndex(const std::string& source, std::vector<size_t>& out)
    {

        out.push_back(0);

        for (size_t i = 0; i < source.length(); i++) {

            if (source[i] == '\n') {
                out.push_back(i + 1);
            }
        }
    }

    void PrintAnnotation(const std::string& prefix,
        const snowcrash::SourceAnnotation& annotation,
        const std::string& source,
        const bool useLineNumbers)
    {

        std::cerr << prefix;

        if (annotation.code != sc::SourceAnnotation::OK) {
            std::cerr << " (" << annotation.code << ") ";
        }

        if (!annotation.message.empty()) {
            std::cerr << " " << annotation.message;
        }

        std::vector<size_t> linesEndIndex;

        if (useLineNumbers) {
            GetLinesEndIndex(source, linesEndIndex);
        }

        if (!annotation.location.empty()) {

            for (mdp::CharactersRangeSet::const_iterator it = annotation.location.begin();
                 it != annotation.location.end();
                 ++it) {

                if (useLineNumbers) {

                    AnnotationPosition annotationPosition;
                    GetLineFromMap(linesEndIndex, *it, annotationPosition);

                    std::cerr << "; line " << annotationPosition.fromLine << ", column "
                              << annotationPosition.fromColumn;
                    std::cerr << " - line " << annotationPosition.toLine << ", column " << annotationPosition.toColumn;
                } else {

                    std::cerr << ((it == annotation.location.begin()) ? " :" : ";");
                    std::cerr << it->location << ":" << it->length;
                }
            }
        }

        std::cerr << std::endl;
    }

    struct AnnotationToString {

        std::vector<size_t> linesEndIndex;
        const bool useLineNumbers;

        AnnotationToString(const std::string& source, const bool useLineNumbers) : useLineNumbers(useLineNumbers)
        {
            if (useLineNumbers) {
                GetLinesEndIndex(source, linesEndIndex);
            }
        }

        const std::string location(const IElement* sourceMap)
        {
            std::stringstream output;
            auto map = TypeQueryVisitor::as<const ArrayElement>(sourceMap);
            if (map && map->get().size() == 2) {

                auto loc = TypeQueryVisitor::as<const NumberElement>(map->get().begin()[0].get());
                auto len = TypeQueryVisitor::as<const NumberElement>(map->get().begin()[1].get());
                if (loc && len) {

                    if (useLineNumbers) {

                        AnnotationPosition annotationPosition;
                        mdp::Range pos(loc->get(), len->get());
                        GetLineFromMap(linesEndIndex, pos, annotationPosition);

                        output << "; line " << annotationPosition.fromLine << ", column "
                               << annotationPosition.fromColumn;
                        output << " - line " << annotationPosition.toLine << ", column " << annotationPosition.toColumn;
                    } else {
                        output << loc->get() << ":" << len->get();
                    }
                }
            }
            return output.str();
        }

        const std::string operator()(const IElement* annotation)
        {
            std::stringstream output;

            if (!annotation || annotation->element() != "annotation") {
                return output.str();
            }

            if (auto classes = FindCollectionMemberValue<ArrayElement>(annotation->meta(), "classes")) {
                if (classes->get().size() == 1) {
                    if (auto type = TypeQueryVisitor::as<const StringElement>(classes->get().begin()[0].get())) {
                        output << type->get() << ": ";
                    }
                }
            };

            if (const NumberElement* code
                = FindCollectionMemberValue<NumberElement>(annotation->attributes(), "code")) {
                output << "(" << code->get() << ")  ";
            }

            if (const StringElement* message = TypeQueryVisitor::as<StringElement>(annotation)) {
                output << message->get();
            }

            if (const ArrayElement* sourceMap
                = FindCollectionMemberValue<ArrayElement>(annotation->attributes(), "sourceMap")) {
                if (sourceMap->get().size() == 1) {
                    sourceMap = TypeQueryVisitor::as<const ArrayElement>(sourceMap->get().begin()[0].get());
                    if (sourceMap) {
                        for (const auto& array : sourceMap->get()) {
                            if (!useLineNumbers) {
                                const char* prefix = array == (*sourceMap->get().begin()) ? " :" : ";";
                                output << prefix;
                            }
                            output << location(array.get());
                        }
                    }
                }
            };

            return output.str();
        }
    };
}

/**
 *  \brief Print parser report to stderr.
 *  \param report A parser report to print
 *  \param source Source data
 *  \param isUseLineNumbers True if the annotations needs to be printed by line and column number
 */
void PrintReport(const snowcrash::Report& report, const std::string& source, const bool isUseLineNumbers)
{

    std::cerr << std::endl;

    if (report.error.code == sc::Error::OK) {
        std::cerr << "OK.\n";
    } else {
        PrintAnnotation("error:", report.error, source, isUseLineNumbers);
    }

    for (snowcrash::Warnings::const_iterator it = report.warnings.begin(); it != report.warnings.end(); ++it) {
        PrintAnnotation("warning:", *it, source, isUseLineNumbers);
    }
}

void PrintReport(const drafter_result* result, const std::string& source, const bool useLineNumbers, const int error)
{
    std::cerr << std::endl;

    FilterVisitor filter(query::Element("annotation"));
    Iterate<Children> iterate(filter);
    iterate(*result);

    if (error == sc::Error::OK) {
        std::cerr << "OK.\n";
    }

    std::transform(filter.elements().begin(),
        filter.elements().end(),
        std::ostream_iterator<std::string>(std::cerr, "\n"),
        AnnotationToString(source, useLineNumbers));
}
