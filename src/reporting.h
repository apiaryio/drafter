//
//  reporting.h
//  drafter
//
//  Created by Jiri Kratochvil on 2015-02-13
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_REPORTING_H
#define DRAFTER_REPORTING_H

#include "drafter.h"
#include "SourceAnnotation.h"

/**
 *  \brief Print parser report to stderr.
 *
 *  \param report A parser report to print
 *  \param source Source data
 *  \param useLineNumbers True if the annotations needs to be printed by line and column number
 */
void PrintReport(const snowcrash::Report& report,
                 const std::string& source,
                 const bool useLineNumbers);

/**
 *  \brief Print parser report to stderr.
 *
 *  \param report A parser report to print
 *  \param source Source data
 *  \param useLineNumbers True if the annotations needs to be printed by line and column number
 *  \param error - code form parsing
 */
void PrintReport(const drafter_result*,
                 const std::string& source,
                 const bool useLineNumbers,
                 const int error);

#endif // #ifndef DRAFTER_REPORTING_H
