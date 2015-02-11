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

namespace config {
    static const std::string Output         = "output";
    static const std::string Format         = "format";
    static const std::string Render         = "render";
    static const std::string Sourcemap      = "sourcemap";
    static const std::string Validate       = "validate";
    static const std::string Version        = "version";
    static const std::string UseLineNumbers = "use-line-num";
};

struct Config {
};

void ParseCommadLineOptions(int argc, const char *argv[], /* out */Config& config)
{
}

//using sc = snowcrash;

int main(int argc, const char *argv[])
{
    Config config; 
    ParseCommadLineOptions(argc, argv, config);

#if 0
    sc::ParseResult<sc::Blueprint> blueprint;
    sc::parse(inputStream.str(), options, blueprint);

    if (!config.ValidateOnly) {
    }

    //PrintReport(blueprint.report, inputStream.str(), isUseLineNumbers);
    return blueprint.report.error.code;
#endif

}
