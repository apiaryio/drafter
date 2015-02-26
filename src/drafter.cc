// vi:cin:et:sw=4 ts=4
//
//  drafter.cc
//
//  Created by Jiri Kratochvil on 2015-02-11
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <tr1/memory>
#include <tr1/shared_ptr.h>

#include "snowcrash.h"
#include "SectionParserData.h"  // snowcrash::BlueprintParserOptions

#include "sos.h"
#include "sosJSON.h"
#include "sosYAML.h"

#include "SerializeAST.h"
#include "SerializeSourcemap.h"

#include "reporting.h"
#include "config.h"

namespace sc = snowcrash;

/**
 *  \brief deleter functor to shared_ptr<> which does nothing
 *  usefull to get system resource like std::cin, std::cout to shared_ptr<>
 */
template<typename T>
struct dummy_deleter {
    void operator()(T* obj) const 
    {
      // do nothing
    }
};

/**
 *  \brief functor returns cin/cout ptr in based on istream or ostream
 */
template<typename T> struct std_io;

template<> struct std_io<std::istream> {
    std::istream* operator()() const
    { 
        return &std::cin; 
    }
};

template<> struct std_io<std::ostream> {
    std::ostream* operator()() const
    { 
        return &std::cout; 
    }
};

/**
 *  \brief functor returns appropriate cin/cout based on istream/ostream
 */
template<typename T> struct std_io_selector {
    typedef T stream_type;
    typedef std::tr1::shared_ptr<stream_type> return_type;

    return_type operator()() const
    { 
        return return_type(std_io<T>()(), dummy_deleter<stream_type>()); 
    }
};

/**
 *  \brief functor select fstream based on iostream
 */
template <typename T> struct to_fstream;

template<> 
struct to_fstream<std::istream>{
  typedef std::ifstream stream_type;
};

template<> 
struct to_fstream<std::ostream>{
  typedef std::ofstream stream_type;
};

/**
 *  \brief functor select return appropriate opened fstream based on type of stream
 */
template<typename T> struct fstream_io_selector{
    typedef typename to_fstream<T>::stream_type stream_type;
    typedef std::tr1::shared_ptr<stream_type> return_type;

    return_type operator()(const char* name) const 
    { 
        return return_type(new stream_type(name)); 
    }
};

/**
 *  \brief return writable stream or report error and exit()
 *
 *  return is based on \template param T (must be std::ostream or std::istream)
 *
 *  For std::istream it return opened std::ifstream with filename as in \param `file`
 *  if \param `file` is empty it will return std::cin
 *
 *  In similar way it work for std::ostream
 *
 *  \param template<T> type of returned stream
 *  \param file - name of file to open for read/write if empty use standart input/output
 *
 */
template<typename T>
std::tr1::shared_ptr<T> CreateStreamFromName(const std::string& file)
{
    if (file.empty()) {
        return std_io_selector<T>()();
    }

    typedef typename fstream_io_selector<T>::return_type return_type;

    return_type stream = fstream_io_selector<T>()(file.c_str());

    if (!stream->is_open()) {
      std::cerr << "fatal: unable to open file '" << file << "'\n";
      exit(EXIT_FAILURE);
    }

    return stream;
}

/**
 *  \brief  return instance sos::Serializer based on \param `format`
 *
 *  \param format - output format for serialization
 */

sos::Serialize* CreateSerializer(const std::string& format)
{
    if (format == "json") {
        return new sos::SerializeJSON;
    } else if (format == "yaml") {
        return new sos::SerializeYAML;
    }

    std::cerr << "fatal: unknow serialization format: '" << format << "'\n";
    exit(EXIT_FAILURE);
}

/**
 * \brief Serialize sos::Object into stream
 */
void Serialization(std::tr1::shared_ptr<std::ostream> stream,
                   const sos::Object& object,
                   sos::Serialize* serializer)
{
    serializer->process(object, *stream);
    *stream << "\n";
}


int main(int argc, const char *argv[])
{
    Config config; 
    ParseCommadLineOptions(argc, argv, config);

    sc::BlueprintParserOptions options = 0;  // Or snowcrash::RequireBlueprintNameOption
    if (!config.sourceMap.empty()) {
        options |= snowcrash::ExportSourcemapOption;
    }

    std::stringstream inputStream;
    std::tr1::shared_ptr<std::istream> in = CreateStreamFromName<std::istream>(config.input);
    inputStream << in->rdbuf();

    sc::ParseResult<sc::Blueprint> blueprint;
    sc::parse(inputStream.str(), options, blueprint);

    if (!config.validate) {  // not just validate -> we will serialize
        sos::Serialize* serializer = CreateSerializer(config.format);

        Serialization(CreateStreamFromName<std::ostream>(config.output),
                      drafter::WrapBlueprint(blueprint.node), 
                      serializer);

        Serialization(CreateStreamFromName<std::ostream>(config.sourceMap),
                      drafter::WrapBlueprintSourcemap(blueprint.sourceMap),
                      serializer);

        delete serializer;
    }

    PrintReport(blueprint.report, inputStream.str(), config.lineNumbers);

    return blueprint.report.error.code;

}
