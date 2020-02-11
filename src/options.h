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

struct drafter_parse_options {
    bool requireBlueprintName = false;
};

struct drafter_serialize_options {
    bool sourcemap = false;
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

    /* Access format option
     *   @remark format: API Elements serialisation format (YAML|JSON)
     */
    drafter_format get_format(const drafter_serialize_options*) noexcept;
}

#endif
