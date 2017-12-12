//
//  BlueprintSourcemap.cc
//  snowcrash
//
//  Created by Pavan Kumar Sunkara on 26/01/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "BlueprintSourcemap.h"

using namespace snowcrash;

SourceMap<Elements>& SourceMap<Element>::Content::elements()
{
    return m_elements;
}

const SourceMap<Elements>& SourceMap<Element>::Content::elements() const
{
    return m_elements;
}

SourceMap<Element>::SourceMap(const Element::Class& element_)
    : element(element_)
    /** Source Map of Attributes of the Element */
    , attributes()

    /** Source Map of Content of the Element */
    , content()

    /** Type of the Category element (to be used internally only) */
    , category()
{
}
