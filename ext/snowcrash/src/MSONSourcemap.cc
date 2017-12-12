//
//  MSONSourcemap.cc
//  snowcrash
//
//  Created by Pavan Kumar Sunkara on 03/01/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "MSONSourcemap.h"

using namespace snowcrash;

bool SourceMap<mson::NamedType>::empty() const
{
    return (name.sourceMap.empty() && typeDefinition.sourceMap.empty() && sections.collection.empty());
}

bool SourceMap<mson::ValueMember>::empty() const
{
    return (description.sourceMap.empty() && valueDefinition.sourceMap.empty() && sections.collection.empty());
}

bool SourceMap<mson::PropertyMember>::empty() const
{
    return (name.sourceMap.empty() && description.sourceMap.empty() && valueDefinition.sourceMap.empty()
        && sections.collection.empty());
}

SourceMap<mson::Elements>& SourceMap<mson::TypeSection>::elements()
{
    return m_elements;
}

const SourceMap<mson::Elements>& SourceMap<mson::TypeSection>::elements() const
{
    return m_elements;
}

SourceMap<mson::TypeSection>::SourceMap(
    const SourceMap<mson::Markdown>& description_, const SourceMap<mson::Literal>& value_)
    : description(description_), value(value_), m_elements()
{
}

SourceMap<mson::OneOf>& SourceMap<mson::Element>::oneOf()
{
    return m_elements;
}

const SourceMap<mson::OneOf>& SourceMap<mson::Element>::oneOf() const
{
    return m_elements;
}

SourceMap<mson::Elements>& SourceMap<mson::Element>::elements()
{
    return m_elements;
}

const SourceMap<mson::Elements>& SourceMap<mson::Element>::elements() const
{
    return m_elements;
}

SourceMap<mson::Element>& SourceMap<mson::Element>::operator=(const SourceMap<mson::Elements>& rhs)
{
    m_elements = rhs;
    return *this;
}
