//
//  refract/JSONSchemaVisitor.h
//  librefract
//
//  Created by Vilibald Wanča on 09/11/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_JSONSCHEMAVISITOR_H
#define REFRACT_JSONSCHEMAVISITOR_H

#include "ElementFwd.h"
#include "VisitorUtils.h"

#include <memory>
#include <string>
#include <map>
#include <set>

namespace refract
{

    template <typename T>
    constexpr const char* json_schema_type;

    template <>
    constexpr const char* json_schema_type<BooleanElement> = "boolean";

    template <>
    constexpr const char* json_schema_type<StringElement> = "string";

    template <>
    constexpr const char* json_schema_type<NumberElement> = "number";

    class JSONSchemaVisitor final
    {
        std::unique_ptr<ObjectElement> pObj;
        ObjectElement& pDefs;

        bool fixed;
        bool fixedType;

        void setSchemaType(const std::string& type);
        void addSchemaType(const std::string& type);
        void addNullToEnum();
        void addMember(const std::string& key, std::unique_ptr<IElement> val);
        void anyOf(std::map<std::string, std::vector<const IElement*> >& types, std::vector<std::string>& typesOrder);
        bool allItemsEmpty(const ArrayElement::ValueType* val);
        std::unique_ptr<ObjectElement> definitionFromVariableProperty(JSONSchemaVisitor& renderer);
        void addVariableProps(std::vector<const MemberElement*>& props, std::unique_ptr<ObjectElement> o);
        std::unique_ptr<ArrayElement> arrayFromProps(std::vector<const MemberElement*>& props);

        template <typename T>
        void primitiveType(const T& e)
        {
            if (auto value = GetValue<T>{}(e)) {
                setSchemaType(json_schema_type<T>);

                if (fixed) {
                    auto a = make_element<ArrayElement>();
                    a->get().push_back(value->empty() ? //
                            make_element<T>() :
                            make_element<T>(value->get()));
                    addMember("enum", std::move(a));
                }
            }
        }

        void processMember(const IElement& member,
            std::vector<const MemberElement*>& varProps,
            dsd::Array& oneOfMembers,
            ObjectElement& o,
            std::set<std::string>& required);

        template <typename ContentT>
        void processMembers(const ContentT& members,
            ArrayElement::ValueType& reqVals,
            std::vector<const MemberElement*>& varProps,
            dsd::Array& oneOfMembers,
            ObjectElement& o)
        {
            std::set<std::string> required;

            for (const auto& member : members) {
                if (!member)
                    continue;
                processMember(*member, varProps, oneOfMembers, o, required);
            }

            std::transform(required.begin(), required.end(), std::back_inserter(reqVals), [](const std::string& value) {
                return from_primitive(value);
            });
        }

    public:
        JSONSchemaVisitor(ObjectElement& pDefinitions, bool _fixed = false, bool _fixedType = false);

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

        ObjectElement* get();
        std::unique_ptr<ObjectElement> getOwnership();
    };

    std::string renderJsonSchema(const IElement& e);
}

#endif
