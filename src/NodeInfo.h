//
//  NodeInfo.h
//  drafter
//
//  Created by Jiri Kratochvi on 20-10-2005.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_NODEINFO_H
#define DRAFTER_NODEINFO_H


#include "BlueprintSourcemap.h"

#define MAKE_SECTION_INFO(from, member) MakeSectionInfo(from.section.member, from.sourceMap.member, from.hasSourceMap())

namespace drafter {

    template<typename T>
    struct SectionInfo {
        typedef T SectionType;
        typedef SectionInfo<T> Type;
        typedef snowcrash::SourceMap<T> SourceMapType;

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
}

#endif // #ifndef DRAFTER_NODEINFO_H
