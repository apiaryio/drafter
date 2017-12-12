//
//  refract/PrintVisitor.h
//  librefract
//
//  Created by Vilibald Wanča on 09/11/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_PRINTVISITOR_H
#define REFRACT_PRINTVISITOR_H

#include <ostream>

#include "ElementFwd.h"
#include "ElementIfc.h"

namespace refract
{
    class PrintVisitor
    {
        int indent;
        std::ostream& os;
        bool ommitSourceMap;

    private:
        std::ostream& indented();

        void printMeta(const IElement& e);
        void printAttr(const IElement& e);

        template <typename T>
        void printValues(const T& e, const char* name)
        {
            indented() << "- " << name << "Element\n";

            if (!e.empty())
                for (const auto& v : e.get()) {
                    PrintVisitor{ indent + 1, os, ommitSourceMap }(*v);
                }
        }

    public:
        PrintVisitor();
        PrintVisitor(int indentation, std::ostream& os, bool ommitSourceMap = false);

        void operator()(const IElement& e);
        void operator()(const MemberElement& e);
        void operator()(const ObjectElement& e);
        void operator()(const ArrayElement& e);
        void operator()(const EnumElement& e);
        void operator()(const NullElement& e);
        void operator()(const HolderElement& e);
        void operator()(const StringElement& e);
        void operator()(const NumberElement& e);
        void operator()(const BooleanElement& e);
        void operator()(const RefElement& e);
        void operator()(const ExtendElement& e);
        void operator()(const OptionElement& e);
        void operator()(const SelectElement& e);

        static void Visit(const IElement& e);
    };

    ///
    /// Applies PrintVisitor on given element and dumps the output to a new file
    /// on each encounter.
    ///
    /// @return index of the file logged to
    ///
    int log_to_files(const IElement& e, const std::string& prefix = "print");
}

#endif
