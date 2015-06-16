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

    struct RenderJSONVisitor : refract::IVisitor {

        sos::Object result;

        RenderJSONVisitor();

        void visit(const refract::IElement& e);
        void visit(const refract::MemberElement& e);
        void visit(const refract::ObjectElement& e);

        void visit(const refract::NullElement& e);
        void visit(const refract::StringElement& e);
        void visit(const refract::NumberElement& e);
        void visit(const refract::BooleanElement& e);
        void visit(const refract::ArrayElement& e);

        std::string get() const;
    };
}

#endif
