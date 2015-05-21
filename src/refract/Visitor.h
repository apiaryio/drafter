//
//  refract/Visitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef _REFRACT_VISITOR_H_
#define _REFRACT_VISITOR_H_

namespace refract
{

    struct IVisitor
    {
        virtual ~IVisitor(){};
    };

}; // namespace refract

#endif // #ifndef _REFRACT_VISITOR_H_
