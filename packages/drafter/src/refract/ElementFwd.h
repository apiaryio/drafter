//
//  refract/ElementFwd.h
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_ELEMENTFWD_H
#define REFRACT_ELEMENTFWD_H

namespace refract
{
    namespace dsd
    {
        class Null;
        class String;
        class Boolean;
        class Number;
        class Ref;
        class Holder;
        class Member;
        class Array;
        class Enum;
        class Object;
        class Extend;
        class Option;
        class Select;
    }

    template <typename>
    class Element;

    using NullElement = Element<dsd::Null>;
    using StringElement = Element<dsd::String>;
    using BooleanElement = Element<dsd::Boolean>;
    using NumberElement = Element<dsd::Number>;
    using RefElement = Element<dsd::Ref>;

    using HolderElement = Element<dsd::Holder>;
    using MemberElement = Element<dsd::Member>;

    using ArrayElement = Element<dsd::Array>;
    using EnumElement = Element<dsd::Enum>;
    using ObjectElement = Element<dsd::Object>;

    using ExtendElement = Element<dsd::Extend>;

    using OptionElement = Element<dsd::Option>;
    using SelectElement = Element<dsd::Select>;
}

#endif
