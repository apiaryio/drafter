//
//  ParametersParser.h
//  snowcrash
//
//  Created by Zdenek Nemec on 9/1/13.
//  Copyright (c) 2013 Apiary Inc. All rights reserved.
//

#ifndef SNOWCRASH_PARAMETERSPARSER_H
#define SNOWCRASH_PARAMETERSPARSER_H

#include "SectionParser.h"
#include "ParameterParser.h"
#include "MSONParameterParser.h"
#include "RegexMatch.h"
#include "StringUtility.h"
#include "BlueprintUtility.h"

#include "../../../src/parser/Uritemplate.h"

namespace snowcrash
{

    /** Parameters matching regex */
    const char* const ParametersRegex = "^[[:blank:]]*[Pp]arameters?[[:blank:]]*$";

    /** No parameters specified message */
    const char* const NoParametersMessage
        = "no parameters specified, expected a nested list of parameters, one parameter per list item";

    /** Internal type alias for Collection iterator of Parameter */
    typedef Collection<Parameter>::iterator ParameterIterator;

    /**
     * Parameters section processor
     */
    template <>
    struct SectionProcessor<Parameters> : public SectionProcessorBase<Parameters> {

        static MarkdownNodeIterator processSignature(const MarkdownNodeIterator& node,
            const MarkdownNodes& siblings,
            SectionParserData& pd,
            SectionLayout& layout,
            const ParseResultRef<Parameters>& out)
        {

            mdp::ByteBuffer remainingContent;

            GetFirstLine(node->text, remainingContent);

            if (!remainingContent.empty()) {

                // WARN: Extra content in parameters section
                std::stringstream ss;
                ss << "ignoring additional content after 'parameters' keyword,";
                ss << " expected a nested list of parameters, one parameter per list item";

                mdp::CharactersRangeSet sourceMap
                    = mdp::BytesRangeSetToCharactersRangeSet(node->sourceMap, pd.sourceCharacterIndex);
                out.report.warnings.push_back(Warning(ss.str(), IgnoringWarning, sourceMap));
            }

            return ++MarkdownNodeIterator(node);
        }

        NO_SECTION_DESCRIPTION(Parameters)

        static MarkdownNodeIterator processNestedSection(const MarkdownNodeIterator& node,
            const MarkdownNodes& siblings,
            SectionParserData& pd,
            const ParseResultRef<Parameters>& out)
        {

            IntermediateParseResult<Parameter> parameter(out.report);

            if (pd.sectionContext() == ParameterSectionType) {
                ParameterParser::parse(node, siblings, pd, parameter);
            } else if (pd.sectionContext() == MSONParameterSectionType) {
                IntermediateParseResult<MSONParameter> msonParameter(out.report);
                MSONParameterParser::parse(node, siblings, pd, msonParameter);

                // Copy values from MSON Parameter to normal parameter
                parameter.report = msonParameter.report;
                parameter.node = msonParameter.node;
                parameter.sourceMap = msonParameter.sourceMap;
            } else {
                return node;
            }

            if (!out.node.empty()) {

                ParameterIterator duplicate = findParameter(out.node, parameter.node);

                if (duplicate != out.node.end()) {
                    removeParameter(duplicate, pd, out);

                    // WARN: Parameter already defined
                    std::stringstream ss;
                    ss << "overshadowing previous parameter '" << parameter.node.name << "' definition";

                    mdp::CharactersRangeSet sourceMap
                        = mdp::BytesRangeSetToCharactersRangeSet(node->sourceMap, pd.sourceCharacterIndex);
                    out.report.warnings.push_back(Warning(ss.str(), RedefinitionWarning, sourceMap));
                }
            }

            out.node.push_back(parameter.node);

            if (pd.exportSourceMap()) {
                out.sourceMap.collection.push_back(parameter.sourceMap);
            }

            return ++MarkdownNodeIterator(node);
        }

        static SectionType sectionType(const MarkdownNodeIterator& node)
        {

            if (node->type == mdp::ListItemMarkdownNodeType && !node->children().empty()) {

                mdp::ByteBuffer remaining, subject = node->children().front().text;

                subject = GetFirstLine(subject, remaining);
                TrimString(subject);

                if (RegexMatch(subject, ParametersRegex)) {
                    return ParametersSectionType;
                }
            }

            return UndefinedSectionType;
        }

        static SectionType nestedSectionType(const MarkdownNodeIterator& node)
        {

            return SectionProcessor<Parameter>::sectionType(node);
        }

        static void finalize(
            const MarkdownNodeIterator& node, SectionParserData& pd, const ParseResultRef<Parameters>& out)
        {

            if (out.node.empty()) {

                // WARN: No parameters defined
                mdp::CharactersRangeSet sourceMap
                    = mdp::BytesRangeSetToCharactersRangeSet(node->sourceMap, pd.sourceCharacterIndex);
                out.report.warnings.push_back(Warning(NoParametersMessage, FormattingWarning, sourceMap));
            }
        }

        /** Finds a parameter inside a parameters collection */
        static ParameterIterator findParameter(Parameters& parameters, const Parameter& parameter)
        {

            return std::find_if(parameters.begin(), parameters.end(), std::bind2nd(MatchName<Parameter>(), parameter));
        }

        static void removeParameter(
            ParameterIterator& parameter, SectionParserData& pd, const ParseResultRef<Parameters>& out)
        {

            out.node.erase(parameter);

            if (pd.exportSourceMap()) {
                size_t parameterIndex = parameter - out.node.begin();
                out.sourceMap.collection.erase(out.sourceMap.collection.begin() + parameterIndex);
            }
        }
    };
    
    template <typename T>
    static void reportParameterIsNotFound(const Parameter& param,
        const MarkdownNodeIterator& node,
        const SectionParserData& pd,
        const ParseResultRef<T>& out)
    {
        std::stringstream ss;
        ss << "parameter '" << param.name << "' is not found within the URI template '" << out.node.uriTemplate
           << "'";

        if (!out.node.name.empty()) {
            ss << " for '" << out.node.name << "' ";
        }

        mdp::CharactersRangeSet sourceMap
            = mdp::BytesRangeSetToCharactersRangeSet(node->sourceMap, pd.sourceCharacterIndex);
        out.report.warnings.push_back(Warning(ss.str(), LogicalErrorWarning, sourceMap));
    }

    /**
     * \brief Check Parameters eligibility in URI template
     *
     * \warning Do not specialise this.
     */
    template <typename T>
    static void checkParametersEligibility(const MarkdownNodeIterator& node,
        const SectionParserData& pd,
        const Parameters& parameters,
        const ParseResultRef<T>& out)
    {

        using namespace apib::parser::uritemplate;
        state::uritemplate result;

        tao::pegtl::memory_input<> in(out.node.uriTemplate, "");
        tao::pegtl::parse<match_grammar, action>(in, result);

        // extract names of all variables
        using Vars = std::vector<std::string>;
        Vars variables;

        // extraxt just __valid__ variable names
        for (const auto& part : result) {
            if (const auto expression = mpark::get_if<state::expression>(&part)) {
                for(const auto& var : expression->variables) {
                    if (const auto valid = mpark::get_if<state::variable>(&var)) {
                        variables.push_back(valid->name);
                    } 
                }
            }
        };

        std::for_each(parameters.begin(), parameters.end(),
            [&variables, &out, &node, &pd](const Parameter& param){
                if (variables.end() == std::find_if(variables.begin(), variables.end(),
                    [&param](const std::string& var) {
                        return param.name == var;
                    })) {
                    reportParameterIsNotFound<T>(param, node, pd, out);
                }
            });

    }

    /** Parameters Section parser */
    typedef SectionParser<Parameters, ListSectionAdapter> ParametersParser;
}

#endif
