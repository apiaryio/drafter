//
//  refract/PrintVisitor.h
//  librefract
//
//  Created by Vilibald Wanča on 09/11/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_PRINTVISITOR_H
#define REFRACT_PRINTVISITOR_H

#include <string>
#include <ostream>

#include "ElementFwd.h"

namespace refract
{

    class PrintVisitor 
    {
        int indent;
        std::ostream& os;

        void indentOS(int ind);
        void printMeta(const IElement& e);
        void printAttr(const IElement& e);

    public:
        PrintVisitor();
        PrintVisitor(int indentation, std::ostream& os);

        void operator()(const IElement& e);
        void operator()(const MemberElement& e);
        void operator()(const ObjectElement& e);
        void operator()(const ArrayElement& e);
        void operator()(const EnumElement& e);
        void operator()(const NullElement& e);
        void operator()(const StringElement& e);
        void operator()(const NumberElement& e);
        void operator()(const BooleanElement& e);
        void operator()(const ExtendElement& e);
        void operator()(const OptionElement& e);
        void operator()(const SelectElement& e);

        static void Visit(const IElement& e);
    };
}

#endif
