//
//  refract/SerializeVisitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef _REFRACT_SERIALIZEVISITOR_H_
#define _REFRACT_SERIALIZEVISITOR_H_

#include "Visitor.h"
#include "sos.h"
#include <string>

namespace refract
{

    // Forward declarations of Elements
    struct IElement;
    struct StringElement;
    struct NullElement;
    struct NumberElement;
    struct BooleanElement;
    struct ArrayElement;
    struct ObjectElement;
    struct MemberElement;

    class SerializeVisitor : public IVisitor
    {

        sos::Object result;
        sos::Base partial;
        std::string key;

        static void SetSerializerValue(SerializeVisitor& s, sos::Base& value);

     public:

        SerializeVisitor() : partial(sos::Null()) {}

        void visit(const IElement& e);
        void visit(const NullElement& e);
        void visit(const StringElement& e);
        void visit(const NumberElement& e);
        void visit(const BooleanElement& e);
        void visit(const ArrayElement& e);
        void visit(const MemberElement& e);
        void visit(const ObjectElement& e);

        sos::Object get()
        {
            return result;
        }
    };

}; // namespace refract

#endif // #ifndef _REFRACT_SERIALIZEVISITOR_H_
