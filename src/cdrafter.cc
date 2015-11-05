//
//  cdrafter.cc
//  drafter
//
//  C Implementation of drafter lib for binding purposes
//
//  Created by Jiri Kratochvil on 27-02-2015
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

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
                           enum drafter_ast_type_option astType,
                           char** result)
{

    std::stringstream inputStream;

    inputStream << source;

    sc::ParseResult<sc::Blueprint> blueprint;
    sc::parse(inputStream.str(), options, blueprint);

    sos::SerializeJSON serializer;

    if (result) {
        std::stringstream resultStream;
        drafter::WrapperOptions wrapperOptions(drafter::ASTType(astType), options & SC_EXPORT_SORUCEMAP_OPTION);

        try {
            serializer.process(drafter::WrapResult(blueprint, wrapperOptions), resultStream);
        }
        catch (std::exception& e) {
            blueprint.report.error.message = e.what();
            blueprint.report.error.code = snowcrash::ApplicationError;
        }
        resultStream << "\n";
        *result = ToString(resultStream);
    }

    return blueprint.report.error.code;
}
