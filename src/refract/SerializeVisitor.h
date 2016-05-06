//
//  refract/SerializeVisitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_SERIALIZEVISITOR_H
#define REFRACT_SERIALIZEVISITOR_H

#include "sos.h"
#include <string>

#include "ElementFwd.h"

namespace refract
{

    class SosSerializeVisitor
    {

        sos::Object result;
        sos::Base partial;
        std::string key;
        bool generateSourceMap;


        static void SetSerializerValue(SosSerializeVisitor& s, sos::Base& value);

     public:

        SosSerializeVisitor(bool generateSourceMap) : partial(sos::Null()), generateSourceMap(generateSourceMap) {}

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

        sos::Object get()
        {
            return result;
        }
    };

}; // namespace refract

#endif // #ifndef REFRACT_SERIALIZEVISITOR_H
