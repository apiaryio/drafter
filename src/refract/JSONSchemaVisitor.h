//
//  refract/JSONSchemaVisitor.h
//  librefract
//
//  Created by Vilibald Wanča on 09/11/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_JSONSCHEMAVISITOR_H
#define REFRACT_JSONSCHEMAVISITOR_H

#include "VisitorUtils.h"
#include <string>

#include "ElementFwd.h"

namespace refract
{
    class JSONSchemaVisitor 
    {
        ObjectElement *pObj;
        ObjectElement *pDefs;
        bool fixed;
        bool fixedType;

        void setSchemaType(const std::string& type);
        void addSchemaType(const std::string& type);
        void addMember(const std::string& key, IElement *val);
        void anyOf(std::map<std::string, std::vector<IElement*> >& types, std::vector<std::string>& typesOrder);
        bool allItemsEmpty(const ArrayElement::ValueType* val);
        ObjectElement* definitionFromVariableProperty(JSONSchemaVisitor& renderer);
        void addVariableProps(std::vector<MemberElement*>& props,ObjectElement *o);
        ArrayElement* arrayFromProps(std::vector<MemberElement*>& props);

        template<typename T>
        void setPrimitiveType(const T& e)
        {
            //FIXME: static_assert is missing in our code base
            //static_assert(sizeof(T) == 0, "Only String, Number, Boolean types allowed");

            throw std::runtime_error("Only String, Number, Boolean types allowed");
        };

        template<typename T> void primitiveType(const T& e);

    public:
        JSONSchemaVisitor(ObjectElement *pDefinitions = NULL,
                          bool _fixed = false,
                          bool _fixedType = false);
        ~JSONSchemaVisitor();
        void setFixed(bool _fixed);
        void setFixedType(bool _fixedType);
        void operator()(const IElement& e);
        void operator()(const MemberElement& e);
        void operator()(const ObjectElement& e);
        void operator()(const ArrayElement& e);
        void operator()(const EnumElement& e);

        void operator()(const NullElement& e);
        void operator()(const StringElement& e);
        void operator()(const NumberElement& e);
        void operator()(const BooleanElement& e);
        void operator()(const ExtendElement& e);

        void operator()(const OptionElement& e);

        IElement* get();
        IElement* getOwnership();
        std::string getSchema(const IElement& e);
    };
}

#endif
