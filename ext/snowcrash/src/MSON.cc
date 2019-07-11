//
//  MSON.cc
//  snowcrash
//
//  Created by Pavan Kumar Sunkara on 11/4/14.
//  Copyright (c) 2014 Apiary Inc. All rights reserved.
//

#include "MSON.h"

using namespace mson;

bool Value::empty() const
{
    return (this->literal.empty() && this->variable == false);
}

bool Symbol::empty() const
{
    return (this->literal.empty() && this->variable == false);
}

bool TypeName::empty() const
{
    return (this->base == UndefinedTypeName && this->symbol.empty());
}

bool TypeSpecification::empty() const
{
    return (this->nestedTypes.empty() && this->name.empty());
}

bool TypeDefinition::empty() const
{
    return (this->attributes == 0 && this->typeSpecification.empty());
}

bool ValueDefinition::empty() const
{
    return (this->values.empty() && this->typeDefinition.empty());
}

Elements& TypeSection::Content::elements()
{
    if (!m_elements.get())
        throw ELEMENTS_NOT_SET_ERR;

    return *m_elements;
}

const Elements& TypeSection::Content::elements() const
{
    if (!m_elements.get())
        throw ELEMENTS_NOT_SET_ERR;

    return *m_elements;
}

TypeSection::Content::Content(const Markdown& description_, const Literal& value_)
    : description(description_), value(value_)
{
    m_elements.reset(::new Elements);
}

TypeSection::Content::Content(const TypeSection::Content& rhs)
{
    this->description = rhs.description;
    this->value = rhs.value;
    m_elements.reset(::new Elements(*rhs.m_elements.get()));
}

TypeSection::Content& TypeSection::Content::operator=(const TypeSection::Content& rhs)
{
    this->description = rhs.description;
    this->value = rhs.value;
    m_elements.reset(::new Elements(*rhs.m_elements.get()));

    return *this;
}

TypeSection::Content::~Content() {}

bool TypeSection::empty() const
{
    return (this->klass == TypeSection::UndefinedClass && this->content.value.empty()
        && this->content.description.empty() && this->content.elements().empty());
}

bool NamedType::empty() const
{
    return (this->typeDefinition.empty() && this->name.empty() && this->sections.empty());
}

bool ValueMember::empty() const
{
    return (this->valueDefinition.empty() && this->sections.empty() && this->description.empty());
}

bool PropertyName::empty() const
{
    return (this->literal.empty() && this->variable.empty());
}

bool PropertyMember::empty() const
{
    return (this->name.empty() && this->description.empty() && this->sections.empty() && this->valueDefinition.empty());
}
