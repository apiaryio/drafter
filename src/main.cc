// vi:cin:et:sw=4 ts=4
//
//  drafter.cc
//
//  Created by Jiri Kratochvil on 2015-02-11
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "snowcrash.h"
#include "SectionParserData.h"  // snowcrash::BlueprintParserOptions

#include "sos.h"
#include "sosJSON.h"
#include "sosYAML.h"

#include "SerializeResult.h"

#include "reporting.h"
#include "config.h"
#include "stream.h"

namespace sc = snowcrash;

/**
 *  \brief  return instance sos::Serializer based on \param `format`
 *
 *  \param format - output format for serialization
 */
sos::Serialize* CreateSerializer(const drafter::SerializeFormat& format)
{
    if (format == drafter::JSONFormat) {
        return new sos::SerializeJSON;
    }

    return new sos::SerializeYAML;
}

/**
 * \brief Serialize sos::Object into stream
 */
void Serialization(std::ostream *stream,
                   const sos::Object& object,
                   sos::Serialize* serializer)
{
    serializer->process(object, *stream);
    *stream << "\n";
    *stream << std::flush;
}

int main(int argc, const char *argv[])
{
    Config config;
    ParseCommadLineOptions(argc, argv, config);

    sc::BlueprintParserOptions options = 0;  // Or snowcrash::RequireBlueprintNameOption

    if (config.sourceMap) {
        options |= snowcrash::ExportSourcemapOption;
    }

    std::stringstream inputStream;
    std::auto_ptr<std::istream> in(CreateStreamFromName<std::istream>(config.input));
    inputStream << in->rdbuf();

    sc::ParseResult<sc::Blueprint> blueprint;
    sc::parse(inputStream.str(), options, blueprint);

    sos::Serialize* serializer = CreateSerializer(config.format);
    std::ostream *out = CreateStreamFromName<std::ostream>(config.output);

    try {
        sos::Object resultObject = drafter::WrapResult(blueprint, drafter::WrapperOptions(config.astType, config.sourceMap));

        if (!config.validate) { // If not validate, we serialize
            Serialization(out, resultObject, serializer);
        }
    }
    catch (snowcrash::Error& e) {
        blueprint.report.error = e;
    }
    catch (std::exception& e) {
        blueprint.report.error.message = e.what();
        blueprint.report.error.code = snowcrash::ApplicationError;
    }

    delete out;
    delete serializer;

    PrintReport(blueprint.report, inputStream.str(), config.lineNumbers);

    return blueprint.report.error.code;
}
