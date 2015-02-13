#ifndef CONFIG_H_PIBRI8ND
#define CONFIG_H_PIBRI8ND

#include <string>


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

void ParseCommadLineOptions(int argc, const char *argv[], /** out */Config& conf);


#endif /* end of include guard: CONFIG_H_PIBRI8ND */
