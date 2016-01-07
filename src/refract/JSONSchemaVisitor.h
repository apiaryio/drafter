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
#include "VisitorUtils.h"
#include <string>

#include "ElementFwd.h"

namespace refract
{

    class JSONSchemaVisitor : public IVisitor
    {
        ObjectElement *pObj;
        bool fixed;

        void setSchemaType(const std::string& type);
        void addSchemaType(const std::string& type);
        void addMember(const std::string& key, IElement *val);
        void anyOf(std::map<std::string, std::vector<IElement*> >& types, std::vector<std::string>& typesOrder);
        bool allItemsEmpty(const ArrayElement::ValueType* val);

        template<typename T>
        void setPrimitiveType(const T& e)
        {
            //FIXME: static_assert is missing in our code base
            //static_assert(sizeof(T) == 0, "Only String, Number, Boolean types allowed");

            throw std::runtime_error("Only String, Number, Boolean types allowed");
        };

        template<typename T> void primitiveType(const T& e);

    public:
        JSONSchemaVisitor(bool fixit = false);
        ~JSONSchemaVisitor();
        void setFixed(bool fixit);
        void visit(const IElement& e);
        void visit(const MemberElement& e);
        void visit(const ObjectElement& e);
        void visit(const ArrayElement& e);
        void visit(const EnumElement& e);

        void visit(const NullElement& e);
        void visit(const StringElement& e);
        void visit(const NumberElement& e);
        void visit(const BooleanElement& e);
        void visit(const ExtendElement& e);

        IElement* get();
        IElement* getOwnership();
        std::string getSchema(const IElement& e);
    };
}

#endif
