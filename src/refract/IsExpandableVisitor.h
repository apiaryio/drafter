//
//  refract/IsExpandableVisitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef _REFRACT_ISEXPANDABLEVISITOR_H_
#define _REFRACT_ISEXPANDABLEVISITOR_H_

#include "Visitor.h"

namespace refract
{

    struct IsExpandableVisitor : public IVisitor {

        bool result;
        IsExpandableVisitor();

        template<typename T> 
        void visit(const T& e);

        bool get() const;
    };



}; // namespace refract

#endif // #ifndef _REFRACT_EXPANDVISITOR_H_
