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

SourceMap<mson::MemberTypes>& SourceMap<mson::TypeSection>::elements()
{
    if (!m_elements.get())
        throw ELEMENTS_NOT_SET_ERR;

    return *m_elements;
}

const SourceMap<mson::MemberTypes>& SourceMap<mson::TypeSection>::elements() const
{
    if (!m_elements.get())
        throw ELEMENTS_NOT_SET_ERR;

    return *m_elements;
}

SourceMap<mson::TypeSection>::SourceMap(
    const SourceMap<mson::Markdown>& description_, const SourceMap<mson::Literal>& value_)
    : description(description_), value(value_)
{
    m_elements.reset(::new SourceMap<mson::MemberTypes>);
}

SourceMap<mson::TypeSection>::SourceMap(const SourceMap<mson::TypeSection>& rhs)
{
    this->description = rhs.description;
    this->value = rhs.value;
    m_elements.reset(::new SourceMap<mson::MemberTypes>(*rhs.m_elements.get()));
}

SourceMap<mson::TypeSection>& SourceMap<mson::TypeSection>::operator=(const SourceMap<mson::TypeSection>& rhs)
{
    this->description = rhs.description;
    this->value = rhs.value;
    m_elements.reset(::new SourceMap<mson::MemberTypes>(*rhs.m_elements.get()));

    return *this;
}

SourceMap<mson::TypeSection>::~SourceMap() {}

SourceMap<mson::OneOf>& SourceMap<mson::MemberType>::oneOf()
{
    if (!m_elements.get())
        throw ELEMENTS_NOT_SET_ERR;

    return *m_elements;
}

const SourceMap<mson::OneOf>& SourceMap<mson::MemberType>::oneOf() const
{
    if (!m_elements.get())
        throw ELEMENTS_NOT_SET_ERR;

    return *m_elements;
}

SourceMap<mson::MemberTypes>& SourceMap<mson::MemberType>::elements()
{
    if (!m_elements.get())
        throw ELEMENTS_NOT_SET_ERR;

    return *m_elements;
}

const SourceMap<mson::MemberTypes>& SourceMap<mson::MemberType>::elements() const
{
    if (!m_elements.get())
        throw ELEMENTS_NOT_SET_ERR;

    return *m_elements;
}

SourceMap<mson::MemberType>& SourceMap<mson::MemberType>::operator=(const SourceMap<mson::MemberTypes>& rhs)
{
    m_elements.reset(::new SourceMap<mson::MemberTypes>(rhs));

    return *this;
}

SourceMap<mson::MemberType>::SourceMap()
{
    m_elements.reset(::new SourceMap<mson::MemberTypes>);
}

SourceMap<mson::MemberType>::SourceMap(const SourceMap<mson::MemberType>& rhs)
{
    this->property = rhs.property;
    this->value = rhs.value;
    this->mixin = rhs.mixin;
    m_elements.reset(::new SourceMap<mson::MemberTypes>(*rhs.m_elements.get()));
}

SourceMap<mson::MemberType>& SourceMap<mson::MemberType>::operator=(const SourceMap<mson::MemberType>& rhs)
{
    this->property = rhs.property;
    this->value = rhs.value;
    this->mixin = rhs.mixin;
    m_elements.reset(::new SourceMap<mson::MemberTypes>(*rhs.m_elements.get()));

    return *this;
}

SourceMap<mson::MemberType>::~SourceMap() {}
