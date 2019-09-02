//
//  ContentTypeMatcher.h
//  drafter
//
//  Created by Jiri Kratochvil on 04/08/2019
//  Copyright (c) 2019 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_CONTENTYPEMATCHER_H
#define DRAFTER_CONTENTYPEMATCHER_H

#include "parser/Mediatype.h"

namespace apib {
namespace parser {
namespace mediatype {
    struct state;
}}};

namespace drafter
{
    const char* const JSONSchemaContentType = "application/schema+json";

    apib::parser::mediatype::state parseMediaType(const std::string& input);

    bool IsJSONContentType(const apib::parser::mediatype::state& mediaType);
    bool IsJSONSchemaContentType(const apib::parser::mediatype::state& mediaType);

}

#endif // DRAFTER_CONTENTYPEMATCHER_H
