//
//  refract/TypeQueryVisitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_TYPEQUERYVISITOR_H
#define REFRACT_TYPEQUERYVISITOR_H

#include "ElementFwd.h"
#include "Visitor.h"

namespace refract
{

    class TypeQueryVisitor 
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

            Option,
            Select,

            Unknown = 0,
        } ElementType;

    private:
        ElementType typeInfo;

    public:

        TypeQueryVisitor();

        void operator()(const IElement& e);
        void operator()(const NullElement& e);
        void operator()(const StringElement& e);
        void operator()(const NumberElement& e);
        void operator()(const BooleanElement& e);
        void operator()(const ArrayElement& e);
        void operator()(const MemberElement& e);
        void operator()(const ObjectElement& e);
        void operator()(const EnumElement& e);
        void operator()(const ExtendElement& e);
        void operator()(const OptionElement& e);
        void operator()(const SelectElement& e);

        ElementType get() const;

        template<typename E>
        static E* as(IElement* e)
        {
            if (!e) {
                return static_cast<E*>(e);
            }

            TypeQueryVisitor tq;
            Visit(tq, *e);

            E type;
            TypeQueryVisitor eq;
            VisitBy(type, eq);

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
            Visit(tq, *e);

            E type;
            TypeQueryVisitor eq;
            VisitBy(type, eq);

            if (eq.typeInfo != tq.typeInfo) {
                return 0;
            }

            return static_cast<const E*>(e);
        }

    };

}; // namespace refract

#endif // #ifndef REFRACT_TYPEQUERYVISITOR_H
