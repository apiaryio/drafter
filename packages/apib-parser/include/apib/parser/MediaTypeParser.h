//
//  MediaTypeParser.h
//  apib-parser
//
//  Created by Thomas Jandecka on 07/04/2020
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//

#ifndef APIB_PARSER_MEDIA_TYPE_PARSER_H
#define APIB_PARSER_MEDIA_TYPE_PARSER_H

#include <apib/syntax/MediaType.h>

namespace apib
{
    namespace parser
    {
        apib::syntax::media_type parseMediaType(const std::string& input);
    }

}

#endif
