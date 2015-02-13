#ifndef REPORTING_H_JHA2MUHM
#define REPORTING_H_JHA2MUHM

//
// vi:cin:et:sw=4 ts=4
//
//  reporting.h - part of drafter
//
//  Created by Jiri Kratochvil on 2015-02-13
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
//
//

#include "SourceAnnotation.h"

/**
 *  \brief Print parser report to stderr.
 *
 *  \param report A parser report to print
 *  \param source Source data
 *  \param isUseLineNumbers True if the annotations needs to be printed by line and column number
 */

void PrintReport(const snowcrash::Report& report,
                 const std::string& source,
                 const bool isUseLineNumbers);


#endif /* end of include guard: REPORTING_H_JHA2MUHM */
