//
//  cdrafter.h
//  drafter
//
//  C Implementation of drafter lib for binding purposes
//
//  Created by Jiri Kratochvil on 27-02-2015
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef SC_C_DRAFTER_H
#define SC_C_DRAFTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Platform.h" // use Platform.h from snowcrash - we should probably move it to drafter

/**
 *  \This is C interface for drafter parser.
 *
 *  \param source        A textual source data to be parsed.
 *  \param options       Parser options. Use 0 for no addtional options.
 *  \param result        parse result with ast, source map and annotations
 *
 *  for more info about Result format JSON \see https://github.com/apiaryio/api-blueprint-ast/blob/master/Parse%20Result.md
 *
 *  \return Error status code. Zero represents success, non-zero a failure.
 *
 *  This function will allocate memory and returns pointer to
 *  resul. Free allocated memory is responsibility of calling function.
 *  You have to release it by calling standard free() function
 *
 *  if `result` input is NULL output is not created for param and parsed `source` is just validated
 */

typedef unsigned int sc_blueprint_parser_options;

/** brief Blueprint Parser Options Enums */
enum sc_blueprint_parser_option {
    SC_RENDER_DESCRIPTIONS_OPTION = (1 << 0),       /// < Render Markdown in description.
    SC_REQUIRE_BLUEPRINT_NAME_OPTION = (1 << 1),    /// < Treat missing blueprint name as error
    SC_EXPORT_SORUCEMAP_OPTION = (1 << 2)           /// < Export source maps AST
};

/** brief Drafter AST Type Option Enum */
enum drafter_ast_type_option {
    DRAFTER_NORMAL_AST_TYPE = 0,      /// < Normal AST
    DRAFTER_REFRACT_AST_TYPE = 1      /// < Refract AST
};

SC_API int drafter_c_parse(const char* source,
                           sc_blueprint_parser_options options,
                           enum drafter_ast_type_option astType,
                           char** result);

#ifdef __cplusplus
}
#endif

#endif

