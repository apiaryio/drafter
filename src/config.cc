#include "config.h"
#include "cmdline.h"

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
