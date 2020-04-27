//
//  refract/ExpandVisitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_EXPANDVISITOR_H
#define REFRACT_EXPANDVISITOR_H

#include "ElementFwd.h"
#include "ElementIfc.h"
#include <memory>

namespace refract
{

    class Registry;

    class ExpandVisitor
    {

    public:
        struct Context;

        ExpandVisitor(const Registry& registry);
        ~ExpandVisitor();

        void operator()(const IElement& e);

        void operator()(const NullElement& e);

        void operator()(const StringElement& e);
        void operator()(const NumberElement& e);
        void operator()(const BooleanElement& e);
        void operator()(const HolderElement& e);

        void operator()(const MemberElement& e);

        void operator()(const ArrayElement& e);
        void operator()(const EnumElement& e);
        void operator()(const ObjectElement& e);

        void operator()(const RefElement& e);
        void operator()(const ExtendElement& e);

        void operator()(const OptionElement& e);
        void operator()(const SelectElement& e);

        // return expanded elemnt or NULL if expansion is not needed
        // caller responsibility is to delete returned Element
        std::unique_ptr<IElement> get();

    private:
        std::unique_ptr<IElement> result;
        Context* context;
    };

}; // namespace refract

#endif // #ifndef REFRACT_EXPANVISITOR_H
