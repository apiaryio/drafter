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

sos::Object drafter::WrapParseResult(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint,
                                     const snowcrash::BlueprintParserOptions options)
{
    sos::Object object;
    sos::Array content;

    object.set("element", sos::String("parseResult"));

    content.push(WrapBlueprint(blueprint.node, RefractASTType));
    object.set("content", content);

    return object;
}

sos::Object drafter::WrapResult(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint,
                                const snowcrash::BlueprintParserOptions options,
                                const ASTType astType)
{
    if (astType != NormalASTType) {
        return WrapParseResult(blueprint, options);
    }

    sos::Object object;

    object.set(SerializeKey::Version, sos::String(PARSE_RESULT_SERIALIZATION_VERSION));

    object.set(SerializeKey::Ast, WrapBlueprint(blueprint.node, astType));

    if (options & snowcrash::ExportSourcemapOption) {
        const snowcrash::SourceMap<snowcrash::Blueprint>& sourceMap = blueprint.sourceMap;
        object.set(SerializeKey::SourceMap, WrapBlueprintSourcemap(sourceMap));
    }

    object.set(SerializeKey::Error, WrapAnnotation(blueprint.report.error));

    if (!blueprint.report.warnings.empty()) {
        object.set(SerializeKey::Warnings, WrapCollection<snowcrash::SourceAnnotation>()(blueprint.report.warnings, WrapAnnotation));
    }

    return object;
}
