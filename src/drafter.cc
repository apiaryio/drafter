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

template <typename Stream> struct to_fstream;

template<> 
struct to_fstream<std::istream>{
  typedef std::ifstream stream_type;
};

template<> 
struct to_fstream<std::ostream>{
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

    typedef typename to_fstream<T>::stream_type stream_type;
    std::tr1::shared_ptr<stream_type>stream(
        file_io_selector<stream_type>()(file.c_str())
    );

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
