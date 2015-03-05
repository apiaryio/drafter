//
//  cdrafter.h
//  drafter
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
 *  \param report        parse result with source map and 
 *  \param ast           parsed source as blueprint AST in JSON format
 *
 *  \return Error status code. Zero represents success, non-zero a failure.
 *
 *  This function will allocate memory and returns pointers to 
 *  ast and sourceMap you have to free allocated memory 
 *  by calling standard free() function
 *
 *  Report should be free by calling drafter_report_free
 *
 *  if any of `report`, `ast`, or `sourceMap` input is NULL 
 *  output is not created for param
 */

typedef unsigned int sc_blueprint_parser_options;

/** brief Blueprint Parser Options Enums */
enum sc_blueprint_parser_option {
    SC_RENDER_DESCRIPTIONS_OPTION = (1 << 0),       /// < Render Markdown in description.
    SC_REQUIRE_BLUEPRINT_NAME_OPTION = (1 << 1),    /// < Treat missing blueprint name as error
    SC_EXPORT_SORUCEMAP_OPTION = (1 << 2)           /// < Export source maps AST
};

SC_API int drafter_c_parse(const char* source, 
                           sc_blueprint_parser_options option, 
                           char** report, 
                           char** ast);

#ifdef __cplusplus
}
#endif

#endif

