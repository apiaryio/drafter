//
//  Serialize.h
//  drafter
//
//  Created by Zdenek Nemec on 5/3/13.
//  Copyright (c) 2013 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_SERIALIZE_H
#define DRAFTER_SERIALIZE_H

#include <string>
#include "BlueprintSourcemap.h"
#include "sos.h"

#include "NodeInfo.h"

#include "refract/Element.h"
#include "refract/Registry.h"

/** Version of API Blueprint serialization */
#define AST_SERIALIZATION_VERSION "4.0"
#define PARSE_RESULT_SERIALIZATION_VERSION "2.2"

// Forward declaration
namespace snowcrash {

    template <typename T>
    struct ParseResult;
}

namespace drafter {

    enum ASTType {
        NormalASTType = 0,   // Normal AST
        RefractASTType,      // Refract AST
        UnknownASTType = -1
    };

    enum SerializeFormat {
        JSONFormat = 0,     // JSON Format
        YAMLFormat,         // YAML Format
        UnknownFormat = -1
    };

    // Options struct for drafter
    struct WrapperOptions {
        const ASTType astType;
        const bool generateSourceMap;
        const bool expandMSON;

        WrapperOptions(const ASTType astType, const bool generateSourceMap, const bool expandMSON)
        : astType(astType), generateSourceMap(generateSourceMap), expandMSON(expandMSON) {}

        WrapperOptions(const ASTType astType, const bool generateSourceMap)
        : astType(astType), generateSourceMap(generateSourceMap), expandMSON(false) {}

        WrapperOptions(const ASTType astType)
        : astType(astType), generateSourceMap(false), expandMSON(false) {}
    };

    /**
     *  AST and Refract entities serialization keys
     */
    struct SerializeKey {
        static const std::string Metadata;
        static const std::string Reference;
        static const std::string Id;
        static const std::string Name;
        static const std::string Description;
        static const std::string DataStructure;
        static const std::string DataStructures;
        static const std::string ResourceGroup;
        static const std::string ResourceGroups;
        static const std::string Resource;
        static const std::string Resources;
        static const std::string URI;
        static const std::string URITemplate;
        static const std::string Assets;
        static const std::string Actions;
        static const std::string Action;
        static const std::string Relation;
        static const std::string Attributes;
        static const std::string Examples;
        static const std::string Transaction;
        static const std::string Method;
        static const std::string Requests;
        static const std::string Responses;
        static const std::string Body;
        static const std::string Schema;
        static const std::string Headers;
        static const std::string Model;
        static const std::string Value;
        static const std::string Parameters;
        static const std::string Type;
        static const std::string Required;
        static const std::string Default;
        static const std::string Nullable;
        static const std::string Example;
        static const std::string Values;

        static const std::string Source;
        static const std::string Resolved;

        static const std::string Literal;
        static const std::string Variable;
        static const std::string TypeDefinition;
        static const std::string TypeSpecification;
        static const std::string NestedTypes;
        static const std::string Sections;
        static const std::string Class;
        static const std::string Content;
        static const std::string ValueDefinition;

        static const std::string Element;
        static const std::string Role;

        static const std::string Version;
        static const std::string Ast;
        static const std::string Sourcemap;
        static const std::string Error;
        static const std::string Warning;
        static const std::string Warnings;
        static const std::string AnnotationCode;
        static const std::string AnnotationMessage;
        static const std::string AnnotationLocation;
        static const std::string AnnotationLocationIndex;
        static const std::string AnnotationLocationLength;

        // Refract meta
        static const std::string Meta;
        static const std::string Title;
        static const std::string Classes;

        // Refract MSON attributes
        static const std::string Samples;
        static const std::string TypeAttributes;

        // Refract MSON attribute "typeAttibute" values
        static const std::string Optional;
        static const std::string Fixed;
        static const std::string FixedType;

        // Literal to Bool
        static const std::string True;

        // Refract MSON generic element
        static const std::string Generic;

        // Refract (nontyped) element names
        static const std::string Enum;
        static const std::string Ref;

        // Refract Ref Element - keys/values
        static const std::string Href;
        static const std::string Path;

        // API Namespace
        static const std::string Category;
        static const std::string Copy;
        static const std::string API;
        static const std::string User;
        static const std::string Transition;
        static const std::string HrefVariables;
        static const std::string HTTPHeaders;
        static const std::string HTTPTransaction;
        static const std::string ContentType;
        static const std::string HTTPResponse;
        static const std::string HTTPRequest;
        static const std::string StatusCode;
        static const std::string Asset;
        static const std::string MessageBody;
        static const std::string MessageBodySchema;
        static const std::string Data;

        // Parse Result Namespace
        static const std::string ParseResult;
        static const std::string Annotation;
        static const std::string SourceMap;
    };

    template<typename T> std::pair<bool, T> LiteralTo(const mson::Literal&);

    template <> std::pair<bool, bool> LiteralTo<bool>(const mson::Literal& literal);
    template <> std::pair<bool, double> LiteralTo<double>(const mson::Literal& literal);
    template <> std::pair<bool, std::string> LiteralTo<std::string>(const mson::Literal& literal);


    refract::ArrayElement* CreateArrayElement(refract::IElement* value, bool rFull = false);

    template <typename T>
    refract::ArrayElement* CreateArrayElement(const T& content, bool rFull = false)
    {
        refract::IElement* value = refract::IElement::Create(content);
        return CreateArrayElement(value);
    }

    /**
     * \brief functor pattern to translate _collection_ into sos::Array on serialization
     * \requests for collection - must define typedef member ::const_iterator
     *
     * usage:
     *
     * sos::Array elements = WrapCollection<mson::Element>()(getSomeListOfElements(), WrapMSONElement));
     *
     * operator()(const T& collection, Functor &wrapper)
     * \param collection - it come typicaly from snowcrash
     * \param wrapper - adaptee element before push to collection
     *        you have to write your own, for example \see SeriallizeAST.cc
     *
     */
    template<typename T, typename R = sos::Array>
    struct WrapCollection {

        typedef T value_type;

        template<typename Collection, typename Functor>
        R operator()(const Collection& collection, Functor &wrapper) const {
            typedef typename Collection::const_iterator iterator_type;
            R array;

            for (iterator_type it = collection.begin(); it != collection.end(); ++it) {
                array.push(wrapper(*it));
            }

            return array;
        }

        template<typename Collection, typename Functor, typename Arg1>
        R operator()(const Collection& collection, Functor &wrapper, Arg1 arg1) const {
            typedef typename Collection::const_iterator iterator_type;
            R array;

            for (iterator_type it = collection.begin(); it != collection.end(); ++it) {
                array.push(wrapper(*it, arg1));
            }

            return array;
        }

        template<typename Collection, typename Functor, typename Arg1, typename Arg2>
        R operator()(const Collection& collection, Functor &wrapper, Arg1 arg1, Arg2 arg2) const {
            typedef typename Collection::const_iterator iterator_type;
            R array;

            for (iterator_type it = collection.begin(); it != collection.end(); ++it) {
                array.push(wrapper(*it, arg1, arg2));
            }

            return array;
        }
    };

    template<typename T, typename R = sos::Array>
    struct WrapCollectionIf {
        // When we want to use predicate, let's use a dummy argument to distinguish it
        template<typename Collection, typename Functor, typename Predicate>
        R operator()(const Collection& collection, Functor &wrapper, Predicate &predicate) const {
            typedef typename Collection::const_iterator iterator_type;
            R array;

            for (iterator_type it = collection.begin(); it != collection.end(); ++it) {
                if (predicate(*it)) {
                    array.push(wrapper(*it));
                }
            }

            return array;
        }

        template<typename Collection, typename Functor, typename Predicate, typename Arg1>
        R operator()(const Collection& collection, Functor &wrapper, Predicate &predicate, Arg1& arg1) const {
            typedef typename Collection::const_iterator iterator_type;
            R array;

            for (iterator_type it = collection.begin(); it != collection.end(); ++it) {
                if (predicate(*it)) {
                    array.push(wrapper(*it, arg1));
                }
            }

            return array;
        }

    };

}

#endif
