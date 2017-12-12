//
//  Blueprint.cc
//  snowcrash
//
//  Created by Pavan Kumar Sunkara on 26/01/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "Blueprint.h"

using namespace snowcrash;

DataStructure& DataStructure::operator=(const mson::NamedType& rhs)
{
    this->name = rhs.name;
    this->typeDefinition = rhs.typeDefinition;
    this->sections = rhs.sections;

    return *this;
}

Elements& Element::Content::elements()
{
    return m_elements;
}

const Elements& Element::Content::elements() const
{
    return m_elements;
}

Element::Element(const Element::Class& element_) : element(element_), attributes(), content(), category() {}
