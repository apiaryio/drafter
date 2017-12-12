//
//  MarkdownNode.h
//  markdownparser
//
//  Created by Zdenek Nemec on 4/16/14.
//  Copyright (c) 2014 Apiary. All rights reserved.
//

#ifndef MARKDOWNPARSER_NODE_H
#define MARKDOWNPARSER_NODE_H

#include <vector>
#include <memory>
#include <iostream>
#include "ByteBuffer.h"

namespace mdp
{

    /**
     *  AST block node types
     */
    enum MarkdownNodeType
    {
        RootMarkdownNodeType = 0,
        CodeMarkdownNodeType,
        QuoteMarkdownNodeType,
        HTMLMarkdownNodeType,
        HeaderMarkdownNodeType,
        HRuleMarkdownNodeType,
        ListItemMarkdownNodeType,
        ParagraphMarkdownNodeType,
        TableMarkdownNodeType,
        TableRowMarkdownNodeType,
        TableCellMarkdownNodeType,
        UndefinedMarkdownNodeType = -1
    };

    /* Forward declaration of AST Node */
    class MarkdownNode;

    /**
     *  AST node
     */
    class MarkdownNode
    {
    public:
        typedef int Data;

        /** Markdown AST nodes collection */
        typedef std::vector<MarkdownNode> child_container;

        /** Node type */
        MarkdownNodeType type;

        /** Textual content, where applicable */
        ByteBuffer text;

        /** Additinonal data, if applicable */
        Data data;

        /** Source map of the node including any and all children */
        BytesRangeSet sourceMap;

        /** Parent node, throws exception if no parent is defined */
        MarkdownNode& parent();
        const MarkdownNode& parent() const;

        /** Sets parent node */
        void setParent(MarkdownNode* parent);

        /** True if section's parent is specified, false otherwise */
        bool hasParent() const noexcept;

        /** Children nodes */
        child_container& children() noexcept;
        const child_container& children() const noexcept;

        /** Constructor */
        MarkdownNode(MarkdownNodeType type_ = UndefinedMarkdownNodeType,
            MarkdownNode* parent_ = nullptr,
            const ByteBuffer& text_ = ByteBuffer(),
            const Data& data_ = Data());

        /** Copy constructor */
        MarkdownNode(const MarkdownNode& rhs) = default;

        /** Assignment operator */
        MarkdownNode& operator=(const MarkdownNode& rhs) = default;
        MarkdownNode& operator=(MarkdownNode&& rhs) = default;

        ~MarkdownNode() = default;

        /** Prints the node to the stdout */
        void printNode(size_t level = 0) const;

    private:
        MarkdownNode* m_parent;
        child_container m_children;
    };

    /** Markdown AST nodes collection */
    typedef MarkdownNode::child_container MarkdownNodes;

    /** Markdown AST nodes collection iterator */
    typedef MarkdownNodes::iterator MarkdownNodeIterator;
}

#endif
