//
//  RenderJSONVisitor.h
//  libdrafter
//
//  Created by Pavan Kumar Sunkara on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_RENDERJSONVISITOR_H
#define DRAFTER_RENDERJSONVISITOR_H

#include "refract/Element.h"
#include "refract/Visitors.h"
#include "sos.h"

namespace drafter {

    class RenderJSONVisitor : public refract::IVisitor {
        sos::Object pObj; // partial object
        sos::Array pArr;  // partial array
        sos::Base result;

        sos::Base::Type type;

    public:
        RenderJSONVisitor(const sos::Base::Type& type);
        RenderJSONVisitor();

        void assign(sos::Base value);
        void assign(std::string key, sos::Base value);

        void visit(const refract::IElement& e);
        void visit(const refract::MemberElement& e);
        void visit(const refract::ObjectElement& e);
        void visit(const refract::ArrayElement& e);

        void visit(const refract::NullElement& e);
        void visit(const refract::StringElement& e);
        void visit(const refract::NumberElement& e);
        void visit(const refract::BooleanElement& e);

        void extend(const refract::ObjectElement& e);

        sos::Base get() const;
        std::string getString() const;
    };
}

#endif
