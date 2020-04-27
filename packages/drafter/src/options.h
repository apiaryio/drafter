//
//  options.h
//  drafter
//
//  Created by Thomas Jandecka on 2020-02-04
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_OPTIONS_H
#define DRAFTER_OPTIONS_H

#include "drafter.h"

#include <bitset>

struct drafter_parse_options {
    using flags_type = std::bitset<3>;

    static constexpr std::size_t NAME_REQUIRED = 0;
    static constexpr std::size_t SKIP_GEN_BODIES = 1;
    static constexpr std::size_t SKIP_GEN_BODY_SCHEMAS = 2;

    flags_type flags = 0;
};

struct drafter_serialize_options {
    using flags_type = std::bitset<1>;

    static constexpr std::size_t SOURCEMAPS_INCLUDED = 0;

    flags_type flags = 0;
    drafter_format format = DRAFTER_SERIALIZE_YAML;
};

namespace drafter
{
    /* Access name_required option
     *   @remark name_required: documents without an API name section are rejected
     */
    bool is_name_required(const drafter_parse_options*) noexcept;

    /* Access sourcemaps_included option
     *   @remark sourcemaps_included: source maps are not filtered from non-Annotations
     */
    bool are_sourcemaps_included(const drafter_serialize_options*) noexcept;

    /* Access skip_gen_bodies option
     *   @remark skip_gen_bodies: skip generating message body payloads
     */
    bool is_skip_gen_bodies(const drafter_parse_options*) noexcept;

    /* Access skip_gen_body_schemas option
     *   @remark skip_gen_body_schemas: skip generating message body schema payloads
     */
    bool is_skip_gen_body_schemas(const drafter_parse_options*) noexcept;

    /* Access format option
     *   @remark format: API Elements serialisation format (YAML|JSON)
     */
    drafter_format get_format(const drafter_serialize_options*) noexcept;
}

#endif
