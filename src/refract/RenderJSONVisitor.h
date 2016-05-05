//
//  refract/RenderJSONVisitor.h
//  librefract
//
//  Created by Pavan Kumar Sunkara on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_RENDERJSONVISITOR_H
#define REFRACT_RENDERJSONVISITOR_H

#include "sos.h"
#include <string>

#include "ElementFwd.h"

namespace refract
{

    class RenderJSONVisitor
    {
        IElement* result;
        IElement* enumValue;
    public:

        RenderJSONVisitor();
        virtual ~RenderJSONVisitor();

        void operator()(const IElement& e);
        void operator()(const MemberElement& e);
        void operator()(const ObjectElement& e);
        void operator()(const ArrayElement& e);
        void operator()(const EnumElement& e);

        void operator()(const NullElement& e);
        void operator()(const StringElement& e);
        void operator()(const NumberElement& e);
        void operator()(const BooleanElement& e);
        void operator()(const ExtendElement& e);

        // FIXME: throwning - not implemented 
        // need to resolve in ApplyVisitor
        //void operator()(const OptionElement& e);
        //void operator()(const SelectElement& e);

        std::string getString() const;
        IElement* getOwnership(); 
    };
}

#endif
