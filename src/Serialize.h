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

/** Version of API Blueprint serialization */
#define AST_SERIALIZATION_VERSION "3.0"
#define PARSE_RESULT_SERIALIZATION_VERSION "2.1"

namespace drafter {

    /**
     *  AST entities serialization keys
     */
    struct SerializeKey {
        static const std::string Metadata;
        static const std::string Reference;
        static const std::string Id;
        static const std::string Name;
        static const std::string Description;
        static const std::string ResourceGroups;
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
        static const std::string SourceMap;
        static const std::string Error;
        static const std::string Warnings;
        static const std::string AnnotationCode;
        static const std::string AnnotationMessage;
        static const std::string AnnotationLocation;
        static const std::string AnnotationLocationIndex;
        static const std::string AnnotationLocationLength;
    };


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

    };

}

#endif
