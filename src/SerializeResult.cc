//
//  SerializeResult.cc
//  drafter
//
//  Created by Jiri Kratochvil on 27-02-2015
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "RefractParseResult.h"
#include "RefractAPI.h"
#include "RefractDataStructure.h"

#include "SerializeResult.h"
#include "SerializeSourcemap.h"
#include "SerializeAST.h"

using namespace drafter;

sos::Object WrapLocation(const mdp::BytesRange& range)
{
    sos::Object location;

    location.set(SerializeKey::AnnotationLocationIndex, sos::Number(range.location));
    location.set(SerializeKey::AnnotationLocationLength, sos::Number(range.length));

    return location;
}

sos::Object drafter::WrapAnnotation(const snowcrash::SourceAnnotation& annotation)
{
    sos::Object object;

    object.set(SerializeKey::AnnotationCode,     sos::Number(annotation.code));
    object.set(SerializeKey::AnnotationMessage,  sos::String(annotation.message));
    object.set(SerializeKey::AnnotationLocation, WrapCollection<mdp::BytesRange>()(annotation.location, WrapLocation));

    return object;
}

sos::Object WrapParseResultAST(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint,
                               const snowcrash::BlueprintParserOptions options)
{
    sos::Object object;

    object.set(SerializeKey::Version, sos::String(PARSE_RESULT_SERIALIZATION_VERSION));

    object.set(SerializeKey::Ast, WrapBlueprint(blueprint.node, drafter::NormalASTType));

    if (options & snowcrash::ExportSourcemapOption) {
        object.set(SerializeKey::Sourcemap, WrapBlueprintSourcemap(blueprint.sourceMap));
    }

    object.set(SerializeKey::Error, WrapAnnotation(blueprint.report.error));
    object.set(SerializeKey::Warnings, WrapCollection<snowcrash::SourceAnnotation>()(blueprint.report.warnings, WrapAnnotation));

    return object;
}

sos::Object WrapParseResultRefract(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint,
                                   const snowcrash::BlueprintParserOptions options)
{
    refract::IElement* element = ParseResultToRefract(blueprint);
    sos::Object object = SerializeRefract(element);

    if (element) {
        delete element;
    }

    return object;
}

sos::Object drafter::WrapResult(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint,
                                const snowcrash::BlueprintParserOptions options,
                                const ASTType astType)
{
    sos::Object object;
    snowcrash::Error error;

    try {
        RegisterNamedTypes(blueprint.node.content.elements());

        if (astType == RefractASTType) {
            object = WrapParseResultRefract(blueprint, options);
        }
        else {
            object = WrapParseResultAST(blueprint, options);
        }
    }
    catch (std::exception& e) {
        error = snowcrash::Error(e.what(), snowcrash::MSONError);
    }
    catch (snowcrash::Error& e) {
        error = e;
    }

    GetNamedTypesRegistry().clearAll(true);

    if (error.code != snowcrash::Error::OK) {
        throw error;
    }

    return object;
}

sos::Object drafter::WrapParseResult(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint,
                                     const snowcrash::BlueprintParserOptions options)
{
    return WrapResult(blueprint, options, drafter::RefractASTType);
}