//
//  parser/Mediatype.h
//  apib::parser
//
//  Created by Jiri Kratochvil on 02/08/2019
//  Copyright (c) 2019 Apiary Inc. All rights reserved.
//

#ifndef PARSER_MEDIATYPE_H
#define PARSER_MEDIATYPE_H

#include "../PEGTL/include/tao/pegtl.hpp"
#include "../PEGTL/include/tao/pegtl/contrib/abnf.hpp"

#include <string>
#include <map>

namespace apib
{
    namespace parser
    {

        //
        // Parsed for Media Types
        // - type, subtype, suffix, paramemters attributes conforms to https://tools.ietf.org/html/rfc6838#section-4.2
        // - parameter values conforms to https://tools.ietf.org/html/rfc2616#section-3.7
        //

        namespace mediatype
        {

            namespace pegtl = tao::pegtl;

            // clang-format off

        struct restricted : pegtl::one< '!', '#', '$', '&', '^',  '_', '-', '.' >  {};

        using WS = pegtl::abnf::WSP; // White Space

        using RWS = pegtl::plus<WS>; // Required White Space
        using OWS = pegtl::star<WS>; // Optional White Space

        using obs_text = pegtl::not_range< 0x00, 0x7F >;

        struct quoted_pair : pegtl::if_must<
                                 pegtl::one<'\\'>,
                                 pegtl::sor<
                                     pegtl::abnf::VCHAR,
                                     obs_text,
                                     pegtl::abnf::WSP
                                 >
                             > {};

        // OPT: pegtl::if_must<> - if we want to throw on non-closed quotes
        struct quoted : pegtl::seq<
                            pegtl::one<'"'>,
                            pegtl::star<pegtl::sor<quoted_pair, pegtl::not_one<'"'> > >,
                            pegtl::one<'"'>
                        > {};

        struct name : pegtl::seq<
                           pegtl::ascii::alnum,
                           pegtl::rep_max<126, pegtl::sor<pegtl::ascii::alnum, restricted> >
                       > {};

        struct nonquoted : name {}; // nonqouted is used in parameter value to allow action template specialization

        // parameters
        struct attribute : name {};
        struct value : pegtl::sor<quoted, nonquoted>  {};

        // OPT convert to if_must<> to force correct attribute=value pair required
        struct parameter : pegtl::seq<attribute, pegtl::one<'='>, value> {}; 
        struct parameters : pegtl::list<parameter, RWS> {};

        // type
        struct type : name {};

        // subtype 
        struct subtype_nonprefixed : name {};
        struct subtype_prefix : pegtl::plus<name, pegtl::one<'+'> > {};
        struct subtype_suffix : name {};


        struct subtype : pegtl::sor<
                             pegtl::seq<subtype_prefix, subtype_suffix>,
                             subtype_nonprefixed
                         > {};

        // separator
        struct slash : pegtl::one<'/'> {};

        // mime - main grammar to parsing
        struct grammar : pegtl::seq<
                             pegtl::must<
                                 type,
                                 slash,
                                 subtype
                             >,
                             pegtl::opt<
                                 OWS,
                                 pegtl::one<';'>,
                                 OWS,
                                 parameters
                             >
                         > {};


        // mime - grammar to check text (not intendet to use in other grammars)
        struct match_grammar : pegtl::seq<OWS, grammar, OWS, pegtl::ascii::eolf>{};

            // clang-format on

            // state
            struct state {
                using parameters_type = std::vector<std::pair<std::string, std::string> >;

                std::string type;
                std::string subtype;
                std::string suffix;
                parameters_type parameters;
            };

            bool operator==(const state& lhs, const state& rhs);
            bool operator!=(const state& lhs, const state& rhs);
            bool operator==(const state& lhs, const std::string& rhs);
            bool operator!=(const state& lhs, const std::string& rhs);
            bool operator==(const std::string& lhs, const state& rhs);
            bool operator!=(const std::string& lhs, const state& rhs);

            struct param_state {
                template <typename Input>
                explicit param_state(const Input& /*unused*/, state&)
                {
                }

                // internal
                std::string attribute;
                std::string value;

                template <typename Input>
                void success(const Input& /*unused*/, state& c)
                {
                    c.parameters.emplace_back(std::make_pair(std::move(attribute), std::move(value)));
                }
            };

            // actions
            template <typename Rule>
            struct action : pegtl::nothing<Rule> {
            };

            template <>
            struct action<type> {
                template <typename Input>
                static void apply(const Input& in, state& s)
                {
                    s.type = in.string();
                }
            };

            template <>
            struct action<subtype_nonprefixed> {
                template <typename Input>
                static void apply(const Input& in, state& s)
                {
                    s.subtype = in.string();
                }
            };

            template <>
            struct action<subtype_prefix> {
                template <typename Input>
                static void apply(const Input& in, state& s)
                {
                    s.subtype = std::string(in.begin(), in.end() - 1);
                }
            };

            template <>
            struct action<subtype_suffix> {
                template <typename Input>
                static void apply(const Input& in, state& s)
                {
                    s.suffix = in.string();
                }
            };

            template <>
            struct action<parameter> : pegtl::change_state<param_state> {
            }; // converting between state and param_state is handled by param_state c-tor and success() member

            template <>
            struct action<attribute> {
                template <typename Input>
                static void apply(const Input& in, param_state& s)
                {
                    s.attribute = in.string();
                }
            };

            template <>
            struct action<quoted> {
                template <typename Input>
                static void apply(const Input& in, param_state& s)
                {
                    s.value = std::string(in.begin() + 1, in.end() - 1);
                }
            };

            template <>
            struct action<nonquoted> {
                template <typename Input>
                static void apply(const Input& in, param_state& s)
                {
                    s.value = in.string();
                }
            };

            // FIXME: customize error messages
#if 0

        template< typename Rule >
            struct errors
            : public tao::TAO_PEGTL_NAMESPACE::normal< Rule >
            {
                static const std::string error_message;

                template< typename Input, typename... States >
                    static void raise( const Input& in, States&&... /*unused*/ )
                    {
                        throw tao::TAO_PEGTL_NAMESPACE::parse_error( error_message, in );
                    }
            };

        template<> const std::string parser::mediatype::errors< type >::error_message = "invalid mediatype - expected alpanumeric character, reserved character, or separator '/'";
        template<> const std::string parser::mediatype::errors< slash >::error_message = "invalid mediatype - expected alpanumeric character, reserved character, or separator '/'";
        template<> const std::string parser::mediatype::errors< subtype >::error_message = "invalid mediatype - expected alpanumeric character, reserved character, or separator '/'";

#endif

        }; // namespace mediatype
    };     // namespace parsers
};         // namespace apib

#endif // PARSER_MEDIATYPE_H
