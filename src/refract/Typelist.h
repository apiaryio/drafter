#if !defined _REFRACT_TYPELIST_H_
#define _REFRACT_TYPELIST_H_

namespace typelist {
    struct null_type {};

    template <typename H, typename T>
    struct typelist {
        typedef H head;
        typedef T tail;
    };

    template <typename T1, 
             typename T2 = null_type, typename T3 = null_type, typename T4 = null_type, typename T5 = null_type,
             typename T6 = null_type, typename T7 = null_type, typename T8 = null_type, typename T9 = null_type>
    struct cons;

    template<typename T1>
    struct cons <T1, null_type, null_type, null_type, null_type, null_type, null_type, null_type, null_type>{
        typedef typelist<T1, null_type> type;
    };

    template<typename T1, typename T2>
    struct cons <T1, T2, null_type, null_type, null_type, null_type, null_type, null_type, null_type>{
        typedef typelist<T1, typelist<T2, null_type> > type;
    };

    template<typename T1, typename T2, typename T3>
    struct cons <T1, T2, T3, null_type, null_type, null_type, null_type, null_type, null_type>{
        typedef typelist<T1, typelist<T2, typelist<T3, null_type> > > type;
    };

    template<typename T1, typename T2, typename T3, typename T4>
    struct cons <T1, T2, T3, T4, null_type, null_type, null_type, null_type, null_type>{
        typedef typelist<T1, typelist<T2, typelist<T3, typelist<T4, null_type > > > > type;
    };

    template<typename T1, typename T2, typename T3, typename T4, typename T5>
    struct cons <T1, T2, T3, T4, T5, null_type, null_type, null_type, null_type>{
        typedef typelist<T1, typelist<T2, typelist<T3, typelist<T4, typelist<T5, null_type> > > > > type;
    };

    template<typename T1, typename T2, typename T3, typename T4, typename T5,
             typename T6>
    struct cons <T1, T2, T3, T4, T5, T6, null_type, null_type, null_type>{
        typedef typelist<T1, typelist<T2, typelist<T3, typelist<T4, typelist<T5, 
                typelist<T6, null_type> > > > > > type;
    };

    template<typename T1, typename T2, typename T3, typename T4, typename T5,
             typename T6, typename T7>
    struct cons <T1, T2, T3, T4, T5, T6, T7, null_type, null_type>{
        typedef typelist<T1, typelist<T2, typelist<T3, typelist<T4, typelist<T5, 
                typelist<T6, typelist<T7, null_type> > > > > > > type;
    };

    template<typename T1, typename T2, typename T3, typename T4, typename T5,
             typename T6, typename T7, typename T8>
    struct cons <T1, T2, T3, T4, T5, T6, T7, T8, null_type> {
        typedef typelist<T1, typelist<T2, typelist<T3, typelist<T4, typelist<T5, 
                typelist<T6, typelist<T7, typelist<T8, null_type> > > > > > > > type;
    };

    template<typename T1, typename T2, typename T3, typename T4, typename T5,
             typename T6, typename T7, typename T8, typename T9>
    struct cons {
        typedef typelist<T1, typelist<T2, typelist<T3, typelist<T4, typelist<T5, 
                typelist<T6, typelist<T7, typelist<T8, typelist<T9, null_type> > > > > > > > > type;
    };
};

#endif // #if !defined _REFRACT_TYPELIST_H_
