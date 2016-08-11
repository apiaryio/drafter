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
#include "SectionParserData.h"  // snowcrash::BlueprintParserOptions

#include "sos.h"
#include "sosJSON.h"
#include "sosYAML.h"

#include "SerializeResult.h"

#include "reporting.h"
#include "config.h"
#include "stream.h"

#include "ConversionContext.h"

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

int ProcessAST(const Config& config, std::unique_ptr<std::istream>& in, std::unique_ptr<std::ostream>& out)
{
    std::stringstream inputStream;
    inputStream << in->rdbuf();

    sc::ParseResult<sc::Blueprint> blueprint;
    sc::parse(inputStream.str(), snowcrash::ExportSourcemapOption, blueprint);

    std::unique_ptr<sos::Serialize> serializer(CreateSerializer(config.format));

    try {
        sos::Object resultObject = drafter::WrapResult(blueprint, drafter::WrapperOptions(config.astType, config.sourceMap));

        if (!config.validate) { // If not validate, we serialize
            Serialization(out.get(), resultObject, serializer.get());
        }
    }
    catch (snowcrash::Error& e) {
        blueprint.report.error = e;
    }
    catch (std::exception& e) {
        blueprint.report.error = snowcrash::Error(e.what(), snowcrash::ApplicationError);
    }

    PrintReport(blueprint.report, inputStream.str(), config.lineNumbers);

    return blueprint.report.error.code;
}

int ProcessRefract(const Config& config, std::unique_ptr<std::istream>& in, std::unique_ptr<std::ostream>& out)
{
    std::stringstream inputStream;
    inputStream << in->rdbuf();

    drafter_options options;
    options.sourcemap = config.sourceMap;
    options.format = config.format == drafter::YAMLFormat
       ? DRAFTER_SERIALIZE_YAML
       : DRAFTER_SERIALIZE_JSON;

    refract::IElement* result = nullptr;

    int ret = drafter_parse_blueprint(inputStream.str().c_str(), &result);

    if (!result) {
        return -1;
    }

    if (!config.validate) { // If not validate, we serialize
        char* output = drafter_serialize(result, options);

        if (output) {
            *out << output
                 << "\n"
                 << std::flush;

            free(output);
        }
    }

    PrintReport(result, inputStream.str(), config.lineNumbers, ret);

    drafter_free_result(result);

    return ret;
}

int main(int argc, const char *argv[])
{
    Config config;
    ParseCommadLineOptions(argc, argv, config);

    std::unique_ptr<std::istream> in(CreateStreamFromName<std::istream>(config.input));
    std::unique_ptr<std::ostream> out(CreateStreamFromName<std::ostream>(config.output));

    if (config.astType == drafter::RefractASTType) {
        return ProcessRefract(config, in, out);
    }
    else if (config.astType == drafter::NormalASTType) {
        return ProcessAST(config, in, out);
    }


}
