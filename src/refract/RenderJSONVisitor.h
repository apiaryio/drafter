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

    class RenderJSONVisitor : public IVisitor
    {
        sos::Object pObj; // partial object
        sos::Array pArr;  // partial array
        sos::Base result;

        sos::Base::Type type;
        bool isExtend;

    public:
        RenderJSONVisitor(const sos::Base::Type& type);
        RenderJSONVisitor();

        void assign(sos::Base value);
        void assign(std::string key, sos::Base value);

        void extend(sos::Base value);

        void visit(const IElement& e);
        void visit(const MemberElement& e);
        void visit(const ObjectElement& e);
        void visit(const ArrayElement& e);

        void visit(const NullElement& e);
        void visit(const StringElement& e);
        void visit(const NumberElement& e);
        void visit(const BooleanElement& e);

        sos::Base get() const;
        std::string getString() const;
    };
}

#endif
