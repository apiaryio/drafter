//
//  RenderFormURLEncodedVisitor.h
//  librefract
//
//  Created by Kyle Fuller on 17/09/16.
//  Copyright (c) 2016 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_RENDERFORMURLENCODEDVISITOR_H
#define REFRACT_RENDERFORMURLENCODEDVISITOR_H

#include <string>
#include <sstream>
#include "ElementFwd.h"

namespace refract
{
    class RenderFormURLEncodedVisitor
    {
        std::stringstream result;
        std::string baseKey;

        /// Returns the given value as percent encoded
        std::string percentEncode(const std::string& value) const;

        template<typename T>
        void primitiveType(const T& element);

    public:

        RenderFormURLEncodedVisitor(const std::string &key = "");
        virtual ~RenderFormURLEncodedVisitor();

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

        std::string getString() const;
    };
}

#endif
