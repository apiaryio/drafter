//
//  refract/PrintVisitor.cc
//  librefract
//
//  Created by Vilibald Wanča on 09/11/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "VisitorUtils.h"
#include <sstream>
#include <iostream>

namespace refract
{
    PrintVisitor::PrintVisitor()
    : indent(0), os(std::cout) {}

    PrintVisitor::PrintVisitor(int indent_, std::ostream& os_)
    : indent(indent_), os(os_) {}

    void PrintVisitor::indentOS(int ind)
    {
        for (int i = 0; i < ind; i++) {
            os << "  ";
        }
    }

    void PrintVisitor::printMeta(const IElement& e)
    {
        if (e.meta.size() > 0) {
            os << "meta {\n";
            indentOS(indent);

            for (IElement::MemberElementCollection::const_iterator i = e.meta.begin();
                i != e.meta.end();
                ++i) {

                visit(*(*i));
            }

            indentOS(indent);
            os << "}\n";
        }
    }

    void PrintVisitor::visit(const IElement& e)
    {
        indentOS(indent);
        printMeta(e);
        e.content(*this);
    }

    void PrintVisitor::visit(const NullElement& e)
    {
        os << "NullElement(null)" << "\n";
    }

    void PrintVisitor::visit(const StringElement& e)
    {
        const StringElement::ValueType* v = GetValue<StringElement>(e);
        os << "StringElement(" << *v << ")" << "\n";
    }

    void PrintVisitor::visit(const NumberElement& e)
    {
        const NumberElement::ValueType* v = GetValue<NumberElement>(e);

        os << "NumberElement(";
        if (v) {
            os << *v;
        } else {
            os << "null";
        }
        os << ")" << "\n";
    }

    void PrintVisitor::visit(const BooleanElement& e)
    {
        const BooleanElement::ValueType* v = GetValue<BooleanElement>(e);

        os << "BooleanElement(";
        if (v) {
            os << *v;
        } else {
            os << "null";
        }
        os << ")" << "\n";
    }

    void PrintVisitor::visit(const ArrayElement& e)
    {
        typedef ArrayElement::ValueType::const_iterator iterator;
        PrintVisitor ps(indent + 1, os);

        os << "ArrayElement {\n";
        for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
            ps.visit(*(*it));
        }
        indentOS(indent);
        os << "}\n";
    }

    void PrintVisitor::visit(const EnumElement& e)
    {
        typedef ArrayElement::ValueType::const_iterator iterator;
        PrintVisitor ps(indent + 1, os);

        os << "EnumElement {\n";
        for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
            ps.visit(*(*it));
        }
        indentOS(indent);
        os << "}\n";
    }

    void PrintVisitor::visit(const MemberElement& e)
    {
        os << "MemberElement {\n";
        indentOS(indent + 1);
        if (e.value.first) {
            if (StringElement* str = TypeQueryVisitor::as<StringElement>(e.value.first)) {
                os<< "\"" << str->value << "\": \n";
            } else {
                throw std::logic_error("A property's key in the object is not of type string");
            }
        }

        if (e.value.second) {
            PrintVisitor ps(indent + 1, os);
            ps.visit(*e.value.second);
        }
        indentOS(indent);
        os << "}\n";
    }


    void PrintVisitor::visit(const ObjectElement& e)
    {
        typedef ObjectElement::ValueType::const_iterator iterator;
        PrintVisitor ps(indent + 1, os);

        os << "ObjectElement {\n";
        for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
            ps.visit(*(*it));
        }
        indentOS(indent);
        os << "}\n";
    }

    void PrintVisitor::visit(const ExtendElement& e)
    {
        typedef ExtendElement::ValueType::const_iterator iterator;
        PrintVisitor ps(indent + 1, os);

        os << "ExtendElement {\n";
        for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
            ps.visit(*(*it));
        }
        indentOS(indent);
        os << "}\n";
    }

    void PrintVisitor::Visit(const IElement& e)
    {
        PrintVisitor ps;
        ps.visit(e);
    }

}; // namespace refract
