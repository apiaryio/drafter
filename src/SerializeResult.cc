//
//  SerializeResult.cc
//  drafter
//
//  Created by Jiri Kratochvil on 27-02-2015
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "RefractAPI.h"
#include "RefractDataStructure.h"

#include "SerializeResult.h"
#include "SerializeSourcemap.h"
#include "SerializeAST.h"

#include "SourceAnnotation.h"
#include "SectionProcessor.h"

#include "refract/Build.h"
#include "refract/ElementInserter.h"

#include "NamedTypesRegistry.h"
#include "ConversionContext.h"

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

sos::Object WrapParseResultAST(snowcrash::ParseResult<snowcrash::Blueprint>& blueprint,
                               ConversionContext& context)
{
    sos::Object object;
    snowcrash::Error error;

    object.set(SerializeKey::Version, sos::String(PARSE_RESULT_SERIALIZATION_VERSION));

    if (blueprint.report.error.code == snowcrash::Error::OK) {
        try {
            object.set(SerializeKey::Ast, WrapBlueprint(blueprint, context));

            if (context.options.generateSourceMap) {
                object.set(SerializeKey::Sourcemap, WrapBlueprintSourcemap(blueprint.sourceMap, context));
            }
        }
        catch (std::exception& e) {
            error = snowcrash::Error(e.what(), snowcrash::MSONError);
        }
        catch (snowcrash::Error& e) {
            error = e;
        }

        if (error.code != snowcrash::Error::OK) {
            blueprint.report.error = error;
        }
    }

    for (auto& warning : context.warnings) {
        blueprint.report.warnings.push_back(warning);
    }

    object.set(SerializeKey::Error, WrapAnnotation(blueprint.report.error));
    object.set(SerializeKey::Warnings, WrapCollection<snowcrash::SourceAnnotation>()(blueprint.report.warnings, WrapAnnotation));

    return object;
}


namespace helper {

    struct AnnotationToRefract {

        const std::string& key;

        AnnotationToRefract(const std::string& key) : key(key) {}

        refract::IElement* operator()(snowcrash::SourceAnnotation& annotation) {
            return drafter::AnnotationToRefract(annotation, key);
        }
    };
}

refract::IElement* drafter::WrapParseResultRefract(snowcrash::ParseResult<snowcrash::Blueprint>& blueprint,
                                                   ConversionContext& context)
{
    snowcrash::Error error;
    refract::IElement* blueprintRefract = NULL;

    refract::ArrayElement* parseResult = new refract::ArrayElement;
    parseResult->element(SerializeKey::ParseResult);

    if (blueprint.report.error.code == snowcrash::Error::OK) {
        try {
            RegisterNamedTypes(MakeNodeInfo(blueprint.node.content.elements(), blueprint.sourceMap.content.elements()), context);
            blueprintRefract = BlueprintToRefract(MakeNodeInfo(blueprint.node, blueprint.sourceMap), context);
        }
        catch (std::exception& e) {
            error = snowcrash::Error(e.what(), snowcrash::MSONError);
        }
        catch (snowcrash::Error& e) {
            error = e;
        }

        context.GetNamedTypesRegistry().clearAll(true);

        if (error.code != snowcrash::Error::OK) {
            blueprint.report.error = error;
        }

        if (blueprintRefract) {
            parseResult->push_back(blueprintRefract);
        }
    }

    if (blueprint.report.error.code != snowcrash::Error::OK) {
        parseResult->push_back(helper::AnnotationToRefract(SerializeKey::Error)(blueprint.report.error));
    }

    snowcrash::Warnings& warnings = blueprint.report.warnings;
    if (!context.warnings.empty()) {
        warnings.insert(warnings.end(), context.warnings.begin(), context.warnings.end());
    }

    if (!warnings.empty()) {
        std::transform(warnings.begin(), warnings.end(),
                       refract::ElementInserter(*parseResult),
                       helper::AnnotationToRefract(SerializeKey::Warning));
    }

    return parseResult;
}

sos::Object drafter::WrapResult(snowcrash::ParseResult<snowcrash::Blueprint>& blueprint,
                                const WrapperOptions& options)
{
    ConversionContext context(options);

    if (options.astType == RefractASTType) {
        refract::IElement* parseResult = WrapParseResultRefract(blueprint, context);

        sos::Object result = SerializeRefract(parseResult, context);

        if (parseResult) {
            delete parseResult;
        }

        return result;
    }
    else {
        return WrapParseResultAST(blueprint, context);
    }
}
