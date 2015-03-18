//
//  SerializeResult.cc
//  drafter
//
//  Created by Jiri Kratochvil on 27-02-2015
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "SerializeResult.h"
#include "SerializeSourcemap.h"
#include "SerializeAST.h"

#include "SourceAnnotation.h"

#include "SectionProcessor.h"
#include "Blueprint.h"

#include <stdio.h>

using namespace drafter;

static sos::Object WrapLocation(const mdp::BytesRange& range)
{
    sos::Object location;

    location.set(SerializeKey::AnnotationLocationIndex, sos::Number(range.location));
    location.set(SerializeKey::AnnotationLocationLength, sos::Number(range.length));

    return location;
}

static sos::Object WrapAnnotation(const snowcrash::SourceAnnotation& annotation)
{
    sos::Object object;

    object.set(SerializeKey::AnnotationCode,     sos::Number(annotation.code));
    object.set(SerializeKey::AnnotationMessage,  sos::String(annotation.message));
    object.set(SerializeKey::AnnotationLocation, WrapCollection<mdp::BytesRange>()(annotation.location, WrapLocation));

    return object;
}

sos::Object drafter::WrapResult(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint, const snowcrash::BlueprintParserOptions options)
{
    sos::Object object;

    using namespace snowcrash;

    const Report& report = blueprint.report;

    object.set(SerializeKey::Version, sos::String(PARSE_RESULT_SERIALIZATION_VERSION));
    
    object.set(SerializeKey::Ast, WrapBlueprint(blueprint.node));

    if (options & ExportSourcemapOption) {
        const SourceMap<Blueprint>& sourceMap = blueprint.sourceMap;
        object.set(SerializeKey::SourceMap, WrapBlueprintSourcemap(sourceMap));
    }

    object.set(SerializeKey::Error, WrapAnnotation(report.error));

    if (!report.warnings.empty()) {
        object.set(SerializeKey::Warnings, WrapCollection<snowcrash::SourceAnnotation>()(report.warnings, WrapAnnotation));
    }

    return object;
}
