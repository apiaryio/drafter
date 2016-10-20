//
//  drafter_private.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 20/10/16.
//  Copyright © 2016 Apiary. All rights reserved.
//

#include "drafter_private.h"

#include "snowcrash.h"

#include "refract/Element.h"
#include "refract/FilterVisitor.h"
#include "refract/Query.h"
#include "refract/Iterate.h"

#include "SerializeResult.h" // FIXME: remove - actualy required by WrapParseResultRefract()
#include "Serialize.h" // FIXME: remove - actualy required by WrapperOptions
#include "ConversionContext.h" // FIXME: remove - required by ConversionContext

namespace sc = snowcrash;

DRAFTER_API int drafter_parse_blueprint_with_options(const char* source,
                                                     drafter_result** out,
                                                     const drafter_parse_options options) {

    if (!source || !out) {
        return -1;
    }

    sc::BlueprintParserOptions scOptions = sc::ExportSourcemapOption;

    if (options.requireBlueprintName) {
        scOptions |= sc::RequireBlueprintNameOption;
    }

    sc::ParseResult<sc::Blueprint> blueprint;
    sc::parse(source, scOptions, blueprint);

    drafter::WrapperOptions wrapperOptions(drafter::RefractASTType);
    drafter::ConversionContext context(wrapperOptions);
    refract::IElement* result = WrapParseResultRefract(blueprint, context);

    *out = result;

    return blueprint.report.error.code;
}

DRAFTER_API drafter_result* drafter_check_blueprint_with_options(const char* source,
                                                                 const drafter_parse_options options) {

    if (!source) {
        return nullptr;
    }

    drafter_result* result = nullptr;

    drafter_parse_blueprint_with_options(source, &result, options);

    if (!result) {
        return nullptr;
    }

    drafter_result* out = nullptr;

    refract::FilterVisitor filter(refract::query::Element("annotation"));
    refract::Iterate<refract::Children> iterate(filter);
    iterate(*result);

    if (!filter.empty()) {
        refract::ArrayElement::ValueType elements;

        std::transform(filter.elements().begin(), filter.elements().end(),
                       std::back_inserter(elements),
                       std::bind(&refract::IElement::clone, std::placeholders::_1, refract::IElement::cAll));

        out = new refract::ArrayElement(elements);
        out->element(drafter::SerializeKey::ParseResult);
    }

    drafter_free_result(result);

    return out;
}
