//
//  refract/Utils.h
//  librefract
//
//  Created by Thomas Jandecka on 07/11/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_UTILS_H
#define REFRACT_UTILS_H

#include "ElementFwd.h"
#include "ElementIfc.h"

#include <iostream>
#include <memory>
#include <string>
#include <type_traits>

namespace refract
{

    struct IVisitor {
        virtual void operator()(const NullElement& e) = 0;
        virtual void operator()(const HolderElement& e) = 0;
        virtual void operator()(const StringElement& e) = 0;
        virtual void operator()(const NumberElement& e) = 0;
        virtual void operator()(const BooleanElement& e) = 0;
        virtual void operator()(const ArrayElement& e) = 0;
        virtual void operator()(const EnumElement& e) = 0;
        virtual void operator()(const MemberElement& e) = 0;
        virtual void operator()(const ObjectElement& e) = 0;
        virtual void operator()(const RefElement& e) = 0;
        virtual void operator()(const ExtendElement& e) = 0;
        virtual void operator()(const OptionElement& e) = 0;
        virtual void operator()(const SelectElement& e) = 0;
    };

    namespace impl
    {
        template <typename Ftor,
            typename Result = decltype(std::declval<Ftor>()(std::declval<BooleanElement&>())),
            bool = std::is_reference<Result>::value>
        struct state_functor : IVisitor {
            Ftor f;
            Result result = {}; // TODO @tjanc@ avoid default construction

            state_functor(Ftor pure) : f(std::move(pure)) {}

            Result consume()
            {
                return std::move(result);
            }

            void operator()(const NullElement& e) override
            {
                result = f(e);
            }
            void operator()(const HolderElement& e) override
            {
                result = f(e);
            }
            void operator()(const StringElement& e) override
            {
                result = f(e);
            }
            void operator()(const NumberElement& e) override
            {
                result = f(e);
            }
            void operator()(const BooleanElement& e) override
            {
                result = f(e);
            }
            void operator()(const ArrayElement& e) override
            {
                result = f(e);
            }
            void operator()(const EnumElement& e) override
            {
                result = f(e);
            }
            void operator()(const MemberElement& e) override
            {
                result = f(e);
            }
            void operator()(const ObjectElement& e) override
            {
                result = f(e);
            }
            void operator()(const RefElement& e) override
            {
                result = f(e);
            }
            void operator()(const ExtendElement& e) override
            {
                result = f(e);
            }
            void operator()(const OptionElement& e) override
            {
                result = f(e);
            }
            void operator()(const SelectElement& e) override
            {
                result = f(e);
            }
        };

        // specialization for reference results
        template <typename Ftor, typename Result>
        struct state_functor<Ftor, Result, true> : IVisitor {
            using functor_type = Ftor;
            using result_type = Result;
            using intermediate_type = typename std::add_pointer<Result>::type;

            Ftor f;
            intermediate_type result = {};

            state_functor(Ftor pure) : f(std::move(pure)) {}

            Result consume()
            {
                return *result;
            }

            void operator()(const NullElement& e) override
            {
                result = &f(e);
            }
            void operator()(const HolderElement& e) override
            {
                result = &f(e);
            }
            void operator()(const StringElement& e) override
            {
                result = &f(e);
            }
            void operator()(const NumberElement& e) override
            {
                result = &f(e);
            }
            void operator()(const BooleanElement& e) override
            {
                result = &f(e);
            }
            void operator()(const ArrayElement& e) override
            {
                result = &f(e);
            }
            void operator()(const EnumElement& e) override
            {
                result = &f(e);
            }
            void operator()(const MemberElement& e) override
            {
                result = &f(e);
            }
            void operator()(const ObjectElement& e) override
            {
                result = &f(e);
            }
            void operator()(const RefElement& e) override
            {
                result = &f(e);
            }
            void operator()(const ExtendElement& e) override
            {
                result = &f(e);
            }
            void operator()(const OptionElement& e) override
            {
                result = &f(e);
            }
            void operator()(const SelectElement& e) override
            {
                result = &f(e);
            }
        };

        // specialization for void results
        template <typename Ftor>
        struct state_functor<Ftor, void> : IVisitor {
            Ftor f;

            state_functor(Ftor pure) : f(std::move(pure)) {}

            void consume() const noexcept {}

            void operator()(const NullElement& e) override
            {
                f(e);
            }
            void operator()(const HolderElement& e) override
            {
                f(e);
            }
            void operator()(const StringElement& e) override
            {
                f(e);
            }
            void operator()(const NumberElement& e) override
            {
                f(e);
            }
            void operator()(const BooleanElement& e) override
            {
                f(e);
            }
            void operator()(const ArrayElement& e) override
            {
                f(e);
            }
            void operator()(const EnumElement& e) override
            {
                f(e);
            }
            void operator()(const MemberElement& e) override
            {
                f(e);
            }
            void operator()(const ObjectElement& e) override
            {
                f(e);
            }
            void operator()(const RefElement& e) override
            {
                f(e);
            }
            void operator()(const ExtendElement& e) override
            {
                f(e);
            }
            void operator()(const OptionElement& e) override
            {
                f(e);
            }
            void operator()(const SelectElement& e) override
            {
                f(e);
            }
        };
    }

    template <typename How,
        typename HowRef = How&,
        typename Result = typename std::result_of<HowRef(const NullElement&)>::type,
        typename = typename std::enable_if<std::is_void<Result>::value>::type>
    void visit(const IElement& ifc, How&& f)
    {
        using namespace impl;

        state_functor<How> stm(std::forward<How>(f));
        ifc.visit(stm);

        stm.consume();
    }

    template <typename How,
        typename HowRef = How&,
        typename Result = typename std::result_of<HowRef(const NullElement&)>::type,
        typename = typename std::enable_if<!std::is_void<Result>::value>::type,
        typename = void>
    Result visit(const IElement& ifc, How&& f)
    {
        using namespace impl;

        state_functor<How> stm(std::forward<How>(f));
        ifc.visit(stm);

        return stm.consume();
    }

    bool operator==(const IElement& lhs, const IElement& rhs) noexcept;
    bool operator!=(const IElement& lhs, const IElement& rhs) noexcept;

    bool operator==(const InfoElements& lhs, const InfoElements& rhs) noexcept;
    bool operator!=(const InfoElements& lhs, const InfoElements& rhs) noexcept;
}

#endif
