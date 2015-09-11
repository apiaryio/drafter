//
//  refract/ExpandVisitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_EXPANDVISITOR_H
#define REFRACT_EXPANDVISITOR_H

#include "Visitor.h"

namespace refract
{

    // Forward declarations of Elements
    struct IElement;
    struct ObjectElement;
    struct MemberElement;

    class Registry;

    struct NullElement;
    struct StringElement;
    struct NumberElement;
    struct BooleanElement;
    struct ArrayElement;

    class ExpandVisitor : public IVisitor {

        IElement* result;
        const Registry& registry;

    public:

        ExpandVisitor(const Registry& registry);

        void visit(const IElement& e);
        void visit(const MemberElement& e);
        void visit(const ObjectElement& e);

        void visit(const NullElement& e);
        void visit(const StringElement& e);
        void visit(const NumberElement& e);
        void visit(const BooleanElement& e);
        void visit(const ArrayElement& e);

        // return expanded elemnt or NULL if expansion is not needed
        // caller responsibility is to delete returned Element
        IElement* get() const;
    };

}; // namespace refract

#endif // #ifndef REFRACT_EXPANVISITOR_H
