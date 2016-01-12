//
//  refract/ApplyVisitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_APPLYVISITOR_H
#define REFRACT_APPLYVISITOR_H

#include "Visitor.h"
#include "ElementFwd.h"

namespace refract
{

    struct Apply {
        virtual void visit(const IElement& e) = 0;

        virtual void visit(const NullElement& e) = 0;
        virtual void visit(const StringElement& e) = 0;
        virtual void visit(const NumberElement& e) = 0;
        virtual void visit(const BooleanElement& e) = 0;
        virtual void visit(const ArrayElement& e) = 0;
        virtual void visit(const EnumElement& e) = 0;
        virtual void visit(const MemberElement& e) = 0;
        virtual void visit(const ObjectElement& e) = 0;
        virtual void visit(const ExtendElement& e) = 0;

        virtual ~Apply() {}
    };

    template <typename Impl>
    struct ApplyImpl : public Apply {

        Impl& impl;
        ApplyImpl(Impl& impl) : impl(impl) {}

        virtual void visit(const IElement& e) {
            impl(e);
        }

        virtual void visit(const NullElement& e) {
            impl(e);
        }

        virtual void visit(const StringElement& e) {
            impl(e);
        }

        virtual void visit(const NumberElement& e) {
            impl(e);
        }

        virtual void visit(const BooleanElement& e) {
            impl(e);
        }

        virtual void visit(const ArrayElement& e) {
            impl(e);
        }

        virtual void visit(const EnumElement& e) {
            impl(e);
        }

        virtual void visit(const MemberElement& e) {
            impl(e);
        }

        virtual void visit(const ObjectElement& e) {
            impl(e);
        }

        virtual void visit(const ExtendElement& e) {
            impl(e);
        }

        virtual ~ApplyImpl() {}
    };

    class ApplyVisitor : public IVisitor
    {

    private:

        Apply* apply;

    public:

        template <typename Functor>
        ApplyVisitor(Functor& functor) : apply(new ApplyImpl<Functor>(functor)) {}
        virtual ~ApplyVisitor();

        template<typename T>
        void visit(const T& e);

    };

}; // namespace refract

#endif // #ifndef REFRACT_APPLYVISITOR_H
