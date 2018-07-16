// vi:cin:et:sw=4 ts=4
//
//  drafter.cc
//
//  Created by Jiri Kratochvil on 2015-02-11
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
//
#include "drafter.h"

#include "snowcrash.h"
#include "SectionParserData.h" // snowcrash::BlueprintParserOptions

#include "SerializeResult.h"

#include "reporting.h"
#include "config.h"
#include "stream.h"

#include "ConversionContext.h"

#include "utils/log/Trivial.h"

namespace sc = snowcrash;

int ProcessRefract(const Config& config, std::unique_ptr<std::istream>& in, std::unique_ptr<std::ostream>& out)
{
    if (config.enableLog)
        ENABLE_LOGGING;

    std::stringstream inputStream;
    inputStream << in->rdbuf();

    drafter_serialize_options options;
    options.sourcemap = config.sourceMap;
    options.format = config.format == drafter::YAMLFormat ? DRAFTER_SERIALIZE_YAML : DRAFTER_SERIALIZE_JSON;

    refract::IElement* result = nullptr;

    // TODO: Read parse options from CLI
    drafter_parse_options parseOptions = { false };

    int ret = drafter_parse_blueprint(inputStream.str().c_str(), &result, parseOptions);

    if (!result) {
        return -1;
    }

    if (!config.validate) { // If not validate, we serialize
        char* output = drafter_serialize(result, options);

        if (output) {
            *out << output << "\n" << std::flush;

            free(output);
        }
    }

    PrintReport(result, inputStream.str(), config.lineNumbers, ret);

    drafter_free_result(result);

    return ret;
}

int main(int argc, const char* argv[])
{
    Config config;
    ParseCommadLineOptions(argc, argv, config);

    std::unique_ptr<std::istream> in(CreateStreamFromName<std::istream>(config.input));
    std::unique_ptr<std::ostream> out(CreateStreamFromName<std::ostream>(config.output));

    return ProcessRefract(config, in, out);
}
