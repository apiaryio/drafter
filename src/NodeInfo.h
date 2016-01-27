//
//  NodeInfo.h
//  drafter
//
//  Created by Jiri Kratochvil on 20-10-2005.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_NODEINFO_H
#define DRAFTER_NODEINFO_H

#include "BlueprintSourcemap.h"
#include <algorithm>

#define NODE_INFO(from, member) from.node->member, from.sourceMap->member
#define MAKE_NODE_INFO(from, member) MakeNodeInfo(NODE_INFO(from, member), from.hasSourceMap())

namespace drafter {

    /**
     * For returned values,
     * used eg. for Assets. There is duality directly "blueprint value"
     * vs value generated via JSON Rendering (aka. boutique)
     *
     * For boutique is used (<generated JSON>, NullSourceMap)
     *
     * There is NodeInfo<> constructor accepting directly NodeInfoByValue<> for simplified conversion
     */
    template <typename T>
    struct NodeInfoByValue : public std::pair<T, const snowcrash::SourceMap<T>* > {
        typedef std::pair<T, const snowcrash::SourceMap<T>* > BaseType;

        NodeInfoByValue(const BaseType& info) {
            BaseType::first = info.first;
            BaseType::second = info.second;
        }
    };

    template<typename T>
    struct NodeInfo {
        typedef T NodeType;
        typedef NodeInfo<T> Type;
        typedef snowcrash::SourceMap<T> SourceMapType;

        const NodeType* node;
        const SourceMapType* sourceMap;
        bool empty;

        NodeInfo(const NodeType* node, const SourceMapType* sourceMap) : node(node), sourceMap(sourceMap), empty(false) {}
        NodeInfo(const NodeInfoByValue<T>& node) : node(&node.first), sourceMap(node.second ? node.second : NodeInfo<T>::NullSourceMap()), empty(false) {}
        NodeInfo() : node(Type::NullNode()), sourceMap(Type::NullSourceMap()), empty(true) {}

        NodeInfo<T>& operator=(const NodeInfo<T>& other) {
            node = other.node;
            sourceMap = other.sourceMap;
            empty = other.empty;
            return *this;
        }

        static const NodeType* NullNode() {
            static NodeType nullNode;
            return &nullNode;
        }

        static const SourceMapType* NullSourceMap() {
            static SourceMapType nullSourceMap;
            return &nullSourceMap;
        }

        bool isNull() const { 
            return empty; 
        }

        bool hasSourceMap() const {
            const SourceMapType* null = NullSourceMap();
            return sourceMap != null;
        }
    };

    template <typename T>
    NodeInfo<T> MakeNodeInfo(const T& node, const snowcrash::SourceMap<T>& sourceMap, const bool hasSourceMap)
    {
        return NodeInfo<T>(&node, hasSourceMap ? &sourceMap : NodeInfo<T>::NullSourceMap());
    }

    template <typename T>
    NodeInfo<T> MakeNodeInfo(const T* node, const snowcrash::SourceMap<T>* sourceMap, const bool hasSourceMap)
    {
        return NodeInfo<T>(node, hasSourceMap ? sourceMap : NodeInfo<T>::NullSourceMap());
    }

    template <typename T>
    NodeInfo<T> MakeNodeInfoWithoutSourceMap(const T& node)
    {
        return NodeInfo<T>(&node, NodeInfo<T>::NullSourceMap());
    }

    template<typename ResultType, typename Collection1, typename Collection2, typename BinOp>
    ResultType Zip(const Collection1& collection1, const Collection2& collection2, const BinOp& Combinator) {
        ResultType result;
        std::transform(collection1.begin(), collection1.end(), collection2.begin(), std::back_inserter(result), Combinator);
        return result;
    }

    template<typename T>
    struct NodeInfoCollection  : public std::vector<NodeInfo<typename T::value_type> > {
        typedef NodeInfoCollection<T> SelfType;
        typedef std::vector<NodeInfo<typename T::value_type> > CollectionType;

        template <typename U>
        static NodeInfo<U> MakeNodeInfo(const U& node, const snowcrash::SourceMap<U>& sourceMap)
        {
            return NodeInfo<U>(&node, &sourceMap);
        }

        NodeInfoCollection(const NodeInfo<T>& nodeInfo)
        {
            const T& collection = *nodeInfo.node;
            const snowcrash::SourceMap<T>& sourceMaps = *nodeInfo.sourceMap;

            if (collection.size() == sourceMaps.collection.size()) {
                CollectionType nodes = Zip<CollectionType>(collection, sourceMaps.collection, NodeInfoCollection::MakeNodeInfo<typename T::value_type>);
                std::copy(nodes.begin(), nodes.end(), std::back_inserter(*this));
            }
            else {
                std::transform(collection.begin(), collection.end(), 
                               std::back_inserter(*this), 
                               MakeNodeInfoWithoutSourceMap<typename T::value_type>);
            }
        }

        NodeInfoCollection(const T& collection, const snowcrash::SourceMap<T>& sourceMaps)
        {

            if (collection.size() == sourceMaps.collection.size()) {
                CollectionType nodes = Zip<CollectionType>(collection, sourceMaps.collection, NodeInfoCollection::MakeNodeInfo<typename T::value_type>);
                std::copy(nodes.begin(), nodes.end(), std::back_inserter(*this));
            }
            else {
                std::transform(collection.begin(), collection.end(), 
                               std::back_inserter(*this), 
                               MakeNodeInfoWithoutSourceMap<typename T::value_type>);
            }
        }
    };
}

#endif // #ifndef DRAFTER_NODEINFO_H
