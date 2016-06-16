#include "API.h"

#include "snowcrash.h"

#include "refract/Element.h"
#include "refract/FilterVisitor.h"
#include "refract/Query.h"


#include "SerializeResult.h" // FIXME: remove - actualy required by WrapParseResultRefract()
#include "Serialize.h" // FIXME: remove - actualy required by WrapperOptions
#include "RefractDataStructure.h" // FIXME: remove - required by SerializeRefract()

#include "sos.h" // FIXME: remove sos dependency
#include "sosJSON.h"
#include "sosYAML.h"

#include "ConversionContext.h"

#include <string.h>

DRAFTER_API int drafter_parse_blueprint_to(const char* source,
                               const char ** out,
                               drafter_options options) {

    drafter_result* result = nullptr;
    *out = nullptr;

    int ret = drafter_parse_blueprint(source, &result);

    if (!result) {
        // FIXME: there is no parse result
        return -1;
    }

    *out = drafter_serialize(result, options);

    free_drafter_result(result);

    return ret;
}

namespace sc = snowcrash;

/* Parse API Bleuprint and return result, which is a opaque handle for
 * later use*/
DRAFTER_API int drafter_parse_blueprint(const char* source, drafter_result** out) {

    if (!source) {
        // FIXME: no source code
    }

    sc::ParseResult<sc::Blueprint> blueprint;
    sc::parse(source, snowcrash::ExportSourcemapOption, blueprint);

    drafter::WrapperOptions options(drafter::RefractASTType);
    drafter::ConversionContext context(options);
    refract::IElement* result = WrapParseResultRefract(blueprint, context);

    if (!out) {
        delete result;
    }

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
DRAFTER_API const char* drafter_serialize(drafter_result *res, drafter_options options) {

    if (!res) {
        // FIXME: nothing to seralize
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

    drafter_result* result = nullptr;

    int ret = drafter_parse_blueprint(source, &result);

    if (!result) {
        // FIXME: there is no parse result
        return result;
    }

    refract::FilterVisitor filter(refract::query::ByName("annotation"));
    //refract::FilterVisitor filter;
    refract::Visit(filter, *result);
    //drafter_result* annotations = drafter_result->content(filter);

    free_drafter_result(result);

    return nullptr;
}

DRAFTER_API void free_drafter_result(drafter_result* result) {
    delete result;
}

DRAFTER_API unsigned int drafter_c_version(void) {
    return -1;
}

DRAFTER_API const char* drafter_c_version_string(void) {
    return 0;
}
