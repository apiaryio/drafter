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
    PrintVisitor::PrintVisitor() : indent(0), os(std::cerr), ommitSourceMap(false)
    {
    }

    PrintVisitor::PrintVisitor(int indent_, std::ostream& os_, bool ommitSourceMap_)
        : indent(indent_), os(os_), ommitSourceMap(ommitSourceMap_)
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
            PrintVisitor{ indent + 1, os, ommitSourceMap }(*m);
        }
    }

    void PrintVisitor::printAttr(const IElement& e)
    {
        indented() << "- <attr>\n";

        for (const auto& a : e.attributes) {
            if (const auto mPtr = TypeQueryVisitor::as<MemberElement>(a))
                if (const auto strPtr = TypeQueryVisitor::as<StringElement>(mPtr->value.first))
                    if (ommitSourceMap && (strPtr->value.compare("sourceMap") == 0))
                        continue;

            PrintVisitor{ indent + 1, os, ommitSourceMap }(*a);
        }
    }

    void PrintVisitor::operator()(const IElement& e)
    {
        indented() << "+ " << e.element() << '\n';

        PrintVisitor pv{ indent + 1, os, ommitSourceMap };

        pv.printMeta(e);
        pv.printAttr(e);

        refract::VisitBy(e, pv);
    }

    void PrintVisitor::operator()(const NullElement& e)
    {
        indented() << "- Null\n";
    }

    void PrintVisitor::operator()(const HolderElement& e)
    {
        indented() << "- Holder[" << e.element() << "]\n";

        assert(e.value);
        PrintVisitor{ indent + 1, os, ommitSourceMap }(*e.value);
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

        assert(v);
        indented() << "- Number " << *v << "\n";
    }

    void PrintVisitor::operator()(const BooleanElement& e)
    {
        const BooleanElement::ValueType* v = GetValue<BooleanElement>(e);

        assert(v);
        indented() << "- Boolean " << *v << "\n";
    }

    void PrintVisitor::operator()(const RefElement& e)
    {
        const RefElement::ValueType* v = GetValue<RefElement>(e);

        assert(v);
        indented() << "- RefElement " << *v << "&\n";
    }

    void PrintVisitor::operator()(const MemberElement& e)
    {
        indented() << "- MemberElement\n";

        const auto keyPtr = e.value.first;
        assert(keyPtr);
        PrintVisitor{ indent + 1, os, ommitSourceMap }(*keyPtr);

        const auto valuePtr = e.value.second;
        assert(valuePtr);
        PrintVisitor{ indent + 1, os, ommitSourceMap }(*valuePtr);
    }

    void PrintVisitor::operator()(const ArrayElement& e)
    {
        printValues(e, "Array");
    }

    void PrintVisitor::operator()(const EnumElement& e)
    {
        //printValues(e, "Enum");
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

}; // namespace refract

#undef VISIT_IMPL
