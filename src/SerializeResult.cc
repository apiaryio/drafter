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

#include "SourceAnnotation.h"
#include "SectionProcessor.h"

#include "refract/Build.h"
#include "refract/ElementInserter.h"

#include "NamedTypesRegistry.h"
#include "ConversionContext.h"

using namespace drafter;

namespace helper
{

    struct AnnotationToRefract {

        const std::string& key;

        AnnotationToRefract(const std::string& key) : key(key) {}

        refract::IElement* operator()(snowcrash::SourceAnnotation& annotation)
        {
            return drafter::AnnotationToRefract(annotation, key);
        }
    };
}

refract::IElement* drafter::WrapRefract(
    snowcrash::ParseResult<snowcrash::Blueprint>& blueprint, ConversionContext& context)
{
    snowcrash::Error error;
    refract::IElement* blueprintRefract = NULL;

    refract::ArrayElement* parseResult = new refract::ArrayElement;
    parseResult->element(SerializeKey::ParseResult);

    if (blueprint.report.error.code == snowcrash::Error::OK) {
        try {
            RegisterNamedTypes(
                MakeNodeInfo(blueprint.node.content.elements(), blueprint.sourceMap.content.elements()), context);
            blueprintRefract = BlueprintToRefract(MakeNodeInfo(blueprint.node, blueprint.sourceMap), context);
        } catch (std::exception& e) {
            error = snowcrash::Error(e.what(), snowcrash::MSONError);
        } catch (snowcrash::Error& e) {
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
        std::transform(warnings.begin(),
            warnings.end(),
            refract::ElementInserter(*parseResult),
            helper::AnnotationToRefract(SerializeKey::Warning));
    }

    return parseResult;
}
