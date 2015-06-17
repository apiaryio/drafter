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
    struct ObjectElement;
    struct MemberElement;
    struct Registry;

    struct ExpandVisitor : IVisitor {

        IElement* result;
        const Registry& registry;
        IElement* expandOrClone(const IElement* e);

        ExpandVisitor(const Registry& registry);

        void visit(const IElement& e);
        void visit(const MemberElement& e);
        void visit(const ObjectElement& e);

        // return expanded elemnt or NULL if expansion is not needed
        // caller responsibility is to delete returned Element
        IElement* get() const;
    };

}; // namespace refract

#endif // #ifndef _REFRACT_EXPANVISITOR_H_
