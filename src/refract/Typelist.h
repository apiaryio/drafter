//
//  refract/Typelist.h
//  librefract
//
//  Based on concept of typelist introduced in
//  http://www.drdobbs.com/generic-programmingtypelists-and-applica/184403813
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_TYPELIST_H
#define REFRACT_TYPELIST_H

namespace typelist {
    struct null_type {};
    typedef null_type N;

    template <typename H, typename T>
    struct typelist {
        typedef H head;
        typedef T tail;
    };

    template <typename T1,
             typename T2 = N, typename T3 = N, typename T4 = N, typename T5 = N,
             typename T6 = N, typename T7 = N, typename T8 = N, typename T9 = N,
             typename T10 = N, typename T11 = N, typename T12 = N, typename T13 = N>
    struct cons;

    template<typename T1>
    struct cons <T1, N, N, N, N, N, N, N, N, N, N, N, N> {
        typedef typelist<T1, N> type;
    };

    template<typename T1, typename T2>
    struct cons <T1, T2, N, N, N, N, N, N, N, N, N, N, N> {
        typedef typelist<T1, typelist<T2, N> > type;
    };

    template<typename T1, typename T2, typename T3>
    struct cons <T1, T2, T3, N, N, N, N, N, N, N, N, N, N> {
        typedef typelist<T1, typelist<T2, typelist<T3, N> > > type;
    };

    template<typename T1, typename T2, typename T3, typename T4>
    struct cons <T1, T2, T3, T4, N, N, N, N, N, N, N, N, N> {
        typedef typelist<T1, typelist<T2, typelist<T3, typelist<T4, N > > > > type;
    };

    template<typename T1, typename T2, typename T3, typename T4, typename T5>
    struct cons <T1, T2, T3, T4, T5, N, N, N, N, N, N, N, N> {
        typedef typelist<T1, typelist<T2, typelist<T3, typelist<T4, typelist<T5, N> > > > > type;
    };

    template<typename T1, typename T2, typename T3, typename T4, typename T5,
             typename T6>
    struct cons <T1, T2, T3, T4, T5, T6, N, N, N, N, N, N, N> {
        typedef typelist<T1, typelist<T2, typelist<T3, typelist<T4, typelist<T5,
                typelist<T6, N> > > > > > type;
    };

    template<typename T1, typename T2, typename T3, typename T4, typename T5,
             typename T6, typename T7>
    struct cons <T1, T2, T3, T4, T5, T6, T7, N, N, N, N, N, N> {
        typedef typelist<T1, typelist<T2, typelist<T3, typelist<T4, typelist<T5,
                typelist<T6, typelist<T7, N> > > > > > > type;
    };

    template<typename T1, typename T2, typename T3, typename T4, typename T5,
             typename T6, typename T7, typename T8>
    struct cons <T1, T2, T3, T4, T5, T6, T7, T8, N, N, N, N, N> {
        typedef typelist<T1, typelist<T2, typelist<T3, typelist<T4, typelist<T5,
                typelist<T6, typelist<T7, typelist<T8, N> > > > > > > > type;
    };

    template<typename T1, typename T2, typename T3, typename T4, typename T5,
             typename T6, typename T7, typename T8, typename T9>
    struct cons <T1, T2, T3, T4, T5, T6, T7, T8, T9, N, N, N, N> {
        typedef typelist<T1, typelist<T2, typelist<T3, typelist<T4, typelist<T5,
                typelist<T6, typelist<T7, typelist<T8, typelist<T9, N> > > > > > > > > type;
    };

    template<typename T1, typename T2, typename T3, typename T4, typename T5,
             typename T6, typename T7, typename T8, typename T9, typename T10>
    struct cons <T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, N, N, N> {
        typedef typelist<T1, typelist<T2, typelist<T3, typelist<T4, typelist<T5,
                typelist<T6, typelist<T7, typelist<T8, typelist<T9, typelist<T10, N> > > > > > > > > > type;
    };

    template<typename T1, typename T2, typename T3, typename T4, typename T5,
             typename T6, typename T7, typename T8, typename T9, typename T10,
             typename T11>
    struct cons <T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, N, N> {
        typedef typelist<T1, typelist<T2, typelist<T3, typelist<T4, typelist<T5,
                typelist<T6, typelist<T7, typelist<T8, typelist<T9, typelist<T10, 
                typelist<T11, N> > > > > > > > > > > type;
    };

    template<typename T1, typename T2, typename T3, typename T4, typename T5,
             typename T6, typename T7, typename T8, typename T9, typename T10,
             typename T11, typename T12>
    struct cons <T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, N> {
        typedef typelist<T1, typelist<T2, typelist<T3, typelist<T4, typelist<T5,
                typelist<T6, typelist<T7, typelist<T8, typelist<T9, typelist<T10, 
                typelist<T11, typelist <T12, N> > > > > > > > > > > > type;
    };

    template<typename T1, typename T2, typename T3, typename T4, typename T5,
             typename T6, typename T7, typename T8, typename T9, typename T10,
             typename T11, typename T12, typename T13>
    struct cons {
        typedef typelist<T1, typelist<T2, typelist<T3, typelist<T4, typelist<T5,
                typelist<T6, typelist<T7, typelist<T8, typelist<T9, typelist<T10, 
                typelist<T11, typelist <T12, typelist <T13, N> > > > > > > > > > > > > type;
    };

};

#endif // #ifndef REFRACT_TYPELIST_H
