//
//  RefractParseResult.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 25/09/15.
//  Copyright (c) 2015 Apiary. All rights reserved.
//

#include "RefractAPI.h"
#include "RefractParseResult.h"

namespace drafter {

    typedef std::vector<refract::IElement*> RefractElements;

    refract::IElement* ParseResultToRefract(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        element->element(SerializeKey::ParseResult);

        content.push_back(BlueprintToRefract(blueprint.node));

        element->set(content);

        return element;
    }
}
