//
// vi:cin:et:sw=4 ts=4
//
//  config.cc - part of drafter
//
//  Created by Jiri Kratochvil on 2015-02-13
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "config.h"
#include "cmdline.h"

#include "Version.h"

namespace config {
    static const std::string Program        = "drafter";

    static const std::string Output         = "output";
    static const std::string Format         = "format";
    static const std::string Type           = "type";
    static const std::string Render         = "render";
    static const std::string Sourcemap      = "sourcemap";
    static const std::string Validate       = "validate";
    static const std::string Version        = "version";
    static const std::string UseLineNumbers = "use-line-num";
};

void PrepareCommanLineParser(cmdline::parser& parser)
{
    parser.set_program_name(config::Program);

    parser.add<std::string>(config::Output,    'o', "save output AST into file", false);
    parser.add<std::string>(config::Format,    'f', "output format of the AST (yaml|json)", false, "yaml", cmdline::oneof<std::string>("yaml", "json"));
    parser.add<std::string>(config::Type,      't', "type of the AST (refract|ast)", false, "refract", cmdline::oneof<std::string>("refract", "ast"));
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

    if (parser.exist(config::Validate)) {
        if (parser.exist(config::Sourcemap) || parser.exist(config::Output)) {
            std::cerr << "WARN: While validation is enabled, source map and output files will not be created" << std::endl;
        }
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
    conf.astType     = parser.get<std::string>(config::Type);
    conf.output      = parser.get<std::string>(config::Output);
    conf.sourceMap   = parser.get<std::string>(config::Sourcemap);
}
