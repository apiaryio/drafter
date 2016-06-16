//
//  refract/FilterVisitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 10/06/16.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_FILTERVISITOR_H
#define REFRACT_FILTERVISITOR_H

#include "ElementFwd.h"
#include "Visitor.h"

namespace refract
{

    class FilterVisitor
    {

        public:

        template<typename Query>
        FilterVisitor(Query query) {};

        void operator()(const IElement& e){};
        void operator()(const NullElement& e){};
        void operator()(const StringElement& e){};
        void operator()(const NumberElement& e){};
        void operator()(const BooleanElement& e){};
        void operator()(const ArrayElement& e){};
        void operator()(const MemberElement& e){};
        void operator()(const ObjectElement& e){};
        void operator()(const EnumElement& e){};
        void operator()(const ExtendElement& e){};
        void operator()(const OptionElement& e){};
        void operator()(const SelectElement& e){};

    };

}; // namespace refract

#endif // #ifndef REFRACT_FILTERVISITOR_H
