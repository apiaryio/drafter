//
//  Serialize.h
//  drafter
//
//  Created by Zdenek Nemec on 5/3/13.
//  Copyright (c) 2013 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_SERIALIZE_H
#define DRAFTER_SERIALIZE_H

#define _WITH_REFRACT_ 1

#include <string>
#include "BlueprintSourcemap.h"
#include "sos.h"

#include "refract/Element.h"
#include "refract/Registry.h"
#include "refract/Visitors.h"

/** Version of API Blueprint serialization */
#define AST_SERIALIZATION_VERSION "4.0"
#define PARSE_RESULT_SERIALIZATION_VERSION "2.2"

namespace drafter {

    enum ASTType {
        NormalASTType = 0,   // Normal AST
        RefractASTType,      // Refract AST
        UnknownASTType = -1
    };

    refract::Registry& GetNamedTypesRegistry();

    template<typename T>
    struct SectionInfo {
        typedef SectionInfo<T> Type;
        typedef T SectionType;
        typedef snowcrash::SourceMap<SectionType> SourceMapType;

        const SectionType& section;
        const SourceMapType& sourceMap;
        const bool empty;

        SectionInfo(const SectionType& section, const SourceMapType& sourceMap) : section(section), sourceMap(sourceMap), empty(false) {}
        SectionInfo() : section(Type::NullSection()), sourceMap(Type::NullSourceMap()), empty(true) {}

        /**
         * BE CAREFUL while assign SectionInfo it probably will not work as you expected
         * but we need this to allow store SectionInfo in containers
         *
         * alternative solution is store `section` and `sourceMap` in C++11 smart pointers
         */
        SectionInfo<T>& operator=(const SectionInfo<T>& other) { 
            return *this; 
        }

        static const SectionType& NullSection() {
            static SectionType nullSection;
            return nullSection;
        }

        static const SourceMapType& NullSourceMap() {
            static SourceMapType nullSourceMap;
            return nullSourceMap;
        }

        bool isNull() const { 
            return empty; 
        }

        bool hasSourceMap() const {
            const SourceMapType& null = NullSourceMap();
            return &sourceMap != &null;
        }
    };

    template <typename T>
    SectionInfo<T> MakeSectionInfo(const T& section, const snowcrash::SourceMap<T>& sourceMap, const bool hasSourceMap)
    {
        return SectionInfo<T>(section, hasSourceMap ? sourceMap : SectionInfo<T>::NullSourceMap());
    }


    template <typename T>
    SectionInfo<T> MakeSectionInfoWithoutSourceMap(const T& section)
    {
        return SectionInfo<T>(section, SectionInfo<T>::NullSourceMap());
    }

#define MAKE_SECTION_INFO(from, member) MakeSectionInfo(from.section.member, from.sourceMap.member, from.hasSourceMap())

    template<typename ResultType, typename Collection1, typename Collection2, typename BinOp>
    ResultType Zip(const Collection1& collection1, const Collection2& collection2, const BinOp& Combinator) {
        ResultType result;
        std::transform(collection1.begin(), collection1.end(), collection2.begin(), std::back_inserter(result), Combinator);
        return result;
    }

    template<typename T>
    struct SectionInfoCollection  : public std::vector<SectionInfo<typename T::value_type> > {
        typedef SectionInfoCollection<T> SelfType;
        typedef std::vector<SectionInfo<typename T::value_type> > CollectionType;

        template <typename U>
        static SectionInfo<U> MakeSectionInfo(const U& section, const snowcrash::SourceMap<U>& sourceMap)
        {
            return SectionInfo<U>(section, sourceMap);
        }

        SectionInfoCollection(const T& collection, const snowcrash::SourceMap<T>& sourceMaps)
        {

            if (collection.size() == sourceMaps.collection.size()) {
                CollectionType sections = Zip<CollectionType>(collection, sourceMaps.collection, SectionInfoCollection::MakeSectionInfo<typename T::value_type>);
                std::copy(sections.begin(), sections.end(), std::back_inserter(*this));
            }
            else {
                std::transform(collection.begin(), collection.end(), 
                               std::back_inserter(*this), 
                               MakeSectionInfoWithoutSourceMap<typename T::value_type>);
            }
            
        }

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

        // Literal to Bool
        static const std::string True;

        // Refract MSON generic element
        static const std::string Generic;

        // Refract (nontyped) element names
        static const std::string Enum;
        static const std::string Select;
        static const std::string Option;
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
        static const std::string MessageSchema;
        static const std::string Data;

        // Parse Result Namespace
        static const std::string ParseResult;
        static const std::string Annotation;
        static const std::string SourceMap;
    };

    template<typename T> T LiteralTo(const mson::Literal&);

    template <> bool LiteralTo<bool>(const mson::Literal& literal);
    template <> double LiteralTo<double>(const mson::Literal& literal);
    template <> std::string LiteralTo<std::string>(const mson::Literal& literal);

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

        template<typename Collection, typename Functor, typename Argument>
        R operator()(const Collection& collection, Functor &wrapper, Argument argument) const {
            typedef typename Collection::const_iterator iterator_type;
            R array;

            for (iterator_type it = collection.begin(); it != collection.end(); ++it) {
                array.push(wrapper(*it, argument));
            }

            return array;
        }

        // When we want to use predicate, let's use a dummy argument to distinguish it
        template<typename Collection, typename Functor, typename Predicate>
        R operator()(const Collection& collection, Functor &wrapper, Predicate &predicate, bool dummy) const {
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
