//
//  UriTemplateTeParser.h
//  snowcrash
//
//  Created by Carl Griffiths 24/02/2014.
//  Copyright (c) 2013 Apiary Inc. All rights reserved.
//

#ifndef SNOWCRASH_URITEMPLATEPARSER_H
#define SNOWCRASH_URITEMPLATEPARSER_H

#include "Blueprint.h"
#include "SourceAnnotation.h"

namespace snowcrash
{

    /**
     *  \brief URI template parse result.
     */
    struct ParsedURITemplate {
        std::string scheme;
        std::string host;
        std::string path;

        Report report;
    };

    /**
     *  URI Template Parser Interface
     *  ------------------------------
     */
    class URITemplateParser
    {
    public:
        /**
         *  \brief Parse the URI template into scheme, host and path and then parse for supported URI template
         * expressions
         *
         *  \param uri        A uri to be parsed.
         */
        static void parse(
            const URITemplate& uri, const mdp::CharactersRangeSet& sourceBlock, ParsedURITemplate& result);
    };
}

#endif
