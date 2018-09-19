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
#include "ElementIfc.h"

namespace refract
{

    struct IApply {
        virtual void visit(const IElement& e) = 0;

        virtual void visit(const NullElement& e) = 0;
        virtual void visit(const HolderElement& e) = 0;
        virtual void visit(const StringElement& e) = 0;
        virtual void visit(const NumberElement& e) = 0;
        virtual void visit(const BooleanElement& e) = 0;
        virtual void visit(const ArrayElement& e) = 0;
        virtual void visit(const EnumElement& e) = 0;
        virtual void visit(const MemberElement& e) = 0;
        virtual void visit(const ObjectElement& e) = 0;
        virtual void visit(const RefElement& e) = 0;
        virtual void visit(const ExtendElement& e) = 0;
        virtual void visit(const OptionElement& e) = 0;
        virtual void visit(const SelectElement& e) = 0;

        virtual ~IApply() {}
    };

#ifndef APPLY_VISIT_IMPL
#define APPLY_VISIT_IMPL(ELEMENT)                                                                                      \
    virtual void visit(const ELEMENT& e)                                                                               \
    {                                                                                                                  \
        impl(e);                                                                                                       \
    }
#endif

    template <typename Impl>
    struct ApplyImpl : public IApply {

        Impl& impl;
        ApplyImpl(Impl& impl) : impl(impl) {}

        APPLY_VISIT_IMPL(IElement)
        APPLY_VISIT_IMPL(NullElement)
        APPLY_VISIT_IMPL(HolderElement)
        APPLY_VISIT_IMPL(StringElement)
        APPLY_VISIT_IMPL(NumberElement)
        APPLY_VISIT_IMPL(BooleanElement)
        APPLY_VISIT_IMPL(ArrayElement)
        APPLY_VISIT_IMPL(EnumElement)
        APPLY_VISIT_IMPL(MemberElement)
        APPLY_VISIT_IMPL(ObjectElement)
        APPLY_VISIT_IMPL(RefElement)
        APPLY_VISIT_IMPL(ExtendElement)
        APPLY_VISIT_IMPL(OptionElement)
        APPLY_VISIT_IMPL(SelectElement)

        virtual ~ApplyImpl() {}
    };

#undef APPLY_VISIT_IMPL

    class Visitor
    {

    private:
        IApply* apply;

    public:
        template <typename Functor>
        Visitor(Functor& functor) : apply(new ApplyImpl<Functor>(functor))
        {
        }
        virtual ~Visitor()
        {
            delete apply;
        }

        template <typename T>
        void visit(const T& e)
        {
            apply->visit(e);
        }
    };

    template <typename Element, typename Functor>
    void Visit(Functor& functor, const Element& element)
    {
        Visitor visitor(functor);
        visitor.visit(element);
    }

    template <typename Element, typename Functor>
    void VisitBy(const Element& element, Functor& functor)
    {
        Visitor visitor(functor);
        element.content(visitor);
    }

}; // namespace refract

#endif // #ifndef REFRACT_VISITOR_H
