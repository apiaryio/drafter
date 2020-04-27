//
//  refract/ElementSize.h
//  librefract
//
//  Created by Thomas Jandecka on 21/03/2019
//  Copyright (c) 2019 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_REFRACT_ELEMENT_SIZE_H
#define DRAFTER_REFRACT_ELEMENT_SIZE_H

#include "ElementIfc.h"
#include "ElementFwd.h"
#include "Cardinal.h"

namespace refract
{
    cardinal sizeOf(const ArrayElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const BooleanElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const EnumElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const ExtendElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const HolderElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const MemberElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const NullElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const NumberElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const ObjectElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const OptionElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const RefElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const SelectElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const StringElement& e, bool inheritsFixed = false);

    cardinal sizeOf(const IElement& e, bool inheritsFixed = false);
}

#endif
