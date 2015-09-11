//
//  refract/IsExpandableVisitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_ISEXPANDABLEVISITOR_H
#define REFRACT_ISEXPANDABLEVISITOR_H

#include "Visitor.h"

namespace refract
{

    class IsExpandableVisitor : public IVisitor {

        bool result;

    public:

        IsExpandableVisitor();

        template<typename T>
        void visit(const T& e);

        bool get() const;
    };

}; // namespace refract

#endif // #ifndef REFRACT_EXPANDVISITOR_H
