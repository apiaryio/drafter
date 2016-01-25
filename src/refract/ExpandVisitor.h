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
#include "ElementFwd.h"

namespace refract
{

    class Registry;

    class ExpandVisitor : public IVisitor {

    public:
        struct Context;

        ExpandVisitor(const Registry& registry);
        ~ExpandVisitor();

        void visit(const IElement& e);

        void visit(const NullElement& e);

        void visit(const StringElement& e);
        void visit(const NumberElement& e);
        void visit(const BooleanElement& e);

        void visit(const MemberElement& e);

        void visit(const ArrayElement& e);
        void visit(const EnumElement& e);
        void visit(const ObjectElement& e);

        void visit(const ExtendElement& e);

        // return expanded elemnt or NULL if expansion is not needed
        // caller responsibility is to delete returned Element
        IElement* get() const;

    private:

        IElement* result;
        Context* context;
    };

}; // namespace refract

#endif // #ifndef REFRACT_EXPANVISITOR_H
