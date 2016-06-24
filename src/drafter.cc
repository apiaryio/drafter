#include "drafter.h"

#include "snowcrash.h"

#include "refract/Element.h"
#include "refract/FilterVisitor.h"
#include "refract/Query.h"
#include "refract/Iterate.h"


#include "SerializeResult.h" // FIXME: remove - actualy required by WrapParseResultRefract()
#include "Serialize.h" // FIXME: remove - actualy required by WrapperOptions
#include "RefractDataStructure.h" // FIXME: remove - required by SerializeRefract()

#include "sos.h" // FIXME: remove sos dependency
#include "sosJSON.h"
#include "sosYAML.h"

#include "ConversionContext.h"

#include "Version.h"

#include <string.h>

#include <assert.h>

DRAFTER_API int drafter_parse_blueprint_to(const char* source,
                                           char ** out,
                                           const drafter_options options) {

    assert(source);
    assert(out);

    if (!source || !out) {
        return -1;
    }

    drafter_result* result = nullptr;
    *out = nullptr;

    int ret = drafter_parse_blueprint(source, &result);

    assert(result);

    if (!result) {
        return -1;
    }

    *out = drafter_serialize(result, options);

    drafter_free_result(result);

    return ret;
}

namespace sc = snowcrash;

/* Parse API Bleuprint and return result, which is a opaque handle for
 * later use*/
DRAFTER_API int drafter_parse_blueprint(const char* source, drafter_result** out) {

    assert(source);
    assert(out);

    if (!source) {
        return -1;
    }

    sc::ParseResult<sc::Blueprint> blueprint;
    sc::parse(source, snowcrash::ExportSourcemapOption, blueprint);

    drafter::WrapperOptions options(drafter::RefractASTType);
    drafter::ConversionContext context(options);
    refract::IElement* result = WrapParseResultRefract(blueprint, context);

    if (!out) {
        delete result;
    }

    *out = result;

    return blueprint.report.error.code;
}

namespace { // FIXME: cut'n'paste from main.cc - duplicity

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
    void Serialization(std::ostream *stream,
                       const sos::Object& object,
                       sos::Serialize* serializer)
    {
        serializer->process(object, *stream);
        *stream << "\n";
        *stream << std::flush;
    }

}

/* Serialize result to given format*/
DRAFTER_API char* drafter_serialize(drafter_result *res, const drafter_options options) {

    assert(res);

    if (!res) {
        return nullptr;
    }

    drafter::WrapperOptions woptions(drafter::RefractASTType, options.sourcemap);
    drafter::ConversionContext context(woptions);

    sos::Object result = drafter::SerializeRefract(res, context);

    std::unique_ptr<sos::Serialize> serializer(CreateSerializer(options.format == DRAFTER_SERIALIZE_JSON ? drafter::JSONFormat : drafter::YAMLFormat));

    std::ostringstream out;

    Serialization(&out, result, serializer.get());

    return strdup(out.str().c_str());
}

/* Parse API Blueprint and return only annotations, if NULL than
 * document is error and warning free.*/
DRAFTER_API drafter_result* drafter_check_blueprint(const char* source) {

    assert(source);
    if (!source) {
        return nullptr;
    }

    drafter_result* result = nullptr;

    drafter_parse_blueprint(source, &result);

    if (!result) {
        return result;
    }

    drafter_result* out = nullptr;

    refract::FilterVisitor filter(refract::query::Element("annotation"));
    refract::Iterate<refract::Children> iterate(filter);
    iterate(*result);

    if (!filter.empty()) {
        typename refract::ArrayElement::ValueType elements;

        std::transform(filter.elements().begin(), filter.elements().end(),
                       std::back_inserter(elements),
                       std::bind(&refract::IElement::clone, std::placeholders::_1, refract::IElement::cAll));

        out = new refract::ArrayElement(elements);
        out->element(drafter::SerializeKey::ParseResult);
    }

    drafter_free_result(result);

    return out;
}

DRAFTER_API void drafter_free_result(drafter_result* result) {
    delete result;
}

#define VERSION_SHIFT_STEP 8

DRAFTER_API unsigned int drafter_version(void) {
    unsigned int version = 0;
    version |= DRAFTER_MAJOR_VERSION;
    version <<= VERSION_SHIFT_STEP;
    version |= DRAFTER_MINOR_VERSION;
    version <<= VERSION_SHIFT_STEP;
    version |= DRAFTER_PATCH_VERSION;
    return version;
}

#undef VERSION_SHIFT_STEP

DRAFTER_API const char* drafter_version_string(void) {
    return DRAFTER_VERSION_STRING;
}
