//
//  refract/Visitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_VISITOR_H
#define REFRACT_VISITOR_H

#include "ElementFwd.h"

namespace refract
{

    struct IApply {
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
        virtual void visit(const OptionElement& e) = 0;
        virtual void visit(const SelectElement& e) = 0;

        virtual ~IApply() {}
    };

    template <typename Impl>
    struct ApplyImpl : public IApply {

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

        virtual void visit(const OptionElement& e) {
            impl(e);
        }

        virtual void visit(const SelectElement& e) {
            impl(e);
        }

        virtual ~ApplyImpl() {}
    };

    class Visitor {

    private:

        IApply* apply;

    public:

        template <typename Functor>
        Visitor(Functor& functor) : apply(new ApplyImpl<Functor>(functor)) {}
        virtual ~Visitor() {
            delete apply;
        }

        template<typename T>
        void visit(const T& e) {
            apply->visit(e);
        }

    };

    template <typename Element, typename Functor>
    void Visit(Functor& functor, const Element& element) {
        Visitor visitor(functor);
        visitor.visit(element);
    }

    template <typename Element, typename Functor>
    void VisitBy(const Element& element, Functor& functor) {
        Visitor visitor(functor);
        element.content(visitor);
    }

}; // namespace refract

#endif // #ifndef REFRACT_VISITOR_H
