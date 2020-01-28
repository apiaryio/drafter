//
//  options.cc
//  drafter
//
//  Created by Thomas Jandecka on 2020-02-04
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//
#include "options.h"

bool drafter::is_name_required(const drafter_parse_options* opts) noexcept
{
    return opts && opts->requireBlueprintName;
}

bool drafter::are_sourcemaps_included(const drafter_serialize_options* opts) noexcept
{
    return opts && opts->sourcemap;
}

drafter_format drafter::get_format(const drafter_serialize_options* opts) noexcept
{
    return opts ? opts->format : DRAFTER_SERIALIZE_YAML;
}
