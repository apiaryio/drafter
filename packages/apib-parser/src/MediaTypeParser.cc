//
//  MediaTypeParser.cc
//  drafter
//
//  Created by Jiri Kratochvil on 27/08/2019
//  Copyright (c) 2019 Apiary Inc. All rights reserved.
//

#include <apib/parser/MediaTypeParser.h>
#include <cctype>

#include "grammar/Mediatype.h"

using namespace apib::syntax;
using namespace apib::parser::mediatype;

namespace pegtl = tao::pegtl;

media_type apib::parser::parseMediaType(const std::string& input)
{

    media_type result;
    pegtl::memory_input<> in(input, "");
    pegtl::parse<pegtl::try_catch<match_grammar>, action>(in, result);
    return result;
}
