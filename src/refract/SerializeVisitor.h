//
//  refract/SerializeVisitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_SERIALIZEVISITOR_H
#define REFRACT_SERIALIZEVISITOR_H

#include "Visitor.h"
#include "sos.h"
#include <string>

#include "ElementFwd.h"

namespace refract
{

    class SerializeVisitor : public IVisitor
    {

        sos::Object result;
        sos::Base partial;
        std::string key;
        bool generateSourceMap;


        static void SetSerializerValue(SerializeVisitor& s, sos::Base& value);

     public:

        SerializeVisitor(bool generateSourceMap) : partial(sos::Null()), generateSourceMap(generateSourceMap) {}

        void visit(const IElement& e);
        void visit(const NullElement& e);
        void visit(const StringElement& e);
        void visit(const NumberElement& e);
        void visit(const BooleanElement& e);
        void visit(const ArrayElement& e);
        void visit(const EnumElement& e);
        void visit(const MemberElement& e);
        void visit(const ObjectElement& e);
        void visit(const ExtendElement& e);
        void visit(const OptionElement& e);
        void visit(const SelectElement& e);

        sos::Object get()
        {
            return result;
        }
    };

}; // namespace refract

#endif // #ifndef REFRACT_SERIALIZEVISITOR_H
