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

#include "NamedTypesRegistry.h"
#include "ConversionContext.h"

using namespace drafter;
using namespace refract;

namespace helper
{

    struct AnnotationToRefract {

        const std::string& key;
        ConversionContext& context;

        AnnotationToRefract(const std::string& key, ConversionContext& context) : key(key), context(context) {}

        std::unique_ptr<refract::IElement> operator()(snowcrash::SourceAnnotation& annotation)
        {
            return drafter::AnnotationToRefract(annotation, key, context);
        }
    };
}

std::unique_ptr<IElement> drafter::WrapRefract(
    snowcrash::ParseResult<snowcrash::Blueprint>& blueprint, ConversionContext& context)
{
    // auto parseResult = make_empty<ArrayElement>();
    auto parseResult = make_element<ArrayElement>(); // XXX @tjanc@ review
    snowcrash::Error error;

    std::unique_ptr<IElement> blueprintRefract = nullptr;

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

        context.typeRegistry().clear();

        if (error.code != snowcrash::Error::OK) {
            blueprint.report.error = error;
        }

        if (blueprintRefract) {
            parseResult->get().push_back(std::move(blueprintRefract));
        }
    }

    if (blueprint.report.error.code != snowcrash::Error::OK) {
        parseResult->get().push_back(helper::AnnotationToRefract(SerializeKey::Error, context)(blueprint.report.error));
    }

    snowcrash::Warnings& warnings = blueprint.report.warnings;

    if (!context.warnings().empty()) {
        warnings.insert(warnings.end(), context.warnings().begin(), context.warnings().end());
    }

    if (!warnings.empty()) {
        std::transform(warnings.begin(),
            warnings.end(),
            std::back_inserter(parseResult->get()),
            helper::AnnotationToRefract(SerializeKey::Warning, context));
    }

    return std::move(parseResult);
}
