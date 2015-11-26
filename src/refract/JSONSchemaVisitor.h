//
//  refract/JSONSchemaVisitor.h
//  librefract
//
//  Created by Vilibald Wanča on 09/11/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_JSONSCHEMAVISITOR_H
#define REFRACT_JSONSCHEMAVISITOR_H

#include "Visitor.h"
#include <string>

namespace refract
{

    // Forward declarations of Elements
    struct IElement;
    struct StringElement;
    struct NullElement;
    struct NumberElement;
    struct BooleanElement;
    struct ArrayElement;
    struct ObjectElement;
    struct MemberElement;

    class JSONSchemaVisitor : public IVisitor
    {
        ObjectElement *pObj;
        bool fixed;

        void setSchemaType(const std::string& type);
        void addMember(const std::string& key, IElement *val);
        void anyOf(std::map<std::string, std::vector<IElement*>>& types,
                   std::vector<std::string>& types_order);
        void enumElement(const ArrayElement& e, const ArrayElement::ValueType *val);

    public:
        JSONSchemaVisitor();
        void setFixed(bool fixit);
        void visit(const IElement& e);
        void visit(const MemberElement& e);
        void visit(const ObjectElement& e);
        void visit(const ArrayElement& e);

        void visit(const NullElement& e);
        void visit(const StringElement& e);
        void visit(const NumberElement& e);
        void visit(const BooleanElement& e);

        IElement* get();
        std::string getSchema(const IElement& e);
    };
}

#endif
