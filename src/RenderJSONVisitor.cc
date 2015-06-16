//
//  RenderJSONVisitor.cc
//  libdrafter
//
//  Created by Pavan Kumar Sunkara on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "RenderJSONVisitor.h"

using namespace refract;

namespace drafter
{

    void RenderJSONVisitor::visit(const IElement& e) {}

    void RenderJSONVisitor::visit(const MemberElement& e) {}

    void RenderJSONVisitor::visit(const ObjectElement& e) {}

    void RenderJSONVisitor::visit(const NullElement& e) {}
    void RenderJSONVisitor::visit(const StringElement& e) {}
    void RenderJSONVisitor::visit(const NumberElement& e) {}
    void RenderJSONVisitor::visit(const BooleanElement& e) {}
    
    void RenderJSONVisitor::visit(const ArrayElement& e) {}
    
    std::string RenderJSONVisitor::get() const {
        return "";
    }
    
}
