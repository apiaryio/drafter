#include "cdrafter.h"

#include "snowcrash.h"
#include "sosJSON.h"

#include "SerializeAST.h"
#include "SerializeSourcemap.h"
#include "SerializeResult.h"

#include <string.h>

namespace sc = snowcrash;

static char* ToString(const std::stringstream& stream) 
{
    size_t length = stream.str().length() + 1;
    char* str = (char*)malloc(length);
    memcpy(str, stream.str().c_str(), length);
    return str;
}

SC_API int drafter_c_parse(const char* source, 
                           sc_blueprint_parser_options options, 
                           char** result) 
{

    std::stringstream inputStream;

    inputStream << source;

    sc::ParseResult<sc::Blueprint> blueprint;
    sc::parse(inputStream.str(), options, blueprint);

    sos::SerializeJSON serializer;

    if (result) {
        std::stringstream resultStream;
        serializer.process(drafter::WrapResult(blueprint, options), resultStream);
        resultStream << "\n";
        *result = ToString(resultStream);
    }

    return blueprint.report.error.code;
}
