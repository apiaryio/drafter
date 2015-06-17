//
//  refract/IsExpandableVisitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef _REFRACT_EXPANDVISITOR_H_
#define _REFRACT_EXPANDVISITOR_H_

#include "Visitor.h"

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

    struct ExpandVisitor : IVisitor {

        IElement* result;
        ExpandVisitor();
        static IElement* expandOrClone(const IElement* e);

        void visit(const IElement& e);
        void visit(const MemberElement& e);
        void visit(const ObjectElement& e);

        IElement* get() const;
    };

}; // namespace refract

#endif // #ifndef _REFRACT_EXPANVISITOR_H_
