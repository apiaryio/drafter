//
//  parser/Mediatype.h
//  librefract
//
//  Created by Jiri Kratochvil on 02/08/2019
//  Copyright (c) 2019 Apiary Inc. All rights reserved.
//

#ifndef PARSER_MEDIATYPE_H
#define PARSER_MEDIATYPE_H

#include "../PEGTL/include/tao/pegtl.hpp"

#include <string>
#include <map>

#include <iostream>

namespace parser 
{

    namespace mediatype {

        namespace pegtl = tao::pegtl;

        struct restricted : pegtl::one< '!', '#', '$', '&', '^',  '_', '-', '.' >  {};

        using RWS = pegtl::plus<pegtl::blank>;
        using OWS = pegtl::star<pegtl::blank>;

        // OPT: pegtl::if_must<> - if we want to throw on non-closed quotes
        struct quoted : pegtl::seq<
                            pegtl::one<'"'>,
                            pegtl::plus<pegtl::not_at<pegtl::one<'"'>>, pegtl::any>,
                            pegtl::one<'"'>
                        > {};
        struct ident : pegtl::seq<
                           pegtl::ascii::alnum,
                           pegtl::star<pegtl::sor<pegtl::ascii::alnum, restricted>>
                       > {};

        // parameters
        struct key : ident {};
        struct nonquoted : ident {};
        struct value : pegtl::sor<quoted, nonquoted>  {};

        // OPT convert to if_must<> to force correct key=value pair required
        struct parameter : pegtl::seq<key, pegtl::one<'='>, value> {}; 
        struct parameters : pegtl::list<parameter, RWS> {};

        // type
        struct type : ident {};

        // subtype 
        struct subtype_nonprefixed : ident {};
        struct subtype_prefix : pegtl::plus<ident, pegtl::one<'+'>> {};
        struct subtype_suffix : ident {};


        struct subtype : pegtl::sor<
                             pegtl::seq<subtype_prefix, subtype_suffix>,
                             subtype_nonprefixed
                         > {};

        // separator
        struct slash : pegtl::one<'/'> {};
        // special case - if we do not want to throw on bad type definition like 'type/sub/type'
        struct bad_slash : pegtl::not_at<slash> {};

        // mime
        struct grammar : pegtl::seq<
                             pegtl::must<
                                 type,
                                 slash,
                                 subtype
                             >,
                             pegtl::must<bad_slash>,
                             pegtl::opt<
                                 OWS,
                                 pegtl::one<';'>,
                                 OWS,
                                  parameters
                             >
                         > {};

        // state
        struct state {
            std::string type;
            std::string subtype;
            std::string suffix;
            std::map<std::string, std::string> parameters;
        };

        bool operator==(const state& lhs, const state& rhs) {
            return lhs.type == rhs.type
                && lhs.subtype == rhs.subtype
                && lhs.suffix == rhs.suffix
                && lhs.parameters.size() == rhs.parameters.size()
                && std::equal(lhs.parameters.cbegin(), lhs.parameters.cend(),
                        rhs.parameters.cbegin());
            ;
        }

        bool operator!=(const state& lhs, const state& rhs) {
            return !(lhs == rhs);
        }

        bool operator==(const state& lhs, const std::string& rhs) {
            std::string s;

            s.reserve(lhs.type.length() + lhs.subtype.length() + lhs.suffix.length() + 2);
            s.append(lhs.type);
            s.append("/");
            s.append(lhs.subtype);
            if (!lhs.suffix.empty()) {
                s.append("+");
                s.append(lhs.suffix);
            }

            return s == rhs;
        }

        bool operator!=(const state& lhs, const std::string& rhs) {
            return !(lhs == rhs);
        }

        bool operator==(const std::string& lhs, const state& rhs) {
            return operator==(rhs, lhs);
        }

        bool operator!=(const std::string& lhs, const state& rhs) {
            return !operator==(rhs, lhs);
        }

        struct param_state {
            template< typename Input >
                explicit param_state( const Input& /*unused*/, state&) {}

            // internal
            std::string key;
            std::string value;

            template <typename Input>
                void success(const Input& /*unused*/, state& c) {
                    c.parameters.emplace(std::make_pair(std::move(key), std::move(value)));
                }

        };

        // actions

        template< std::string state::*Field >
            struct bind_mediatype
            {

                template< typename Input >
                    static void apply( const Input& in, state& s)
                    {
                        s.*Field = in.string();
                    }
            };

        template <typename Rule>
            struct action : pegtl::nothing< Rule > {};

        template <>
            struct action <type> {
                template< typename Input >
                    static void apply( const Input& in, state& s )
                    {
                        s.type = in.string();
                    }
            };

        template <>
            struct action <subtype_nonprefixed> {
                template< typename Input >
                    static void apply( const Input& in, state& s )
                    {
                        s.subtype = in.string();
                    }
            };

        template <>
            struct action <subtype_prefix> {
                template< typename Input >
                    static void apply( const Input& in, state& s )
                    {
                        s.subtype = std::string(in.begin(), in.end()-1) ;
                    }
            };

        template <>
            struct action <subtype_suffix> {
                template< typename Input >
                    static void apply( const Input& in, state& s )
                    {
                        s.suffix = in.string();
                    }
            };

        template <>
            struct action <parameter> : pegtl::change_state<param_state> {}; // converting between state and param_state is handled by param_state c-tor and success() member

        template <>
            struct action <key> {
                template< typename Input >
                    static void apply( const Input& in, param_state& s )
                    {
                        s.key = in.string();
                    }
            };

        template <>
            struct action <quoted> {
                template< typename Input >
                    static void apply( const Input& in, param_state& s )
                    {
                        s.value = std::string(in.begin()+1, in.end()-1) ;
                    }
            };

        template <>
            struct action <nonquoted> {
                template< typename Input >
                    static void apply( const Input& in, param_state& s )
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
}; // namespace parsers

#endif // PARSER_MEDIATYPE_H

