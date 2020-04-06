//
//  drafter.cc
//  drafter
//
//  Created by Jiri Kratochvil on 2016-06-27
//  Copyright (c) 2016 Apiary Inc. All rights reserved.
//
#include "drafter.h"

#include "snowcrash.h"

#include "utils/so/JsonIo.h"
#include "utils/so/YamlIo.h"

#include "refract/Element.h"
#include "refract/FilterVisitor.h"
#include "refract/Query.h"
#include "refract/Iterate.h"
#include "refract/SerializeSo.h"

#include "SerializeResult.h" // FIXME: remove - actualy required by WrapParseResultRefract()
#include "ConversionContext.h"
#include "RefractDataStructure.h" // FIXME: remove - required by SerializeRefract()

#if defined CMAKE_BUILD_TYPE
// we moved version management to cmake
// but we still need to maintain gyp buld to allow npm protagonist build
// version information is not exposed to public space in protagonist
// so we can do hack and hide this API call
#include "Version.h"
#endif

#include "reporting.h"
#include "options.h"

#include <cstring>
#include <cassert>

DRAFTER_API drafter_error drafter_parse_blueprint_to(const char* source,
    char** out,
    const drafter_parse_options* parse_opts,
    const drafter_serialize_options* serialize_opts)
{
    if (!source) {
        return DRAFTER_EINVALID_INPUT;
    }

    drafter_result* result = nullptr;

    drafter_error ret = drafter_parse_blueprint(source, &result, parse_opts);

    if (!result) {
        return ret;
    }

    if (out) {
        *out = drafter_serialize(result, serialize_opts);
    }

    drafter_free_result(result);

    return ret;
}

namespace sc = snowcrash;

/* Parse API Bleuprint and return result, which is a opaque handle for
 * later use*/
DRAFTER_API drafter_error drafter_parse_blueprint(
    const char* source, drafter_result** out, const drafter_parse_options* parse_opts)
{
    if (!source) {
        return DRAFTER_EINVALID_INPUT;
    }

    sc::BlueprintParserOptions scOptions = sc::ExportSourcemapOption;

    if (drafter::is_name_required(parse_opts)) {
        scOptions |= sc::RequireBlueprintNameOption;
    }

    sc::ParseResult<sc::Blueprint> blueprint;
    sc::parse(source, scOptions, blueprint);

    drafter::ConversionContext context(source, parse_opts);
    auto result = WrapRefract(blueprint, context);

    if (out) {
        *out = result.release();
    }

    return (drafter_error)blueprint.report.error.code;
}

/* Serialize result to given format*/
DRAFTER_API char* drafter_serialize(drafter_result* res, const drafter_serialize_options* serialize_opts)
{
    if (!res) {
        return nullptr;
    }

    std::ostringstream out;

    switch (drafter::get_format(serialize_opts)) {
        case DRAFTER_SERIALIZE_JSON: {
            auto soValue = refract::serialize::renderSo(*res, drafter::are_sourcemaps_included(serialize_opts));
            drafter::utils::so::serialize_json(out, soValue);
            break;
        }
        case DRAFTER_SERIALIZE_YAML: {
            auto soValue = refract::serialize::renderSo(*res, drafter::are_sourcemaps_included(serialize_opts));
            drafter::utils::so::serialize_yaml(out, soValue);
            break;
        }

        default:
            return nullptr;
    }

    return strdup(out.str().c_str());
}

/* Parse API Blueprint and return only annotations, if NULL than
 * document is error and warning free.*/
DRAFTER_API drafter_error drafter_check_blueprint(
    const char* source, drafter_result** res, const drafter_parse_options* parse_opts)
{
    if (!source) {
        return DRAFTER_EINVALID_INPUT;
    }

    drafter_result* result = nullptr;

    drafter_error ret = res ? drafter_parse_blueprint(source, &result, parse_opts) :
                              drafter_parse_blueprint(source, nullptr, parse_opts);

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

DRAFTER_API drafter_parse_options* drafter_init_parse_options()
{
    return new drafter_parse_options{};
}

DRAFTER_API void drafter_free_parse_options(drafter_parse_options* opts)
{
    delete opts;
}

DRAFTER_API void drafter_set_name_required(drafter_parse_options* opts)
{
    assert(opts);
    opts->flags.set(drafter_parse_options::NAME_REQUIRED);
}

DRAFTER_API void drafter_set_skip_gen_bodies(drafter_parse_options* opts)
{
    assert(opts);
    opts->flags.set(drafter_parse_options::SKIP_GEN_BODIES);
}

DRAFTER_API void drafter_set_skip_gen_body_schemas(drafter_parse_options* opts)
{
    assert(opts);
    opts->flags.set(drafter_parse_options::SKIP_GEN_BODY_SCHEMAS);
}

DRAFTER_API drafter_serialize_options* drafter_init_serialize_options()
{
    return new drafter_serialize_options{};
}

DRAFTER_API void drafter_free_serialize_options(drafter_serialize_options* opts)
{
    delete opts;
}

DRAFTER_API void drafter_set_sourcemaps_included(drafter_serialize_options* opts)
{
    assert(opts);
    opts->flags.set(drafter_serialize_options::SOURCEMAPS_INCLUDED);
}

DRAFTER_API void drafter_set_format(drafter_serialize_options* opts, drafter_format fmt)
{
    assert(opts);
    opts->format = fmt;
}

#define VERSION_SHIFT_STEP 8

DRAFTER_API unsigned int drafter_version(void)
{
    unsigned int version = 0;

#if defined CMAKE_BUILD_TYPE
    version |= DRAFTER_MAJOR_VERSION;
    version <<= VERSION_SHIFT_STEP;
    version |= DRAFTER_MINOR_VERSION;
    version <<= VERSION_SHIFT_STEP;
    version |= DRAFTER_PATCH_VERSION;
#endif

    return version;
}

#undef VERSION_SHIFT_STEP

DRAFTER_API const char* drafter_version_string(void)
{
#if defined CMAKE_BUILD_TYPE
    return DRAFTER_VERSION_STRING;
#else
    return "Non-oficial drafter gyp-based build";
#endif
}
