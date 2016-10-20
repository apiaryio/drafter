//
//  drafter.cc
//  drafter
//
//  Created by Jiri Kratochvil on 2016-06-27
//  Copyright (c) 2016 Apiary Inc. All rights reserved.
//
#include "drafter_private.h"

#include "snowcrash.h"

#include "SerializeResult.h" // FIXME: remove - actualy required by WrapParseResultRefract()
#include "Serialize.h" // FIXME: remove - actualy required by WrapperOptions
#include "ConversionContext.h" // FIXME: remove - required by ConversionContext
#include "RefractDataStructure.h" // FIXME: remove - required by SerializeRefract()

#include "sos.h" // FIXME: remove sos dependency
#include "sosJSON.h"
#include "sosYAML.h"

#include "Version.h"

#include <string.h>

DRAFTER_API int drafter_parse_blueprint_to(const char* source,
                                           char ** out,
                                           const drafter_options options) {

    if (!source || !out) {
        return -1;
    }

    drafter_result* result = nullptr;
    *out = nullptr;

    int ret = drafter_parse_blueprint(source, &result);

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

    drafter_parse_options options = {false};

    return drafter_parse_blueprint_with_options(source, out, options);
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

    drafter_parse_options options = {false};

    return drafter_check_blueprint_with_options(source, options);
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
