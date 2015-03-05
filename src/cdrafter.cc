#include "cdrafter.h"

#include "snowcrash.h"
#include "sosJSON.h"

#include "SerializeAST.h"
#include "SerializeSourceAnnotations.h"

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
                           char** report, 
                           char** ast)
{

    std::stringstream inputStream;

    if (report) {
        options |= snowcrash::ExportSourcemapOption;
    } 

    inputStream << source;

    sc::ParseResult<sc::Blueprint> blueprint;
    sc::parse(inputStream.str(), options, blueprint);

    sos::SerializeJSON serializer;

    if (ast) {
        std::stringstream astStream;
        serializer.process(drafter::WrapBlueprint(blueprint.node), astStream);
        astStream << "\n";
        *ast = ToString(astStream);
    }


    if (report) {
        std::stringstream reportStream;
        serializer.process(drafter::WrapSourceAnnotations(blueprint.report, blueprint.sourceMap), reportStream);
        reportStream << "\n";
        *report = ToString(reportStream);
    }

    return blueprint.report.error.code;
}
