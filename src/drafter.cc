// vi:cin:et:sw=4 ts=4
//
//  drafter.cc
//
//  Created by Jiri Kratochvil on 2015-02-11
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
//
//

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <tr1/memory>
#include <tr1/shared_ptr.h>

#include "cmdline.h"

#include "snowcrash.h"
#include "SectionParserData.h"  // snowcrash::BlueprintParserOptions

#include "sos.h"
#include "sosJSON.h"
#include "sosYAML.h"

#include "SerializeAST.h"
#include "SerializeSourcemap.h"

namespace sc = snowcrash;

namespace config {
    static const std::string Program        = "drafter";

    static const std::string Output         = "output";
    static const std::string Format         = "format";
    static const std::string Render         = "render";
    static const std::string Sourcemap      = "sourcemap";
    static const std::string Validate       = "validate";
    static const std::string Version        = "version";
    static const std::string UseLineNumbers = "use-line-num";
};

struct Config {
    std::string input;
    bool lineNumbers;
    bool validate;
    std::string format;
    std::string sourceMap;
    std::string output;
};

void PrepareCommanLineParser(cmdline::parser& parser)
{
    parser.set_program_name(config::Program);

    parser.add<std::string>(config::Output,    'o', "save output AST into file", false);
    parser.add<std::string>(config::Format,    'f', "output AST format", false, "yaml", cmdline::oneof<std::string>("yaml", "json"));
    parser.add<std::string>(config::Sourcemap, 's', "export sourcemap AST into file", false);
    parser.add("help",                         'h', "display this help message");
    parser.add(config::Version ,               'v', "print Drafter version");
    parser.add(config::Validate,               'l', "validate input only, do not print AST");
    parser.add(config::UseLineNumbers ,        'u', "use line and row number instead of character index when printing annotation");

    std::stringstream ss;

    ss << "<input file>\n\n";
    ss << "API Blueprint Parser\n";
    ss << "If called without <input file>, 'drafter' will listen on stdin.\n";

    parser.footer(ss.str());
}

#define DRAFTER_VERSION_STRING "x.x.x"

void ValidateParsedCommandLine(const cmdline::parser& parser)
{
    if (parser.rest().size() > 1) {
        std::cerr << "one input file expected, got " << parser.rest().size() << std::endl;
        exit(EXIT_FAILURE);
    }

    if (parser.exist(config::Version)) {
        std::cout << DRAFTER_VERSION_STRING << std::endl;
        exit(EXIT_SUCCESS);
    }

}

void ParseCommadLineOptions(int argc, const char *argv[], /* out */Config& conf)
{
    cmdline::parser parser;
    PrepareCommanLineParser(parser);

    parser.parse_check(argc, argv);

    ValidateParsedCommandLine(parser);

    if (!parser.rest().empty()) {
        conf.input = parser.rest().front();
    }

    conf.lineNumbers = parser.exist(config::UseLineNumbers);
    conf.validate    = parser.exist(config::Validate);
    conf.format      = parser.get<std::string>(config::Format);
    conf.output      = parser.get<std::string>(config::Output);
    conf.sourceMap   = parser.get<std::string>(config::Sourcemap);
}

template<typename T>
struct dummy_deleter {
    void operator()(T* obj) const {
      // do nothing
    }
};

template<typename T> struct std_io_selector;

template<> 
struct std_io_selector<std::ostream>{
    std::ostream* operator()() { return &std::cout; }
};

template<> 
struct std_io_selector<std::istream>{
    std::istream* operator()() { return &std::cin; }
};

template <typename Stream> struct fstream_selector;

template<> 
struct fstream_selector<std::istream>{
  typedef std::ifstream stream_type;
};

template<> 
struct fstream_selector<std::ostream>{
  typedef std::ofstream stream_type;
};



template<typename T> struct file_io_selector;

template<> 
struct file_io_selector<std::ofstream>{
    std::ofstream* operator()(const char* name) { return new std::ofstream(name); }
};

template<> 
struct file_io_selector<std::ifstream>{
    std::ifstream* operator()(const char* name) { return new std::ifstream(name); }
};

template<typename T>
std::tr1::shared_ptr<T> CreateStreamFromName(const std::string& file)
{
    if(file.empty()) {
        return std::tr1::shared_ptr<T>(
            std_io_selector<T>()(), 
            dummy_deleter<T>()
        );
    }

    typedef typename fstream_selector<T>::stream_type stream_type;
    std::tr1::shared_ptr<stream_type>stream(
        file_io_selector<stream_type>()(file.c_str())
    );
    stream->open(file.c_str());

    if (!stream->is_open()) {
      std::cerr << "fatal: unable to open file '" << file << "'\n";
      exit(EXIT_FAILURE);
    }

    return stream;
}

void Serialization(const std::string& out, 
    const sos::Object& object, 
    sos::Serialize* serializer) 
{
    std::tr1::shared_ptr<std::ostream> stream = CreateStreamFromName<std::ostream>(out);
    serializer->process(object, *stream);
    *stream << std::endl;
}

sos::Serialize* CreateSerializer(const std::string& format)
{
    if(format == "json") {
        return new sos::SerializeJSON;
    } else if(format == "yaml") {
        return new sos::SerializeYAML;
    }

    std::cerr << "fatal: unknow serialization format: '" << format << "'\n";
    exit(EXIT_FAILURE);
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


int main(int argc, const char *argv[])
{
    Config config; 
    ParseCommadLineOptions(argc, argv, config);

    sc::BlueprintParserOptions options = 0;  // Or snowcrash::RequireBlueprintNameOption
    if(!config.sourceMap.empty()) {
        options |= snowcrash::ExportSourcemapOption;
    }

    std::stringstream inputStream;
    std::tr1::shared_ptr<std::istream> in = CreateStreamFromName<std::istream>(config.input);
    inputStream << in->rdbuf();

    sc::ParseResult<sc::Blueprint> blueprint;
    sc::parse(inputStream.str(), options, blueprint);

    if (!config.validate) {
        sos::Serialize* serializer = CreateSerializer(config.format);

        Serialization(config.output, 
            snowcrash::WrapBlueprint(blueprint.node), 
            serializer
            );

        Serialization(config.sourceMap, 
            snowcrash::WrapBlueprintSourcemap(blueprint.sourceMap),
            serializer
            );

        delete serializer;
    }

    PrintReport(blueprint.report, inputStream.str(), config.lineNumbers);

    return blueprint.report.error.code;

}
