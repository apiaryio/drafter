//
//  refract/SerializeCompactVisitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_SERIALIZECOMPACTVISITOR_H
#define REFRACT_SERIALIZECOMPACTVISITOR_H

#include "sos.h"
#include <string>

#include "ElementFwd.h"

namespace refract
{

    class SosSerializeCompactVisitor 
    {
        std::string key_;
        sos::Base value_;
        bool generateSourceMap;

    public:
        SosSerializeCompactVisitor() : generateSourceMap(true) {}
        SosSerializeCompactVisitor(bool generateSourceMap) : generateSourceMap(generateSourceMap) {}

        void operator()(const IElement& e);
        void operator()(const NullElement& e);
        void operator()(const StringElement& e);
        void operator()(const NumberElement& e);
        void operator()(const BooleanElement& e);
        void operator()(const ArrayElement& e);
        void operator()(const EnumElement& e);
        void operator()(const MemberElement& e);
        void operator()(const ObjectElement& e);
        void operator()(const ExtendElement& e);
        void operator()(const SelectElement& e);
        void operator()(const OptionElement& e);

        std::string key()
        {
            return key_;
        }

        sos::Base value()
        {
            return value_;
        }

    };

}; // namespace refract

#endif // #ifndef REFRACT_SERIALIZECOMPACTVISITOR_H
