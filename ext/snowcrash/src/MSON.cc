//
//  MSON.cc
//  snowcrash
//
//  Created by Pavan Kumar Sunkara on 11/4/14.
//  Copyright (c) 2014 Apiary Inc. All rights reserved.
//

#include "MSON.h"

#include "MarkdownNode.h"

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
    return m_elements;
}

const Elements& TypeSection::Content::elements() const
{
    return m_elements;
}

TypeSection::Content::Content(const Markdown& description_, const Literal& value_)
    : description(description_), value(value_), m_elements()
{
}

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

OneOf& Element::Content::oneOf()
{
    return m_elements;
}

const OneOf& Element::Content::oneOf() const
{
    return m_elements;
}

Elements& Element::Content::elements()
{
    return m_elements;
}

const Elements& Element::Content::elements() const
{
    return m_elements;
}

Element::Content& Element::Content::operator=(const Elements& rhs)
{
    m_elements = rhs;
    return *this;
}

Element::Element(const Element::Class& klass_) : klass(klass_) {}

/**
 * \brief Build Element from property member
 *
 * \param propertyMember Property member which was given
 */
void Element::build(const PropertyMember& propertyMember)
{
    this->klass = Element::PropertyClass;
    this->content.property = propertyMember;
}

/**
 * \brief Build Element from value member
 *
 * \param valueMember Value member which was given
 */
void Element::build(const ValueMember& valueMember)
{
    this->klass = Element::ValueClass;
    this->content.value = valueMember;
}

/**
 * \brief Build Element from mixin type
 *
 * \param mixin Mixin which was given
 */
void Element::build(const Mixin& mixin)
{
    this->klass = Element::MixinClass;
    this->content.mixin = mixin;
}

/**
 * \brief Build Element from one of type
 *
 * \param oneOf One Of which was given
 */
void Element::build(const OneOf& oneOf)
{
    this->buildFromElements(oneOf);
    this->klass = Element::OneOfClass;
}

/**
 * \brief Build Element from a value
 *
 * \param value Value of the value member
 */
void Element::build(const Value& value)
{
    ValueMember valueMember;

    valueMember.valueDefinition.values.push_back(value);
    this->build(valueMember);
}

/**
 * \brief Buile Element from group of elements
 *
 * \param elements Group of elements
 */
void Element::buildFromElements(const Elements& elements)
{
    this->klass = Element::GroupClass;
    this->content = elements;
}
