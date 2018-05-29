//
//  ModelTable.h
//  snowcrash
//
//  Created by Zdenek Nemec on 6/9/13.
//  Copyright (c) 2013 Apiary Inc. All rights reserved.
//

#ifndef SNOWCRASH_MODELTABLE_H
#define SNOWCRASH_MODELTABLE_H

#include <string>
#include <map>
#include "ByteBuffer.h"
#include "RegexMatch.h"

#include "BlueprintSourcemap.h"
#include "StringUtility.h"

// Symbol identifier regex
#define SYMBOL_IDENTIFIER "([^][()]+)"

namespace snowcrash
{

    /** Symbol reference matching regex */
    const char* const ModelReferenceRegex("^[[:blank:]]*\\[" SYMBOL_IDENTIFIER "]\\[][[:blank:]]*$");

    // Resource Object Model Table
    typedef std::map<Identifier, ResourceModel> ModelTable;

    // Resource Object Model Table source map
    typedef std::map<Identifier, SourceMap<ResourceModel> > ModelSourceMapTable;

    // Checks whether given source data represents reference to a symbol returning true if so,
    // false otherwise. If source data is represent reference referred symbol name is filled in.
    inline bool GetModelReference(const mdp::ByteBuffer& sourceData, Identifier& referredModel)
    {

        CaptureGroups captureGroups;

        if (RegexCapture(sourceData, ModelReferenceRegex, captureGroups, 3)) {
            referredModel = captureGroups[1];
            TrimString(referredModel);
            return true;
        }

        return false;
    }
}

#endif
