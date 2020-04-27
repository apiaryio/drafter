//
//  refract/Iterate.h
//  librefract
//
//  Created by Jiri Kratochvil on 12/01/16.
//  Copyright (c) 2016 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_ITERATE_H
#define REFRACT_ITERATE_H

#include "ElementIfc.h"
#include "ElementFwd.h"
#include "Visitor.h"
#include "dsd/Traits.h"

namespace refract
{
    struct Recursive {

        template <typename T, typename V = typename T::ValueType>
        struct Iterate {

            template <typename U, bool IsIterable = dsd::is_iterable<U>::value, bool IsPair = dsd::is_pair<U>::value>
            struct Impl {
                void operator()(IApply* apply, Visitor&, const U&) {}
            };

            template <typename U, bool IsPair>
            struct Impl<U, true, IsPair> {
                void operator()(IApply* apply, Visitor& v, const U& e)
                {
                    for (const auto& child : e) {
                        if (!child)
                            continue;
                        child->content(v);
                    }
                }
            };

            template <typename U, bool IsIterable>
            struct Impl<U, IsIterable, true> {
                void operator()(IApply* apply, Visitor& v, const U& e)
                {
                    if (auto key = e.key()) {
                        key->content(v);
                    }

                    if (auto value = e.value()) {
                        value->content(v);
                    }
                }
            };

            void operator()(IApply* apply, Visitor& iterable, const T& e)
            {
                apply->visit(e);

                if (!e.empty())
                    Impl<V>()(apply, iterable, e.get());
            }
        };

        template <typename T>
        void operator()(IApply* apply, Visitor& iterable, const T& e)
        {
            Iterate<T>()(apply, iterable, e);
        }
    };

    struct Children {

        template <typename T, typename V = typename T::ValueType>
        struct Iterate {

            template <typename U, bool IsIterable = dsd::is_iterable<U>::value>
            struct Impl {
                void operator()(Children* strategy, IApply* apply, Visitor&, const U&) {}
            };

            template <typename U>
            struct Impl<U, true> {
                void operator()(Children* strategy, IApply* apply, Visitor& v, const U& e)
                {
                    if (strategy->level) { // we need no go deeply
                        return;
                    }

                    strategy->level++;
                    for (const auto& el : e) {
                        if (!el) // decide whether this is an assert
                            continue;
                        el->content(v);
                    }
                    strategy->level--;
                }
            };

            void operator()(Children* strategy, IApply* apply, Visitor& iterable, const T& e)
            {
                Impl<V>()(strategy, apply, iterable, e.get());
            }
        };

        int level;
        Children() : level(0) {}

        template <typename T>
        void operator()(IApply* apply, Visitor& iterable, const T& e)
        {
            if (level == 1) {
                apply->visit(e);
            }
            Iterate<T>()(this, apply, iterable, e);
        }
    };

    template <typename Strategy = Recursive>
    class Iterate
    {

        struct Impl {

            Strategy* strategy;
            Visitor* iterator;
            IApply* apply;

            void operator()(const IElement& e)
            {
                // redirect to concrete override
                e.content(*iterator);
            }

            template <typename T>
            void operator()(const T& e)
            {
                if (!apply) {
                    return;
                    // apply->visit(e);
                }
                (*strategy)(apply, *iterator, e);
            }
        };

        Impl impl;
        Visitor iterator;
        Strategy strategy;
        IApply* apply;

    public:
        template <typename Functor>
        explicit Iterate(Functor& functor) : impl(), iterator(impl), strategy(), apply(new ApplyImpl<Functor>(functor))
        {
            impl.strategy = &strategy;
            impl.iterator = &iterator;
            impl.apply = apply;
        }

        ~Iterate()
        {
            if (apply) {
                delete apply;
            }
        }

        void operator()(const IElement& e)
        {
            iterator.visit(e);
        }
    };

}; // namespace refract

#endif // #ifndef REFRACT_ITERATE_H
