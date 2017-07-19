//
//  refract/PrintVisitor.cc
//  librefract
//
//  Created by Vilibald Wanča on 09/11/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include <cassert>
#include <iostream>
#include "VisitorUtils.h"

#include "PrintVisitor.h"
#include "Visitor.h"

namespace refract
{
    PrintVisitor::PrintVisitor() : indent(0), os(std::cerr), ommitSrcMap(false)
    {
    }

    PrintVisitor::PrintVisitor(int indent_, std::ostream& os_,
                               bool ommitSrcMap_)
        : indent(indent_), os(os_), ommitSrcMap(ommitSrcMap_)
    {
    }

    std::ostream& PrintVisitor::indented()
    {
        for (int i = 0; i < indent; ++i) {
            os << "  ";
        }
        return os;
    }

    void PrintVisitor::printMeta(const IElement& e)
    {
        indented() << "- <meta>\n";

        for (const auto& m : e.meta) {
            PrintVisitor{indent + 1, os, ommitSrcMap}(*m);
        }
    }

    void PrintVisitor::printAttr(const IElement& e)
    {
        indented() << "- <attr>\n";

        for (const auto& a : e.attributes) {
            PrintVisitor{indent + 1, os, ommitSrcMap}(*a);
        }
    }

    void PrintVisitor::operator()(const IElement& e)
    {
        indented() << "+ " << e.element() << '\n';

        PrintVisitor pv{indent + 1, os, ommitSrcMap};

        refract::VisitBy(e, pv);

        pv.printMeta(e);
        pv.printAttr(e);
    }

    void PrintVisitor::operator()(const NullElement& e)
    {
        indented() << "- Null\n";
    }

    void PrintVisitor::operator()(const HolderElement& e)
    {
        indented() << "- Holder[" << e.element() << "]\n";

        assert(e.value);
        PrintVisitor{indent + 1, os, ommitSrcMap}(*e.value);
    }

    void PrintVisitor::operator()(const StringElement& e)
    {
        const StringElement::ValueType* v = GetValue<StringElement>(e);

        assert(v);
        indented() << "- String \"" << *v << "\"\n";
    }

    void PrintVisitor::operator()(const NumberElement& e)
    {
        const NumberElement::ValueType* v = GetValue<NumberElement>(e);

        auto& out = indented();
        out << "- Number ";
        if (v) {
            out << *v;
        }
        out << "\n";
    }

    void PrintVisitor::operator()(const BooleanElement& e)
    {
        const BooleanElement::ValueType* v = GetValue<BooleanElement>(e);

        auto& out = indented();
        out << "- Boolean ";
        if (v) {
            out << *v;
        }
        out << "\n";
    }

    void PrintVisitor::operator()(const RefElement& e)
    {
        const RefElement::ValueType* v = GetValue<RefElement>(e);

        assert(v);
        indented() << "- RefElement " << *v << "&\n";
    }

    void PrintVisitor::operator()(const MemberElement& e)
    {
        StringElement* str = TypeQueryVisitor::as<StringElement>(e.value.first);
        assert(str);

        if (ommitSrcMap && (str->value.compare("sourceMap") == 0)) return;

        auto& out = indented();
        out << "- MemberElement ";

        if (e.value.first) {
            if (str) {
                out << '[' << str->value << "]";
            } else {
                throw std::logic_error(
                    "A property's key in the object is not of type string");
            }
        }

        if (e.value.second) {
            out << ":\n";
            PrintVisitor{indent + 1, out, ommitSrcMap}(*e.value.second);
        } else {
            out << "\n";
        }
    }

    void PrintVisitor::operator()(const ArrayElement& e)
    {
        printValues(e, "Array");
    }

    void PrintVisitor::operator()(const EnumElement& e)
    {
        printValues(e, "Enum");
    }

    void PrintVisitor::operator()(const ObjectElement& e)
    {
        printValues(e, "Object");
    }

    void PrintVisitor::operator()(const ExtendElement& e)
    {
        printValues(e, "Extend");
    }

    void PrintVisitor::operator()(const OptionElement& e)
    {
        printValues(e, "Option");
    }

    void PrintVisitor::operator()(const SelectElement& e)
    {
        printValues(e, "Select");
    }

    void PrintVisitor::Visit(const IElement& e)
    {
        PrintVisitor ps;
        refract::Visit(ps, e);
    }

};  // namespace refract

#undef VISIT_IMPL
