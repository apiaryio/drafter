//
//  parser/recovery.h
//  apib::parser
//
//  Created by Jiri Kratochvil on 27-01-2020
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//

#ifndef PARSER_RECOVER_H
#define PARSER_RECOVER_H

#include "../PEGTL/include/tao/pegtl.hpp"

#include <mpark/variant.hpp>

#include <string>
#include <map>

namespace apib
{
    namespace parser
    {

        //
        // recovery parsing of list
        // if list item is wrong defined it allows continue to parsing with next item 
        // based on idea https://github.com/taocpp/PEGTL/blob/2.x/src/example/pegtl/recover.cpp
        //

        namespace recover
        {

            using namespace tao;

            namespace impl
            {

                template <typename T>
                struct skip : pegtl::sor<
                                pegtl::plus<
                                  pegtl::seq<pegtl::not_at<T>, pegtl::any>
                                >,
                                pegtl::seq<>
                              > {};

                template <typename T>
                struct must_skip : pegtl::seq<
                                pegtl::plus<
                                  pegtl::seq<pegtl::not_at<T>, pegtl::any>
                                >,
                                pegtl::at<T>
                              > {};

                template <typename R, typename S, typename T>
                struct item : pegtl::must<
                                R, 
                                pegtl::at<
                                  pegtl::sor<S,T>
                                >
                              > {};


                template <typename R, typename S, typename T>
                struct head : pegtl::sor<
                                pegtl::try_catch<
                                  item<R,S,T>
                                >,
                                skip<
                                  pegtl::sor<S,T>
                                >
                              > {};

                template <typename R, typename S, typename T>
                struct tail : pegtl::seq<
                                pegtl::star<
                                  S,
                                  head<R,S,T>
                                >,
                                pegtl::at<T>
                              > {};

                template <typename R, typename T>
                struct content_with_terminator : pegtl::must<
                                R, 
                                pegtl::at<T>
                              > {};

            }; // namespace impl

            /*
             * R - parsing grammar for individual items
             * S - separator
             * T - terminator (is not consumed in by grammar, just ensure it is in place)
             *
             * allow parsing eg. `{ 1, 2, 3 }`
             * by grammar:
             * ```
             * struct G : seq<
             *              one<'{'>,
             *              recover::list<
             *                ascii::number, // grammar of item (one number)
             *                one<','>,      // separated by commas ','
             *                one<'}'>       // finished by '}'
             *              >,
             *              one<'}'>
             *            > {}
             */

            template <typename R, typename S, typename T>
            struct list : pegtl::seq<
                            impl::head<R,S,T>,
                            impl::tail<R,S,T>
                          > {

                using item = impl::item<R,S,T>;
                using invalid = impl::skip< pegtl::sor<S,T> >;

            };


            template <typename R, typename T>
            struct ensure : pegtl::sor<
                              pegtl::try_catch<
                                impl::content_with_terminator<R,T>
                              >,
                              impl::must_skip<T>
                            > {

                using content = impl::content_with_terminator<R,T>;
                using invalid = impl::must_skip<T>;

            };

        }; // namespace recover
    };     // namespace parsers
};         // namespace apib

#endif // PARSER_RECOVER_H
