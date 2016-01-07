//
//  refract/RenderJSONVisitor.h
//  librefract
//
//  Created by Pavan Kumar Sunkara on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_RENDERJSONVISITOR_H
#define REFRACT_RENDERJSONVISITOR_H

#include "Visitor.h"
#include "sos.h"
#include <string>

#include "ElementFwd.h"

namespace refract
{

    class RenderJSONVisitor : public IVisitor
    {
        IElement* result;
        IElement* enumValue;
    public:

        RenderJSONVisitor();
        virtual ~RenderJSONVisitor();

        void visit(const IElement& e);
        void visit(const MemberElement& e);
        void visit(const ObjectElement& e);
        void visit(const ArrayElement& e);
        void visit(const EnumElement& e);

        void visit(const NullElement& e);
        void visit(const StringElement& e);
        void visit(const NumberElement& e);
        void visit(const BooleanElement& e);
        void visit(const ExtendElement& e);

        std::string getString() const;
        IElement* getOwnership(); 
    };
}

#endif
