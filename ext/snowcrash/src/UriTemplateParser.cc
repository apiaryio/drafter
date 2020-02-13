//
//  UriTemplateParser.cc
//  snowcrash
//
//  Created by Carl Griffiths on 24/02/14.
//  Copyright (c) 2014 Apiary Inc. All rights reserved.
//
#include <iomanip>
#include "UriTemplateParser.h"
#include "RegexMatch.h"

#define URI_REGEX "^(http|https|ftp|file)?(://)?([^/]*)?(.*)$"

#include "../../../src/parser/Uritemplate.h"
using namespace snowcrash;

using namespace apib::parser::uritemplate;

namespace error_locator {

    struct state {
        std::string message;
        size_t position = 0;
        size_t length = 0;
    };

    template< typename Rule >
    struct control
       : tao::pegtl::normal< Rule > 
    {};

    template<>
    struct control<expression_close>
       : tao::pegtl::normal<expression_close>
    {
        template< typename Input, typename... States >
        static void raise( const Input& in, States&&... )
        {
           throw error_locator::state{ "URI Template expression is missing closing bracket '}'", in.position().byte};
        }

    };

    /* FIXME: shoud we send warning for partial pct-triplets like `abc%2`
     * we can report it as invalid char `%` 
     * or as invalit pct-triplet `%2`
     */
    struct pct_triplet : pegtl::seq<
                                   pegtl::one<'%'>,
                                   pegtl::any,
                                   pegtl::any
                                 > {};

    struct continuous_dots : pegtl::seq<
                               pegtl::one<'.'>,
                               pegtl::plus<
                                 pegtl::one<'.'>
                               >
                             > {};

    struct invalid_char : pegtl::utf8::any {};


    template <typename R>
    struct grammar : pegtl::seq<
                                pegtl::sor<R, pegtl::seq<>>,
                                  pegtl::sor<
                                    pct_triplet,
                                    continuous_dots,
                                    invalid_char
                                 >
                            > {};

    template <typename Rule>
    struct report_action : pegtl::nothing<Rule> {
    };

    template <>
    struct report_action<invalid_char> {

        template<typename Input>
        static void apply(const Input& in, state& s)
        {
            s.position = in.position().byte;
            s.length = in.string().length();

            if (in.string() == "{") { // special case
                s.message = std::string{"nested expression bracket '{'"};
            } else {
                s.message = std::string{"character '"} + in.string() + std::string{"'"};
            }

        }
    };

    template <>
    struct report_action<continuous_dots> {

        template<typename Input>
        static void apply(const Input& in, state& s)
        {
            s.message = std::string{"continuous dots '"} + in.string() + std::string{"'"};
            s.position = in.position().byte;
            s.length = in.string().length();
        }
    };

    template <>
    struct report_action<pct_triplet> {

        template<typename Input>
        static void apply(const Input& in, state& s)
        {
            s.message = std::string{"percent encoded character '"} + in.string() + std::string{"'"};
            s.position = in.position().byte;
            s.length = in.string().length();
        }
    };

}

struct VariableValidityChecker {
    ParsedURITemplate& result;
    const mdp::CharactersRangeSet& sourceBlock;

    void operator()(const state::variable& s) const {
        // do nothing - variable is parsed correctly, so there is nothing to report
    }

    void operator()(const state::invalid& s) const {

        tao::pegtl::memory_input<> in(s.content, "");
        error_locator::state state;
        tao::pegtl::parse<error_locator::grammar<varspec>, error_locator::report_action>(in, state);

        std::stringstream ss;
        ss << "URI template variable '" << s.content
           << "' contains invalid " << state.message
           <<". Allowed characters for expressions are A-Z a-z 0-9 _ and percent encoded characters";

#if 0
        /**
         *  this is prepared to set exact location of invalid character
         *  but in current version we receive souce map for full makdown node instead of position
         *  URI template so we are not able to set exact location of invalid character
         */
        assert(sourceBlock.size() == 1);
        mdp::CharactersRangeSet err;
        err.emplace_back(mdp::CharactersRange{sourceBlock.front().location + s.position + state.position, s.content.length()});

        result.report.warnings.push_back(Warning(ss.str(), URIWarning, err));
#endif
        result.report.warnings.push_back(Warning(ss.str(), URIWarning, sourceBlock));
    }
};

struct TemplateValidityChecker {
    ParsedURITemplate& result;
    const mdp::CharactersRangeSet& sourceBlock;

    void operator()(const state::literals& s) const {
        // do nothing literal is parsed correctly, so there is nothing to report
    }

    void operator()(const state::invalid& s) const {

        tao::pegtl::memory_input<> in(s.content, "");
        error_locator::state state;
        tao::pegtl::parse<error_locator::grammar<literals_>, error_locator::report_action>(in, state);

        std::stringstream ss;
        ss << "URI template contains invalid " << state.message
           << ". You probably will need to repace invalid chars by percent encoded character.";

#if 0
        /**
         *  this is prepared to set exact location of invalid character
         *  but in current version we receive souce map for full makdown node instead of position
         *  URI template so we are not able to set exact location of invalid character
         */
        assert(sourceBlock.size() == 1);
        mdp::CharactersRangeSet err;
        err.emplace_back(mdp::CharactersRange{sourceBlock.front().location + s.position + state.position, s.content.length()});

        result.report.warnings.push_back(Warning(ss.str(), URIWarning, err));
#endif
        result.report.warnings.push_back(Warning(ss.str(), URIWarning, sourceBlock));
    }

    void operator()(const state::expression& s) const {
        if (!isSupportedOperator(s.type)) {
            std::stringstream ss;
            ss <<  "URI template '" <<  operatorToText(s.type) << "' expansion is not supported";
            result.report.warnings.push_back(
                Warning(ss.str(), URIWarning, sourceBlock));
        }

        for (const auto& variable: s.variables) {
            mpark::visit(VariableValidityChecker{ result, sourceBlock }, variable);
        }
    }

    void operator()(const mpark::monostate& s) const {
        assert(0);
    }

    static bool isSupportedOperator(const state::expression_type type) {
        return type == state::expression_type::noop ||
            type == state::expression_type::reserved_chars ||
            type == state::expression_type::fragment ||
            type == state::expression_type::query_param ||
            type == state::expression_type::query_continue;
    }

    static std::string operatorToText(const state::expression_type type) {
        // identify unsupported is good enough,
        // it is intended just for reports
        switch (type) {
            case state::expression_type::label:
                return "label";
            case state::expression_type::path:
                return "path segment";
            case state::expression_type::path_param:
                return "path segment parameter";
            default:
                return "reserved operator";
        }
        assert(0);
        return std::string{};
    }
};

void URITemplateParser::parse(
    const URITemplate& uri, const mdp::CharactersRangeSet& sourceBlock, ParsedURITemplate& result)
{
    using namespace apib::parser::uritemplate;

    CaptureGroups groups;
    const size_t gSize = 5;

    if (uri.empty())
        return;

    if (!RegexCapture(uri, URI_REGEX, groups, gSize)) {
        result.report.error = Error("Failed to parse URI Template", ApplicationError);
        return;
    }

    result.scheme = groups[1];
    result.host = groups[3];
    result.path = groups[4];

    state::uritemplate state;

    //tao::pegtl::memory_input<> in(result.path, "");
    tao::pegtl::memory_input<> in(uri, "");

    try {
        tao::pegtl::parse<match_grammar, action, error_locator::control>(in, state);
    } catch (const pegtl::parse_error& e) {
        result.report.warnings.push_back(Warning(e.what(), URIWarning, sourceBlock));
    } catch (const error_locator::state& e) {
        result.report.warnings.push_back(Warning(e.message, URIWarning, sourceBlock));
    }

    for (const auto& part: state) {
        mpark::visit(TemplateValidityChecker{ result, sourceBlock }, part);
    }
}
