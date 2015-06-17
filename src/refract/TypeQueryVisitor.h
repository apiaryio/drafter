//
//  refract/TypeQueryVisitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef _REFRACT_TYPEQUERYVISITOR_H_
#define _REFRACT_TYPEQUERYVISITOR_H_

#include "Visitor.h"

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

    class TypeQueryVisitor : public IVisitor
    {
        typedef unsigned char ElementType;
        ElementType typeInfo;
    public:

        enum 
        {
            Null,
            String,
            Number,
            Boolean,
            Array,
            Member,
            Object,

            Unknown = 0,
        };

        TypeQueryVisitor();

        template<typename E>
        E* as(IElement* e) const;

        template<typename E>
        const E* as(const IElement* e) const;

        void visit(const IElement& e);
        void visit(const NullElement& e);
        void visit(const StringElement& e);
        void visit(const NumberElement& e);
        void visit(const BooleanElement& e);
        void visit(const ArrayElement& e);
        void visit(const MemberElement& e);
        void visit(const ObjectElement& e);

        ElementType get() const;

    };

}; // namespace refract

#endif // #ifndef _REFRACT_TYPEQUERYVISITOR_H_
