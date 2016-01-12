//
//  refract/Iterate.h
//  librefract
//
//  Created by Jiri Kratochvil on 12/01/16.
//  Copyright (c) 2016 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_ITERATE_H
#define REFRACT_ITERATE_H

#include "ElementFwd.h"

namespace refract
{

    struct Recursive {

        template <typename T, typename V = typename T::ValueType>
        struct Iterate {

            template <typename U, bool dummy = true>
            struct Impl {
                void operator()(ApplyVisitor&, const U&) {
                }
            };

            template <bool dummy>
            struct Impl <RefractElements, dummy> {
                void operator()(ApplyVisitor& v, const RefractElements& e) {
                    for (RefractElements::const_iterator i = e.begin() ; i != e.end() ; ++i) {
                        if (!(*i)) continue;
                        (*i)->content(v);
                    }
                }
            };

            template <bool dummy>
            struct Impl <MemberElement::ValueType, dummy> {
                void operator()(ApplyVisitor& v, const MemberElement::ValueType& e) {
                    if (e.first) {
                        e.first->content(v);
                    }

                    if (e.second) {
                        e.second->content(v);
                    }
                }
            };

            void operator()(ApplyVisitor& iterable, const T& e) {
                Impl<V> impl;
                impl(iterable ,e.value);
            }
        };

        template <typename T>
        void operator()(ApplyVisitor& iterable, const T& e) {
            Iterate<T> iterate;
            iterate(iterable, e);
        }

    };

    template <typename Strategy = Recursive>
    class Iterate {

        struct Impl {

            Strategy* strategy;
            ApplyVisitor* visitor;
            Apply* apply;

            void operator()(const IElement& e) {
                throw LogicError("'CatchAll' implementation should be never invoked");
            }

            template <typename T>
            void operator()(const T& e) {
                if (apply) {
                    apply->visit(e);
                }
                (*strategy)(*visitor, e);
            }
        };

        Impl impl;
        ApplyVisitor visitor;
        Strategy strategy;
        Apply* apply;

    public:

        Iterate() : impl(), visitor(impl), strategy(), apply(NULL) {
            impl.strategy = &strategy;
            impl.iterable = &visitor;
        }

        template <typename Functor>
        Iterate(Functor& functor) : impl(), visitor(impl), strategy(), apply(new ApplyImpl<Functor>(functor)) {
            impl.strategy = &strategy;
            impl.visitor = &visitor;
            impl.apply = apply;
        }

        ~Iterate() {
            if (apply) {
                delete apply;
            }
        }

        void operator()(const IElement& e) {
            visitor.visit(e);
        }
    };


}; // namespace refract

#endif // #ifndef REFRACT_ITERATE_H
