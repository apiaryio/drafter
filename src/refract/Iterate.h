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
                void operator()(IApply* apply, Visitor&, const U&) {
                }
            };

            template <bool dummy>
            struct Impl <RefractElements, dummy> {
                void operator()(IApply* apply, Visitor& v, const RefractElements& e) {
                    for (RefractElements::const_iterator i = e.begin() ; i != e.end() ; ++i) {
                        if (!(*i)) continue;
                        (*i)->content(v);
                    }
                }
            };

            template <bool dummy>
            struct Impl <MemberElement::ValueType, dummy> {
                void operator()(IApply* apply, Visitor& v, const MemberElement::ValueType& e) {
                    if (e.first) {
                        e.first->content(v);
                    }

                    if (e.second) {
                        e.second->content(v);
                    }
                }
            };

            void operator()(IApply* apply, Visitor& iterable, const T& e) {
                apply->visit(e);
                Impl<V>()(apply, iterable ,e.value);
            }
        };

        template <typename T>
        void operator()(IApply* apply, Visitor& iterable, const T& e) {
            Iterate<T>()(apply, iterable, e);
        }

    };

    struct Children {

        template <typename T, typename V = typename T::ValueType>
        struct Iterate {

            template <typename U, bool dummy = true>
            struct Impl {
                void operator()(Children* strategy, IApply* apply, Visitor&, const U&) {
                }
            };

            template <bool dummy>
            struct Impl <RefractElements, dummy> {
                void operator()(Children* strategy, IApply* apply, Visitor& v, const RefractElements& e) {
                    if (strategy->level) { // we need no go deeply
                        return;
                    }

                    strategy->level++;
                    for (RefractElements::const_iterator i = e.begin() ; i != e.end() ; ++i) {
                        if (!(*i)) continue;
                        (*i)->content(v);
                    }
                    strategy->level--;
                }
            };

            void operator()(Children* strategy, IApply* apply, Visitor& iterable, const T& e) {
                Impl<V>()(strategy, apply, iterable ,e.value);
            }
        };

        int level;
        Children() : level(0) {}

        template <typename T>
        void operator()(IApply* apply, Visitor& iterable, const T& e) {
            if (level == 1) {
                apply->visit(e);
            }
            Iterate<T>()(this, apply, iterable, e);
        }

    };

    template <typename Strategy = Recursive>
    class Iterate {

        struct Impl {

            Strategy* strategy;
            Visitor* iterator;
            IApply* apply;

            void operator()(const IElement& e) {
                // redirect to concrete override
                e.content(*iterator);
            }

            template <typename T>
            void operator()(const T& e) {
                if (!apply) {
                    return;
                    //apply->visit(e);
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
        explicit Iterate(Functor& functor) : impl(), iterator(impl), strategy(), apply(new ApplyImpl<Functor>(functor)) {
            impl.strategy = &strategy;
            impl.iterator = &iterator;
            impl.apply = apply;
        }

        ~Iterate() {
            if (apply) {
                delete apply;
            }
        }

        void operator()(const IElement& e) {
            iterator.visit(e);
        }
    };


}; // namespace refract

#endif // #ifndef REFRACT_ITERATE_H
