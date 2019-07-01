//
//  MarkdownNode.cc
//  markdownparser
//
//  Created by Zdenek Nemec on 4/19/14.
//  Copyright (c) 2014 Apiary Inc. All rights reserved.
//

#ifdef DEBUG
#include <iostream>
#endif

#include "MarkdownNode.h"

using namespace mdp;

MarkdownNode::MarkdownNode(MarkdownNodeType type_, MarkdownNode* parent_, const ByteBuffer& text_, const Data& data_)
    : type(type_), text(text_), data(data_), m_parent(parent_)
{
    m_children.reset(::new MarkdownNodes);
}

MarkdownNode::MarkdownNode(const MarkdownNode& rhs)
{
    this->type = rhs.type;
    this->text = rhs.text;
    this->data = rhs.data;
    this->sourceMap = rhs.sourceMap;
    this->m_children.reset(::new MarkdownNodes(*rhs.m_children.get()));
    this->m_parent = rhs.m_parent;
}

MarkdownNode& MarkdownNode::operator=(const MarkdownNode& rhs)
{
    this->type = rhs.type;
    this->text = rhs.text;
    this->data = rhs.data;
    this->sourceMap = rhs.sourceMap;
    this->m_children.reset(::new MarkdownNodes(*rhs.m_children.get()));
    this->m_parent = rhs.m_parent;
    return *this;
}

MarkdownNode::~MarkdownNode() {}

MarkdownNode& MarkdownNode::parent()
{
    if (!hasParent())
        throw "no parent set";
    return *m_parent;
}

const MarkdownNode& MarkdownNode::parent() const
{
    if (!hasParent())
        throw "no parent set";
    return *m_parent;
}

void MarkdownNode::setParent(MarkdownNode* parent)
{
    m_parent = parent;
}

bool MarkdownNode::hasParent() const
{
    return (m_parent != NULL);
}

MarkdownNodes& MarkdownNode::children()
{
    if (!m_children.get())
        throw "no children set";

    return *m_children;
}

const MarkdownNodes& MarkdownNode::children() const
{
    if (!m_children.get())
        throw "no children set";

    return *m_children;
}

#ifdef DEBUG
void MarkdownNode::printNode(size_t level) const
{
    using std::cerr;
    for (size_t i = 0; i < level; ++i)
        cerr << "  ";

    cerr << "+ ";
    switch (type) {
        case RootMarkdownNodeType:
            cerr << "root";
            break;

        case CodeMarkdownNodeType:
            cerr << "code";
            break;

        case QuoteMarkdownNodeType:
            cerr << "quote";
            break;

        case HTMLMarkdownNodeType:
            cerr << "HTML";
            break;

        case HeaderMarkdownNodeType:
            cerr << "header";
            break;

        case HRuleMarkdownNodeType:
            cerr << "hrul";
            break;

        case ListItemMarkdownNodeType:
            cerr << "list item";
            break;

        case ParagraphMarkdownNodeType:
            cerr << "paragraph";
            break;

        default:
            cerr << "undefined";
            break;
    }

    cerr << " (type " << type << ", data " << data << ") - ";
    cerr << "`" << text << "`";

    if (!sourceMap.empty()) {
        for (mdp::BytesRangeSet::const_iterator it = sourceMap.begin(); it != sourceMap.end(); ++it) {
            std::cerr << ((it == sourceMap.begin()) ? " :" : ";");
            std::cerr << it->location << ":" << it->length;
        }
    }

    cerr << std::endl;

    for (MarkdownNodeIterator it = m_children->begin(); it != m_children->end(); ++it) {
        it->printNode(level + 1);
    }

    if (level == 0)
        cerr << std::endl << std::endl;
}
#endif
