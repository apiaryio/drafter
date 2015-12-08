//
//  refract/PrintVisitor.h
//  librefract
//
//  Created by Vilibald Wanča on 09/11/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_PRINTVISITOR_H
#define REFRACT_PRINTVISITOR_H

#include "Visitor.h"
#include <string>
#include <ostream>

namespace refract
{

    // Forward declarations of Elements
    struct IElement;
    struct StringElement;
    struct NullElement;
    struct NumberElement;
    struct BooleanElement;
    struct ArrayElement;
    struct ObjectElement;
    struct MemberElement;

    class PrintVisitor : public IVisitor
    {
        int indent;
        std::ostream& os;

        void indentOS(int ind);
        void printMeta(const IElement& e);

    public:
        PrintVisitor();
        PrintVisitor(int indentation, std::ostream& os);

        void visit(const IElement& e);
        void visit(const MemberElement& e);
        void visit(const ObjectElement& e);
        void visit(const ArrayElement& e);
        void visit(const NullElement& e);
        void visit(const StringElement& e);
        void visit(const NumberElement& e);
        void visit(const BooleanElement& e);

        static void Visit(const IElement& e);
    };
}

#endif
