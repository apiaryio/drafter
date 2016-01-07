//
//  refract/TypeQueryVisitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_TYPEQUERYVISITOR_H
#define REFRACT_TYPEQUERYVISITOR_H

#include "Visitor.h"
#include "ElementFwd.h"

namespace refract
{

    class TypeQueryVisitor : public IVisitor
    {

    public:
        typedef enum
        {
            Null,

            String,
            Number,
            Boolean,

            Array,
            Member,
            Object,
            Enum,

            Extend,

            Unknown = 0,
        } ElementType;

    private:
        ElementType typeInfo;

    public:

        TypeQueryVisitor();

        void visit(const IElement& e);
        void visit(const NullElement& e);
        void visit(const StringElement& e);
        void visit(const NumberElement& e);
        void visit(const BooleanElement& e);
        void visit(const ArrayElement& e);
        void visit(const MemberElement& e);
        void visit(const ObjectElement& e);
        void visit(const EnumElement& e);
        void visit(const ExtendElement& e);

        ElementType get() const;

        template<typename E>
        static E* as(IElement* e)
        {
            if (!e) {
                return static_cast<E*>(e);
            }

            TypeQueryVisitor tq;
            tq.visit(*e);

            E type;
            TypeQueryVisitor eq;
            type.content(eq);

            if (eq.typeInfo != tq.typeInfo) {
                return 0;
            }

            return static_cast<E*>(e);
        }

        template<typename E>
        static const E* as(const IElement* e)
        {
            if (!e) {
                return static_cast<const E*>(e);
            }

            TypeQueryVisitor tq;
            tq.visit(*e);

            E type;
            TypeQueryVisitor eq;
            type.content(eq);

            if (eq.typeInfo != tq.typeInfo) {
                return 0;
            }

            return static_cast<const E*>(e);
        }

    };

}; // namespace refract

#endif // #ifndef REFRACT_TYPEQUERYVISITOR_H
