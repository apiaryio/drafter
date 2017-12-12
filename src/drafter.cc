//
//  drafter.cc
//  drafter
//
//  Created by Jiri Kratochvil on 2016-06-27
//  Copyright (c) 2016 Apiary Inc. All rights reserved.
//
#include "drafter.h"

#include "snowcrash.h"

#include "refract/Element.h"
#include "refract/FilterVisitor.h"
#include "refract/Query.h"
#include "refract/Iterate.h"

#include "SerializeResult.h"      // FIXME: remove - actualy required by WrapParseResultRefract()
#include "Serialize.h"            // FIXME: remove - actualy required by WrapperOptions
#include "ConversionContext.h"    // FIXME: remove - required by ConversionContext
#include "RefractDataStructure.h" // FIXME: remove - required by SerializeRefract()

#include "sos.h" // FIXME: remove sos dependency
#include "sosJSON.h"
#include "sosYAML.h"

#include "Version.h"

#include <string.h>

DRAFTER_API drafter_error drafter_parse_blueprint_to(const char* source,
    char** out,
    const drafter_parse_options parse_opts,
    const drafter_serialize_options serialize_opts)
{

    if (!source) {
        return DRAFTER_EINVALID_INPUT;
    }

    if (!out) {
        return DRAFTER_EINVALID_OUTPUT;
    }

    drafter_result* result = nullptr;
    *out = nullptr;

    drafter_error ret = drafter_parse_blueprint(source, &result, parse_opts);

    if (!result) {
        return ret;
    }

    *out = drafter_serialize(result, serialize_opts);

    drafter_free_result(result);

    return ret;
}

namespace sc = snowcrash;

/* Parse API Bleuprint and return result, which is a opaque handle for
 * later use*/
DRAFTER_API drafter_error drafter_parse_blueprint(
    const char* source, drafter_result** out, const drafter_parse_options parse_opts)
{

    if (!source) {
        return DRAFTER_EINVALID_INPUT;
    }

    if (!out) {
        return DRAFTER_EINVALID_OUTPUT;
    }

    sc::BlueprintParserOptions scOptions = sc::ExportSourcemapOption;

    if (parse_opts.requireBlueprintName) {
        scOptions |= sc::RequireBlueprintNameOption;
    }

    sc::ParseResult<sc::Blueprint> blueprint;
    sc::parse(source, scOptions, blueprint);

    drafter::WrapperOptions wrapperOptions;
    drafter::ConversionContext context(wrapperOptions);
    auto result = WrapRefract(blueprint, context);

    *out = result.release();

    return (drafter_error)blueprint.report.error.code;
}

namespace
{ // FIXME: cut'n'paste from main.cc - duplicity

    sos::Serialize* CreateSerializer(const drafter::SerializeFormat& format)
    {
        if (format == drafter::JSONFormat) {
            return new sos::SerializeJSON;
        }

        return new sos::SerializeYAML;
    }

    /**
     * \brief Serialize sos::Object into stream
     */
    void Serialization(std::ostream* stream, const sos::Object& object, sos::Serialize* serializer)
    {
        serializer->process(object, *stream);
        *stream << "\n";
        *stream << std::flush;
    }
}

/* Serialize result to given format*/
DRAFTER_API char* drafter_serialize(drafter_result* res, const drafter_serialize_options serialize_opts)
{

    drafter::SerializeFormat format = drafter::UnknownFormat;

    if (!res) {
        return nullptr;
    }

    switch (serialize_opts.format) {
        case DRAFTER_SERIALIZE_JSON:
            format = drafter::JSONFormat;
            break;

        case DRAFTER_SERIALIZE_YAML:
            format = drafter::YAMLFormat;
            break;

        default:
            return nullptr;
    }

    drafter::WrapperOptions wrapperOptions(serialize_opts.sourcemap);
    drafter::ConversionContext context(wrapperOptions);

    sos::Object result = drafter::SerializeRefract(res, context);

    std::unique_ptr<sos::Serialize> serializer(CreateSerializer(format));

    std::ostringstream out;

    Serialization(&out, result, serializer.get());

    return strdup(out.str().c_str());
}

/* Parse API Blueprint and return only annotations, if NULL than
 * document is error and warning free.*/
DRAFTER_API drafter_error drafter_check_blueprint(
    const char* source, drafter_result** res, const drafter_parse_options parse_opts)
{

    if (!source) {
        return DRAFTER_EINVALID_INPUT;
    }

    drafter_result* result = nullptr;

    drafter_error ret = drafter_parse_blueprint(source, &result, parse_opts);

    if (!result) {
        return ret;
    }

    drafter_result* out = nullptr;

    refract::FilterVisitor filter(refract::query::Element("annotation"));
    refract::Iterate<refract::Children> iterate(filter);
    iterate(*result);

    if (!filter.empty()) {
        refract::ArrayElement::ValueType elements;

        std::transform(filter.elements().begin(),
            filter.elements().end(),
            std::back_inserter(elements),
            std::bind(&refract::IElement::clone, std::placeholders::_1, refract::IElement::cAll));

        out = new refract::ArrayElement(elements);
        out->element(drafter::SerializeKey::ParseResult);
    }

    drafter_free_result(result);

    *res = out;

    return ret;
}

DRAFTER_API void drafter_free_result(drafter_result* result)
{
    delete result;
}

#define VERSION_SHIFT_STEP 8

DRAFTER_API unsigned int drafter_version(void)
{
    unsigned int version = 0;

    version |= DRAFTER_MAJOR_VERSION;
    version <<= VERSION_SHIFT_STEP;
    version |= DRAFTER_MINOR_VERSION;
    version <<= VERSION_SHIFT_STEP;
    version |= DRAFTER_PATCH_VERSION;

    return version;
}

#undef VERSION_SHIFT_STEP

DRAFTER_API const char* drafter_version_string(void)
{
    return DRAFTER_VERSION_STRING;
}
