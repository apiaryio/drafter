//
//  refract/Visitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_VISITOR_H
#define REFRACT_VISITOR_H

namespace refract
{

    struct IVisitor
    {
        virtual ~IVisitor(){};
    };

}; // namespace refract

#endif // #ifndef REFRACT_VISITOR_H
