//
//  parser/Uritemplate.h
//  apib::parser
//
//  Created by Jiri Kratochvil on 02/12/2019
//  Copyright (c) 2019 Apiary Inc. All rights reserved.
//

#ifndef PARSER_URITEMPLATE_H
#define PARSER_URITEMPLATE_H

#include "../PEGTL/include/tao/pegtl.hpp"
#include "../PEGTL/include/tao/pegtl/contrib/abnf.hpp"

#include "recover.h"

#include <mpark/variant.hpp>

#include <string>
#include <map>

#include <iostream>

namespace apib
{
    namespace parser
    {

        //
        // Parser for URI Templates
        //
        // https://tools.ietf.org/html/rfc6570
        //

        namespace uritemplate
        {

            namespace pegtl = tao::pegtl;
            
            // try_catch because we want to continue by recovery - so we need local failure instead of global
            struct pct_encoded : pegtl::try_catch< 
                                   pegtl::if_must<
                                     pegtl::one< '%' >,
                                     pegtl::abnf::HEXDIG,
                                     pegtl::abnf::HEXDIG
                                   >
                                 > {};

            struct ucschar : pegtl::utf8::ranges<
                    0x000A0,0x0D7FF, 0x0F900,0x0FDCF, 0x0FDF0,0x0FFEF,
                    0x10000,0x1FFFD, 0x20000,0x2FFFD, 0x30000,0x3FFFD,
                    0x40000,0x4FFFD, 0x50000,0x5FFFD, 0x60000,0x6FFFD,
                    0x70000,0x7FFFD, 0x80000,0x8FFFD, 0x90000,0x9FFFD,
                    0xA0000,0xAFFFD, 0xB0000,0xBFFFD, 0xC0000,0xCFFFD,
                    0xD0000,0xDFFFD, 0xE1000,0xEFFFD
                    > {};

            struct iprivate : pegtl::utf8::ranges<
                    0xE000,0xF8FF,   0xF0000,0xFFFFD, 0x100000,0x10FFFD
                              > {};

            struct op_reserved_char : pegtl::one<'+'> {};
            struct op_fragment : pegtl::one<'#'> {};

            struct op_label : pegtl::one<'.'> {};
            struct op_path : pegtl::one<'/'> {};
            struct op_path_param : pegtl::one<';'> {};
            struct op_query_param : pegtl::one<'?'> {};
            struct op_query_continue : pegtl::one<'&'> {};

            struct op_l2 : pegtl::sor<
                             op_reserved_char,
                             op_fragment
                           > {};

            struct op_l3 : pegtl::sor<
                             op_label,
                             op_path,
                             op_path_param,
                             op_query_param,
                             op_query_continue
                           > {};

            struct op_reserved : pegtl::one<'=', ',', '!', '@', '|'> {};

            struct expression_open : pegtl::one<'{'> {};
            struct expression_close : pegtl::one<'}'> {};

            struct operator_ : pegtl::sor<op_l2, op_l3, op_reserved> {};

            struct prefix_value : pegtl::rep_max<3, pegtl::abnf::DIGIT> {};
            struct prefix : pegtl::seq<pegtl::one<':'>, prefix_value> {};
            struct explode : pegtl::one<'*'> {};

            struct mod_l4 : pegtl::sor<prefix, explode> {};

            struct varchar : pegtl::sor<
                               pct_encoded,
                               pegtl::abnf::ALPHA,
                               pegtl::abnf::DIGIT,
                               pegtl::one<'_'>
                             > {};

            struct noncontinuous_dots : pegtl::try_catch<
                                          pegtl::if_must<
                                            pegtl::one<'.'>,
                                            pegtl::not_at<pegtl::one<'.'>>
                                          >
                                        > {};

            struct varname : pegtl::seq<
                               varchar,
                               pegtl::star<
                                 pegtl::sor< // allow single dots - not continuous
                                   noncontinuous_dots,
                                   varchar
                                 >
                               >
                             > {};

            struct varspec : pegtl::seq<varname, pegtl::opt<mod_l4>> {};

            struct variable_list : recover::list<
                                     varspec,
                                     pegtl::one<','>,
                                     pegtl::sor<
                                       expression_close,
                                       pegtl::eof
                                     >
                                   > {};

            /**
             * dummy token to handle missing close bracket and avoid global failure while parsing
             */
            struct missing_expression_close : pegtl::seq<> {};

            struct expression : pegtl::if_must<
                                  expression_open,
                                  pegtl::opt<operator_>,
                                  variable_list,
                                  pegtl::sor<
                                    expression_close, missing_expression_close
                                  >
                                > {};

            /**
             *FIXME: character set allowed by RFC 6570 for literals:
             *         %x21 / %x23-24 / %x26 / %x28-3B / %x3D / %x3F-5B
             *      /  %x5D / %x5F / %x61-7A / %x7E / ucschar / iprivate
             *      /  pct-encoded
             *           ; any Unicode character except: CTL, SP,
             *           ;  DQUOTE, "'", "%" (aside from pct-encoded),
             *           ;  "<", ">", "\", "^", "`", "{", "|", "}"
             *
             *  But this collide with RFC1738 - Section 2.2 where are defined "unsafe characters"
             *  which must be pct-encoded.
             *
             *  These characters are "{", "}", "|", "\", "^", "~", "[", "]", and "`".
             *  Coded as in RFC5760:
             *  %x7B / %x7D / %x7C / %x5C / %x5E / %x7E / %x5B / %x5D / %x60
             *
             *  So we modify original literal set from RFC6570 and remove them from allowed chars.
             *
             *  Original set is named `rfc_6570_ascii_allowed`
             *  while modified is named `ascii_allowed`
             *
             *  From `ascii_allowed` are removed chars to be RFC1738 compatible
             *  "~", "[" , "]"
             *  %x7E / %x5B / %x5D 
             *
             */

            struct rfc_6570_ascii_allowed : pegtl::sor<
                                     pegtl::one<0x21>,
                                     pegtl::range<0x23, 0x24>,
                                     pegtl::one<0x26>,
                                     pegtl::range<0x28, 0x3b>,
                                     pegtl::one<0x3d>,
                                     pegtl::range<0x3f, 0x5b>,
                                     pegtl::one<0x5d>,
                                     pegtl::one<0x5f>,
                                     pegtl::range<0x61, 0x7a>,
                                     pegtl::one<0x7e>
                                   > {};

            struct ascii_allowed : pegtl::sor<
                                     pegtl::one<0x21>,
                                     pegtl::range<0x23, 0x24>,
                                     pegtl::one<0x26>,
                                     pegtl::range<0x28, 0x3b>,
                                     pegtl::one<0x3d>,
                                     pegtl::range<0x3f, 0x5a>,
                                     pegtl::one<0x5f>,
                                     pegtl::range<0x61, 0x7a>
                                   > {};

            struct literals_ : pegtl::plus<
                                pegtl::sor<
                                  pct_encoded,
                                  ascii_allowed,
                                  ucschar,
                                  iprivate
                                >
                              > {};


            struct literals : recover::ensure<
                                literals_,
                                pegtl::sor<
                                  expression_open,
                                  pegtl::eof
                                >
                              > {};

            struct grammar : pegtl::plus<
                                 pegtl::sor<
                                  literals,
                                  expression
                                 >
                             > {};

            struct match_grammar : pegtl::seq<grammar, pegtl::eof>{};

            
            namespace state {
                using literals = std::string;

                struct variable {
                    std::string name;
                    size_t prefix = 0;
                    bool explode = false;
                };

                enum class expression_type {
                    noop,
                    reserved_chars,
                    fragment,
                    label,
                    path,
                    path_param,
                    query_param,
                    query_continue,
                    reserved
                };

                struct invalid {
                    std::string content;
                    size_t position = 0;
                };


                struct expression {
                    using variable_type = mpark::variant<variable, invalid>;

                    expression_type type = expression_type::noop;
                    std::vector<variable_type> variables;
                    bool missing_expression_close = false;
                };


                using part = mpark::variant<
                    mpark::monostate,
                    literals,
                    expression,
                    invalid
                >;

                using uritemplate = std::vector<state::part>;
            };


            template <typename Rule>
            struct action : pegtl::nothing<Rule> {
            };


            template <>
            struct action<literals::content> : pegtl::change_states<state::literals> {
                template<typename Input>
                static void apply(const Input& in, state::literals& s)
                {
                    s = std::move(in.string());
                }

                template<typename Input>
                static void success( const Input& in, state::literals& s, state::uritemplate& t) {
                    t.emplace_back(std::move(s));
                };
            }; 

            template <>
            struct action<literals::invalid> : pegtl::change_states<state::invalid> {
                template<typename Input>
                static void apply(const Input& in, state::invalid& s)
                {
                    s.content = std::move(in.string());
                    s.position = in.position().byte;
                }

                template<typename Input>
                static void success( const Input& in, state::invalid& s, state::uritemplate& t) {
                    t.emplace_back(std::move(s));
                };
            };


            template <>
            struct action<expression> : pegtl::change_states<state::expression> {
                template<typename Input>
                static void success( const Input& in, state::expression& s, state::uritemplate& t) {
                    t.emplace_back(state::part{s});
                };
            }; 

            template <>
            struct action<variable_list::item> : pegtl::change_states<state::variable> {
                template<typename Input>
                static void success( const Input& in, state::variable& s, state::expression& t) {
                    t.variables.emplace_back(s);
                };
            }; 

            template <>
            struct action<variable_list::invalid> : pegtl::change_states<state::invalid> {
                template<typename Input>
                static void apply(const Input& in, state::invalid& s)
                {
                    s.content = std::move(in.string());
                    s.position = in.position().byte;
                }

                template<typename Input>
                static void success( const Input& in, state::invalid& s, state::expression& t) {
                    t.variables.emplace_back(s);
                };
            }; 
            
            template <>
            struct action<varname> {
                template <typename Input>
                static void apply(const Input& in, state::variable& s)
                {
                    s.name = std::move(in.string());
                }
            };

            template <>
            struct action<prefix_value> {
                template <typename Input>
                static void apply(const Input& in, state::variable& s)
                {
                    s.prefix = std::atol(in.string().c_str());
                }
            };

            template <>
            struct action<explode> {
                template <typename Input>
                static void apply(const Input& in, state::variable& s)
                {
                    s.explode = true;
                }
            };

            template <typename Rule, state::expression_type Operator>
            struct set_operator { 
                template <typename Input>
                static void apply(const Input& in, state::expression& s)
                {
                    s.type = Operator;
                }
            };

            template<> struct action<op_reserved_char> : set_operator<op_reserved, state::expression_type::reserved_chars> {};
            template<> struct action<op_fragment> : set_operator<op_fragment, state::expression_type::fragment> {};
            template<> struct action<op_label> : set_operator<op_label, state::expression_type::label> {};
            template<> struct action<op_path> : set_operator<op_path, state::expression_type::path> {};
            template<> struct action<op_path_param> : set_operator<op_path_param, state::expression_type::path_param> {};
            template<> struct action<op_query_param> : set_operator<op_query_param, state::expression_type::query_param> {};
            template<> struct action<op_query_continue> : set_operator<op_query_continue, state::expression_type::query_continue> {};
            template<> struct action<op_reserved> : set_operator<op_reserved, state::expression_type::reserved> {};

            template <>
            struct action<missing_expression_close> {
                static void apply0(state::expression& s) {
                    s.missing_expression_close = true;
                };
            }; 



        }; // namespace uritemplate
    };     // namespace parsers
};         // namespace apib

#endif // PARSER_URITEMPLATE_H
