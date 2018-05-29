//
//  refract/JsonSchema.cc
//  librefract
//
//  Created by Thomas Jandecka on 16/02/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "JsonSchema.h"

#include "../ElementData.h"
#include "../utils/log/Trivial.h"
#include "../utils/so/JsonIo.h"
#include "Element.h"
#include "Utils.h"
#include <algorithm>
#include <bitset>
#include <cassert>
#include <regex>

using namespace refract;
using namespace schema;
using namespace drafter::utils;
using namespace drafter::utils::log;

namespace
{ // API Elements tools
    template <typename Element>
    Element* get(IElement* e)
    {
        return dynamic_cast<Element*>(e);
    }

    template <typename Element>
    Element* get(const IElement* e)
    {
        return dynamic_cast<Element*>(e);
    }

    so::String instantiate(const StringElement& e)
    {
        assert(!e.empty());
        return so::String{ e.get().get() };
    }

    so::Number instantiate(const NumberElement& e)
    {
        assert(!e.empty());
        return so::Number{ e.get().get() };
    }

    so::Value instantiate(const BooleanElement& e)
    {
        assert(!e.empty());
        return e.get() ? //
            so::Value{ in_place_type<so::True>{} } :
            so::Value{ in_place_type<so::False>{} };
    }

    bool hasTypeAttr(const IElement& e, const char* name)
    {
        auto typeAttrIt = e.attributes().find("typeAttributes");

        if (typeAttrIt != e.attributes().end())
            if (const auto* typeAttrs = get<const ArrayElement>(typeAttrIt->second.get())) {
                const auto b = typeAttrs->get().begin();
                const auto e = typeAttrs->get().end();
                return e != std::find_if(b, e, [&name](const auto& el) { //
                    const auto* entry = get<const StringElement>(el.get());
                    return entry && !entry->empty() && (entry->get().get() == name);
                });
            }
        return false;
    }

    bool hasFixedAttr(const IElement& e)
    {
        return hasTypeAttr(e, "fixed");
    }

    bool hasFixedTypeAttr(const IElement& e)
    {
        return hasTypeAttr(e, "fixedType");
    }

    bool hasRequiredTypeAttr(const IElement& e)
    {
        return hasTypeAttr(e, "required");
    }

    bool hasOptionalTypeAttr(const IElement& e)
    {
        return hasTypeAttr(e, "optional");
    }

    bool hasNullableTypeAttr(const IElement& e)
    {
        return hasTypeAttr(e, "nullable");
    }

    bool isVariable(const IElement& e)
    {
        const auto it = e.attributes().find("variable");
        if (it == e.attributes().end())
            return false;

        assert(it->second);
        if (auto value = get<const BooleanElement>(it->second.get())) {
            if (value->empty()) {
                LOG(warning) << "empty data structure element in backend";
                return false;
            }

            return value->get().get();
        } else {
            LOG(warning) << "variable attribute must hold Boolean Element; encountered " << it->second->element();
            return false;
        }
        return false;
    }

    std::string key(const MemberElement& m)
    {
        if (const auto& strKey = get<const StringElement>(m.get().key())) {
            if (strKey->empty())
                return "";
            return strKey->get().get();
        } else {
            LOG(error) << "Non-string key in Member Element: " << m.get().key()->element();
            assert(false);
        }
    }
} // namespace

namespace
{
    using TypeAttributes = std::bitset<3>;
    constexpr std::size_t FIXED_FLAG = 0;
    constexpr std::size_t FIXED_TYPE_FLAG = 1;
    constexpr std::size_t NULLABLE_FLAG = 2;

    TypeAttributes updateTypeAttributes(const IElement& e, TypeAttributes options) noexcept
    {
        if (hasFixedAttr(e))
            options.set(FIXED_FLAG);

        if (hasFixedTypeAttr(e))
            options.set(FIXED_TYPE_FLAG);

        if (hasNullableTypeAttr(e))
            options.set(NULLABLE_FLAG);

        return options;
    }

    TypeAttributes pass_flags(TypeAttributes options) noexcept
    {
        return options;
    }

    TypeAttributes inherit_flags(TypeAttributes options) noexcept
    {
        options.reset(FIXED_TYPE_FLAG);
        options.reset(NULLABLE_FLAG);
        return options;
    }

} // namespace

namespace
{ // JSON Schema tools
    so::Object& addSchemaVersion(so::Object& schema)
    {
        schema.data.emplace_back("$schema", so::String{ "http://json-schema.org/draft-04/schema#" });
        return schema;
    }

    so::Object& addType(so::Object& schema, const char* type)
    {
        schema.data.emplace_back("type", so::String{ type });
        return schema;
    }

    so::Object& addItems(so::Object& schema, so::Value value)
    {
        schema.data.emplace_back("items", std::move(value));
        return schema;
    }

    so::Object& addProperties(so::Object& schema, so::Object value)
    {
        schema.data.emplace_back("properties", std::move(value));
        return schema;
    }

    so::Object& addOneOf(so::Object& schema, so::Array value)
    {
        schema.data.emplace_back("oneOf", std::move(value));
        return schema;
    }

    so::Object& addAllOf(so::Object& schema, so::Array value)
    {
        schema.data.emplace_back("allOf", std::move(value));
        return schema;
    }

    so::Object& addAnyOf(so::Object& schema, so::Array value)
    {
        schema.data.emplace_back("anyOf", std::move(value));
        return schema;
    }

    so::Object& addRequired(so::Object& schema, so::Array value)
    {
        schema.data.emplace_back("required", std::move(value));
        return schema;
    }

    so::Object& addPatternProperties(so::Object& schema, so::Object value)
    {
        schema.data.emplace_back("patternProperties", std::move(value));
        return schema;
    }

    template <typename... Args>
    so::Object& addEnum(so::Object& schema, Args&&... args)
    {
        schema.data.emplace_back("enum", so::Array{ so::Value{ std::forward<Args>(args) }... });
        return schema;
    }

    so::Object& addAdditionalItems(so::Object& schema, so::Value value)
    {
        schema.data.emplace_back("additionalItems", std::move(value));
        return schema;
    }

    so::Object& addAdditionalProperties(so::Object& schema, so::Value value)
    {
        schema.data.emplace_back("additionalProperties", std::move(value));
        return schema;
    }

    so::Object& addMinItems(so::Object& schema, unsigned value)
    {
        schema.data.emplace_back("minItems", so::Number{ static_cast<double>(value) });
        return schema;
    }

    so::Object nullSchema()
    {
        return so::Object{ std::make_pair("type", so::String{ "null" }) };
    }

    so::Object& wrapNullable(so::Object& s, TypeAttributes options)
    {
        if (options.test(NULLABLE_FLAG)) {
            addAnyOf(s, so::Array{ nullSchema(), so::Object{} });
            return get<so::Object>(get<so::Array>(s.data.back().second).data.at(1));
        }
        return s;
    }

    ///
    /// Renders a regular expression from a string data structure element
    ///   according to ECMA262 regular expression dialect.
    ///
    /// @param e    string data structure element
    /// @return     matching regular expression
    ///
    std::string renderPattern(const StringElement& e, TypeAttributes options)
    {
        // clang-format off
        if (options.test(FIXED_FLAG) || hasFixedAttr(e)) {
            if(e.empty()) {
                return R"(^(?![\s\S]))";
            } else {
                std::regex sanitizer{ R"([-[\]{}()*+?.,\^$|#\s])" };
                return std::regex_replace(e.get().get(), sanitizer, R"(\$&)");
            }
        }
        // clang-format on

        return "(?:)";
    }

    struct ObjectSchema {
        so::Object properties;
        so::Object patternProperties;
        so::Array required;
        so::Array allOf;
    };

    so::Object& materialize(so::Object& result, ObjectSchema s)
    {
        if (!s.properties.data.empty())
            addProperties(result, std::move(s.properties));

        if (!s.patternProperties.data.empty())
            addPatternProperties(result, std::move(s.patternProperties));

        if (!s.allOf.data.empty())
            addAllOf(result, std::move(s.allOf));

        if (!s.required.data.empty())
            addRequired(result, std::move(s.required));

        return result;
    }

    so::Object materialize(ObjectSchema s)
    {
        so::Object result;

        return materialize(result, s);
    }
} // namespace

namespace
{
    void renderProperty(ObjectSchema& s, const MemberElement& e, TypeAttributes options);
    void renderProperty(ObjectSchema& s, const RefElement& e, TypeAttributes options);
    void renderProperty(ObjectSchema& s, const SelectElement& e, TypeAttributes options);
    void renderProperty(ObjectSchema& s, const ObjectElement& e, TypeAttributes options);
    void renderProperty(ObjectSchema& s, const ExtendElement& e, TypeAttributes options);
    void renderProperty(ObjectSchema& s, const IElement& e, TypeAttributes options);

    so::Object& renderSchema(so::Object& schema, const ObjectElement& e, TypeAttributes options);
    so::Object& renderSchema(so::Object& schema, const ArrayElement& e, TypeAttributes options);
    so::Object& renderSchema(so::Object& schema, const EnumElement& e, TypeAttributes options);
    so::Object& renderSchema(so::Object& schema, const NullElement& e, TypeAttributes options);
    so::Object& renderSchema(so::Object& schema, const StringElement& e, TypeAttributes options);
    so::Object& renderSchema(so::Object& schema, const NumberElement& e, TypeAttributes options);
    so::Object& renderSchema(so::Object& schema, const BooleanElement& e, TypeAttributes options);
    so::Object& renderSchema(so::Object& schema, const ExtendElement& e, TypeAttributes options);
    so::Object& renderSchema(so::Object& schema, const IElement& e, TypeAttributes options);

    template <typename T>
    void renderProperty(ObjectSchema&, const T& e, so::Object&, TypeAttributes)
    {
        LOG(error) << "invalid property element: " << e.element();
        assert(false);
    }

    template <typename T>
    so::Object& renderSchema(so::Object& s, so::Object&, const T& e, TypeAttributes)
    {
        LOG(error) << "invalid top level element: " << e.element();
        assert(false);
        return s;
    }

    so::Object& renderSchema(so::Object& schema, const IElement& e, TypeAttributes options)
    {
        auto schemaPtr = &schema;
        refract::visit(e, [schemaPtr, options](const auto& el) { //
            renderSchema(*schemaPtr, el, options);
        });
        return schema;
    }

    so::Object makeSchema(const IElement& e, TypeAttributes options)
    {
        so::Object result{};
        renderSchema(result, e, options);
        return result;
    }
} // namespace

so::Object schema::generateJsonSchema(const IElement& el)
{
    so::Object result{};

    addSchemaVersion(result);
    renderSchema(result, el, TypeAttributes{});

    return result;
}

namespace
{

    so::Object& renderSchema(so::Object& s, const ObjectElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering ObjectElement to JSON Schema";

        constexpr const char* TYPE_NAME = "object";

        options = updateTypeAttributes(e, options);
        auto& schema = wrapNullable(s, options);

        addType(schema, TYPE_NAME);

        ObjectSchema result{};
        if (e.empty())
            LOG(warning) << "empty data structure element in backend";
        else
            for (const auto& item : e.get()) {
                assert(item);
                renderProperty(result, *item, inherit_flags(options));
            }

        // every property is required iff fixed or fixedType
        if (options.test(FIXED_TYPE_FLAG) || options.test(FIXED_FLAG)) {
            result.required.data.clear();
            std::transform(result.properties.data.begin(), //
                result.properties.data.end(),              //
                std::back_inserter(result.required.data),  //
                [](const auto& property) { return so::String{ property.first }; });
        }

        materialize(schema, std::move(result));

        if (options.test(FIXED_TYPE_FLAG) || options.test(FIXED_FLAG)) {
            addAdditionalProperties(schema, so::False{});
        }

        return schema;
    }

    so::Object& renderSchema(so::Object& s, const ArrayElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering ArrayElement to JSON Schema";

        constexpr const char* TYPE_NAME = "array";

        options = updateTypeAttributes(e, options);

        if (options.test(FIXED_TYPE_FLAG)) { // array of any of types

            if (e.empty() || e.get().empty()) {
                auto& schema = wrapNullable(s, options);
                addType(schema, TYPE_NAME);
                addEnum(schema, so::Array{});

            } else if (e.get().size() == 1) {
                auto item_schema = makeSchema(*e.get().begin()[0], inherit_flags(options));
                auto& schema = wrapNullable(s, options);
                addType(schema, TYPE_NAME);
                addItems(schema, std::move(item_schema));

            } else {
                so::Array items{};
                for (const auto& entry : e.get())
                    items.data.emplace_back(makeSchema(*entry, inherit_flags(options)));
                auto& schema = wrapNullable(s, options);
                addType(schema, TYPE_NAME);
                addItems(schema, so::Object{ std::make_pair("anyOf", std::move(items)) });
            }

        } else if (options.test(FIXED_FLAG)) { // tuple of N constants/types

            if (e.empty() || e.get().empty()) {
                auto& schema = wrapNullable(s, options);
                addType(schema, TYPE_NAME);
                addEnum(schema, so::Array{});

            } else {
                so::Array items{};
                for (const auto& item : e.get())
                    items.data.emplace_back(makeSchema(*item, inherit_flags(options)));

                auto& schema = wrapNullable(s, options);
                addType(schema, TYPE_NAME);
                addMinItems(schema, items.data.size());  // minimum of N entries
                addItems(schema, std::move(items));      // schemas of tuple entries
                addAdditionalItems(schema, so::False{}); // no more entries
            }
        } else {
            auto& schema = wrapNullable(s, options);
            addType(schema, TYPE_NAME);
        }

        return s;
    }

    so::Object& renderSchema(so::Object& schema, const EnumElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering EnumElement to JSON Schema";

        options = updateTypeAttributes(e, options);

        so::Array anyOf{};

        if (options.test(NULLABLE_FLAG))
            anyOf.data.emplace_back(nullSchema());

        auto enumerationsIt = e.attributes().find("enumerations");
        if (e.attributes().end() != enumerationsIt) {

            const auto enums = get<const ArrayElement>(enumerationsIt->second.get());
            assert(enums);
            assert(!enums->empty());

            for (const auto& enumEntry : enums->get())
                anyOf.data.emplace_back(makeSchema(*enumEntry, inherit_flags(options)));
        } else {
            LOG(warning) << "Enum Element SHALL hold enumerations attribute; interpreting as empty";
        }

        schema.data.emplace_back("anyOf", std::move(anyOf));

        return schema;
    } // namespace

    so::Object& renderSchema(so::Object& schema, const NullElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering NullElement to JSON Schema";

        addType(schema, "null");
        return schema;
    }

    so::Object& renderSchema(so::Object& s, const StringElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering StringElement to JSON Schema";

        options = updateTypeAttributes(e, options);
        auto& schema = wrapNullable(s, options);

        addType(schema, "string");

        if (options.test(FIXED_FLAG))
            if (!e.empty())
                addEnum(schema, instantiate(e));

        return schema;
    }

    so::Object& renderSchema(so::Object& s, const NumberElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering NumberElement to JSON Schema";

        options = updateTypeAttributes(e, options);
        auto& schema = wrapNullable(s, options);

        addType(schema, "number");

        if (options.test(FIXED_FLAG))
            if (!e.empty())
                addEnum(schema, instantiate(e));

        return schema;
    }

    so::Object& renderSchema(so::Object& s, const BooleanElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering BooleanElement to JSON Schema";

        options = updateTypeAttributes(e, options);
        auto& schema = wrapNullable(s, options);

        addType(schema, "boolean");

        if (options.test(FIXED_FLAG))
            if (!e.empty())
                addEnum(schema, instantiate(e));

        return schema;
    }

    so::Object& renderSchema(so::Object& s, const ExtendElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering ExtendElement to JSON Schema";

        auto merged = e.get().merge();
        renderSchema(s, *merged, options);
        return s;
    }

} // namespace

namespace
{

    void renderProperty(ObjectSchema& s, const MemberElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering property MemberElement as JSON Schema";

        if (hasFixedAttr(e))
            options.set(FIXED_FLAG);

        options.set(FIXED_TYPE_FLAG, hasFixedTypeAttr(e));
        options.set(NULLABLE_FLAG, hasNullableTypeAttr(e));

        const auto k = e.get().key();
        const auto v = e.get().value();

        assert(k);
        if (isVariable(e)) {

            if (const auto& extKey = get<const ExtendElement>(k)) {
                auto mergedKey = extKey->get().merge();
                auto strKey = get<const StringElement>(mergedKey.get());

                if (!strKey) {
                    LOG(error) << "Merging Member Element key yielded other than String Element: "
                               << mergedKey->element();
                    assert(false);
                }

                emplace_unique(s.patternProperties, //
                    renderPattern(*strKey, pass_flags(options)),
                    makeSchema(*v, pass_flags(options)));

            } else if (const auto& strKey = get<const StringElement>(k)) {

                emplace_unique(s.patternProperties, //
                    renderPattern(*strKey, pass_flags(options)),
                    makeSchema(*v, pass_flags(options)));

            } else {
                LOG(error) << "Unexpected element type in Member Element key: " << k->element();
                assert(false);
            }

        } else {
            auto strKey = key(e);

            s.properties.data.emplace_back(strKey, makeSchema(*v, pass_flags(options)));

            if (hasRequiredTypeAttr(e) && !hasOptionalTypeAttr(e))
                s.required.data.emplace_back(so::String{ strKey });
        }
    }

    void renderProperty(ObjectSchema& s, const RefElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering property RefElement as JSON Schema";

        const auto& resolvedEntry = e.attributes().find("resolved");
        if (resolvedEntry == e.attributes().end()) {
            LOG(error) << "expected all references to be resolved in backend";
            assert(false);
        }

        assert(resolvedEntry->second);
        renderProperty(s, *resolvedEntry->second, options);
    }

    void renderProperty(ObjectSchema& s, const SelectElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering property SelectElement as JSON Schema";

        so::Array oneOfs{};
        for (const auto& option : e.get()) {

            if (option->empty()) {
                LOG(error) << "unexpected empty option element in backend";
                assert(false);
            }

            if (option->get().size() < 1)
                LOG(warning) << "empty option element in backend";

            ObjectSchema optionSchema{};
            for (const auto& optionEntry : option->get()) {
                assert(optionEntry);
                renderProperty(optionSchema, *optionEntry, pass_flags(options));
            }

            oneOfs.data.emplace_back(materialize(std::move(optionSchema)));
        }
        so::Object result{};
        addOneOf(result, std::move(oneOfs));
        s.allOf.data.emplace_back(std::move(result));
    }

    void renderProperty(ObjectSchema& s, const ObjectElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering property ObjectElement as JSON Schema";

        if (hasFixedAttr(e))
            options.set(FIXED_FLAG);

        if (e.empty())
            LOG(warning) << "empty data structure element in backend";
        else
            for (const auto& item : e.get()) {
                assert(item);
                renderProperty(s, *item, inherit_flags(options));
            }
    }

    void renderProperty(ObjectSchema& s, const ExtendElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering property ExtendElement as JSON Schema";

        if (e.empty())
            LOG(warning) << "empty data structure element in backend";

        auto merged = e.get().merge();
        renderProperty(s, *merged, inherit_flags(options));
    }

    void renderProperty(ObjectSchema& s, const IElement& e, TypeAttributes options)
    {
        auto schemaPtr = &s;
        refract::visit(e, [schemaPtr, options](const auto& el) { //
            renderProperty(*schemaPtr, el, options);
        });
    }
} // namespace
