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
#include "ElementIfc.h"
#include "ElementUtils.h"
#include "ElementSize.h"
#include "JsonUtils.h"
#include "JsonValue.h"
#include "Utils.h"
#include <algorithm>
#include <bitset>
#include <cassert>

using namespace refract;
using namespace schema;
using namespace drafter::utils;
using namespace drafter::utils::log;

namespace
{
    using TypeAttributes = std::bitset<4>;
    constexpr std::size_t FIXED_FLAG = 0;
    constexpr std::size_t FIXED_TYPE_FLAG = 1;
    constexpr std::size_t NULLABLE_FLAG = 2;
    constexpr std::size_t REQUIRED_FLAG = 3;

    TypeAttributes updateTypeAttributes(const IElement& e, TypeAttributes options) noexcept
    {
        if (hasFixedTypeAttr(e))
            options.set(FIXED_FLAG);

        if (hasFixedTypeTypeAttr(e))
            options.set(FIXED_TYPE_FLAG);

        if (hasNullableTypeAttr(e))
            options.set(NULLABLE_FLAG);

        if (hasRequiredTypeAttr(e))
            options.set(REQUIRED_FLAG);

        return options;
    }

    TypeAttributes passFlags(TypeAttributes options) noexcept
    {
        return options;
    }

    TypeAttributes inheritFlags(TypeAttributes options) noexcept
    {
        options.reset(FIXED_TYPE_FLAG);
        options.reset(NULLABLE_FLAG);
        options.reset(REQUIRED_FLAG);
        return options;
    }

    TypeAttributes inheritOrPassFlags(TypeAttributes options, const IElement& e)
    {
        auto result = inheritFlags(options);
        if (inheritsFixed(e)) {
            LOG(debug) << "\"" << e.element() << "\"-Element inherits fixed";
            return result;
        }
        return result.reset(FIXED_FLAG);
    }
} // namespace

namespace
{
    std::string key(const MemberElement& m)
    {
        if (const auto& strKey = get<const StringElement>(m.get().key())) {
            if (!strKey->empty()) {
                return strKey->get().get();
            }
        } else {
            LOG(error) << "Non-string key in Member Element: " << m.get().key()->element();
            assert(false);
        }

        return "";
    }

} // namespace

namespace
{ // JSON Schema tools
    so::Object& addSchemaVersion(so::Object& schema)
    {
        schema.data.emplace_back("$schema", so::String{ "http://json-schema.org/draft-07/schema#" });
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

    so::Object& addConst(so::Object& schema, so::Value value)
    {
        schema.data.emplace_back("const", value);
        return schema;
    }

    so::Object& addEnum(so::Object& schema, so::Array value)
    {
        if (value.data.size() == 1) {
            addConst(schema, std::move(value.data.front()));
        } else {
            schema.data.emplace_back("enum", std::move(value));
        }
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
        schema.data.emplace_back("minItems", so::Number{ value });
        return schema;
    }

    so::Object nullSchema()
    {
        return so::Object{ so::from_list{}, std::make_pair("type", so::String{ "null" }) };
    }

    so::Object typeSchema(const char* type)
    {
        return so::Object{ so::from_list{}, std::make_pair("type", so::String{ type }) };
    }

    so::Object& wrapNullable(so::Object& s, TypeAttributes options)
    {
        if (options.test(NULLABLE_FLAG)) {
            addAnyOf(s, so::Array{ so::from_list{}, nullSchema(), so::Object{} });
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
        if (options.test(FIXED_FLAG) || hasFixedTypeAttr(e)) {
            if(e.empty()) {
                return R"(^(?![\s\S]))";
            } else {

                // TODO gcc4.8 support forced us to replace std::regex with the
                //   naive loop-switch implementation that follows
                // std::regex sanitizer{ R"([-[\]{}()*+?.,\^$|#\s])" };
                // const std::string fmt = R"(\$&)";
                // return std::regex_replace(e.get().get(), sanitizer, fmt);
                std::string result{};
                for(char c : e.get().get()) {
                    switch(c) {
                        default:
                            result += c;
                            break;
                        case '^':
                            result += "\\^";
                            break;
                        case '$':
                            result += "\\$";
                            break;
                        case '*':
                            result += "\\*";
                            break;
                        case '+':
                            result += "\\+";
                            break;
                        case '?':
                            result += "\\?";
                            break;
                        case '.':
                            result += "\\.";
                            break;
                        case '(':
                            result += "\\(";
                            break;
                        case ')':
                            result += "\\)";
                            break;
                        case '|':
                            result += "\\|";
                            break;
                        case '{':
                            result += "\\{";
                            break;
                        case '}':
                            result += "\\}";
                            break;
                        case '[':
                            result += "\\[";
                            break;
                        case ']':
                            result += "\\]";
                            break;
                        case '\\':
                            result += "\\\\";
                            break;
                    
                    }
                }
                return result;
            }
        }
        // clang-format on

        return "(?:)";
    }

    struct ObjectSchema {
        so::Object properties = {};
        so::Object patternProperties = {};
        so::Array required = {};
        so::Array allOf = {};
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

    ///
    /// Reduce JSON Schema in-memory; currently only flattens nested `anyOf`s
    ///
    /// @param schema   JSON Schema to be reduced
    ///
    void reduce(so::Object& schema);
} // namespace

namespace
{
    void renderPropertySpecific(ObjectSchema& schema, const ArrayElement& element, TypeAttributes options);
    void renderPropertySpecific(ObjectSchema& schema, const BooleanElement& element, TypeAttributes options);
    void renderPropertySpecific(ObjectSchema& schema, const EnumElement& element, TypeAttributes options);
    void renderPropertySpecific(ObjectSchema& schema, const ExtendElement& element, TypeAttributes options);
    void renderPropertySpecific(ObjectSchema& schema, const HolderElement& element, TypeAttributes options);
    void renderPropertySpecific(ObjectSchema& schema, const MemberElement& element, TypeAttributes options);
    void renderPropertySpecific(ObjectSchema& schema, const NullElement& element, TypeAttributes options);
    void renderPropertySpecific(ObjectSchema& schema, const NumberElement& element, TypeAttributes options);
    void renderPropertySpecific(ObjectSchema& schema, const ObjectElement& element, TypeAttributes options);
    void renderPropertySpecific(ObjectSchema& schema, const OptionElement& element, TypeAttributes options);
    void renderPropertySpecific(ObjectSchema& schema, const RefElement& element, TypeAttributes options);
    void renderPropertySpecific(ObjectSchema& schema, const SelectElement& element, TypeAttributes options);
    void renderPropertySpecific(ObjectSchema& schema, const StringElement& element, TypeAttributes options);
    void renderProperty(ObjectSchema& schema, const IElement& element, TypeAttributes options);

    so::Object& renderSchemaSpecific(so::Object& schema, const ArrayElement& element, TypeAttributes options);
    so::Object& renderSchemaSpecific(so::Object& schema, const BooleanElement& element, TypeAttributes options);
    so::Object& renderSchemaSpecific(so::Object& schema, const EnumElement& element, TypeAttributes options);
    so::Object& renderSchemaSpecific(so::Object& schema, const ExtendElement& element, TypeAttributes options);
    so::Object& renderSchemaSpecific(so::Object& schema, const HolderElement& element, TypeAttributes options);
    so::Object& renderSchemaSpecific(so::Object& schema, const MemberElement& element, TypeAttributes options);
    so::Object& renderSchemaSpecific(so::Object& schema, const NullElement& element, TypeAttributes options);
    so::Object& renderSchemaSpecific(so::Object& schema, const NumberElement& element, TypeAttributes options);
    so::Object& renderSchemaSpecific(so::Object& schema, const ObjectElement& element, TypeAttributes options);
    so::Object& renderSchemaSpecific(so::Object& schema, const OptionElement& element, TypeAttributes options);
    so::Object& renderSchemaSpecific(so::Object& schema, const RefElement& element, TypeAttributes options);
    so::Object& renderSchemaSpecific(so::Object& schema, const SelectElement& element, TypeAttributes options);
    so::Object& renderSchemaSpecific(so::Object& schema, const StringElement& element, TypeAttributes options);
    so::Object& renderSchema(so::Object& schema, const IElement& element, TypeAttributes options);
}

namespace
{
    so::Object makeSchema(const IElement& e, TypeAttributes options)
    {
        so::Object result{};
        renderSchema(result, e, options);
        return result;
    }
} // namespace

namespace
{
    template <typename T>
    so::Object& errorByImpossibleSchema(so::Object& schema, const T& element)
    {
        LOG(error) << "invalid element, interpreting as unsatisfiable schema: " << element.element();
        addOneOf(schema, so::Array{ so::from_list{}, so::Object{}, so::Object{} });
        return schema;
    }

    so::Object& renderSchemaSpecific(so::Object& s, const HolderElement& e, TypeAttributes options)
    {
        if (!e.empty() && e.get().data())
            return renderSchema(s, *e.get().data(), passFlags(options));
        return s;
    }

    so::Object& renderSchemaSpecific(so::Object& s, const RefElement& e, TypeAttributes options)
    {
        if (const IElement* resolved = resolve(e))
            return renderSchema(s, *resolved, passFlags(options));
        LOG(warning) << "ignoring unresolved reference in backend";
        return s;
    }

    so::Object& renderSchemaSpecific(so::Object& s, const ObjectElement& e, TypeAttributes options)
    {
        constexpr const char* TYPE_NAME = "object";

        options = updateTypeAttributes(e, options);
        auto& schema = wrapNullable(s, options);

        addType(schema, TYPE_NAME);

        ObjectSchema result{};
        if (e.empty()) {
            LOG(warning) << "empty data structure element in backend";
        } else {
            for (const auto& item : e.get()) {
                assert(item);
                if (options.test(FIXED_TYPE_FLAG) || options.test(FIXED_FLAG))
                    renderProperty(
                        result, *item, inheritOrPassFlags(options, *item) | TypeAttributes{}.set(REQUIRED_FLAG));
                else
                    renderProperty(result, *item, inheritOrPassFlags(options, *item));
            }
        }

        materialize(schema, std::move(result));

        if (options.test(FIXED_TYPE_FLAG) || options.test(FIXED_FLAG))
            addAdditionalProperties(schema, so::False{});

        return schema;
    }

    so::Object& renderSchemaSpecific(so::Object& s, const ArrayElement& e, TypeAttributes options)
    {
        constexpr const char* TYPE_NAME = "array";

        options = updateTypeAttributes(e, options);

        if (options.test(FIXED_TYPE_FLAG)) { // array of any of types

            so::Array items{};
            if (!e.empty())
                for (const auto& entry : e.get()) {
                    assert(entry);
                    so::emplace_unique(items, makeSchema(*entry, inheritOrPassFlags(options, *entry)));
                }

            auto& schema = wrapNullable(s, options);
            addType(schema, TYPE_NAME);
            addItems(schema, so::Object{ so::from_list{}, std::make_pair("anyOf", std::move(items)) });

        } else if (options.test(FIXED_FLAG)) { // tuple of N constants/types

            so::Array items{};
            if (!e.empty())
                for (const auto& item : e.get()) {
                    assert(item);
                    items.data.emplace_back(makeSchema(*item, inheritOrPassFlags(options, *item)));
                }

            auto& schema = wrapNullable(s, options);
            addType(schema, TYPE_NAME);
            addMinItems(schema, items.data.size());  // minimum of N entries
            addItems(schema, std::move(items));      // schemas of tuple entries
            addAdditionalItems(schema, so::False{}); // no more entries

        } else {
            auto& schema = wrapNullable(s, options);
            addType(schema, TYPE_NAME);
        }

        return s;
    }

    so::Object& renderSchemaSpecific(so::Object& schema, const EnumElement& e, TypeAttributes options)
    {
        options = updateTypeAttributes(e, options);

        so::Array enm{};   // schemas typing single value accumulated in `enum`
        so::Array anyOf{}; // anything other schemas

        if (options.test(NULLABLE_FLAG))
            so::emplace_unique(enm, so::Null{});

        auto enumerationsIt = e.attributes().find("enumerations");
        if (e.attributes().end() != enumerationsIt) {

            const auto enums = get<const ArrayElement>(enumerationsIt->second.get());

            assert(enums);
            if (!enums->empty())
                for (const auto& enumEntry : enums->get()) {
                    assert(enumEntry);
                    if (sizeOf(*enumEntry) == cardinal{ 1 }) // schema types single value
                        so::emplace_unique(enm, generateJsonValue(*enumEntry));
                    else { // schema MAY type more values
                        auto s = makeSchema(*enumEntry, inheritFlags(options));
                        if (s.data.size() == 1) {
                            const auto& key = s.data.at(0).first;
                            auto* vals = mpark::get_if<so::Array>(&s.data.at(0).second);
                            if (key == "enum") {
                                for (auto& val : vals->data)
                                    so::emplace_unique(enm, std::move(val));
                            } else if (key == "anyOf") {
                                for (auto& val : vals->data)
                                    so::emplace_unique(anyOf, std::move(val));
                            } else {
                                so::emplace_unique(anyOf, std::move(s));
                            }
                        } else {
                            so::emplace_unique(anyOf, std::move(s));
                        }
                    }
                }
        } else {
            LOG(warning) << "Enum Element SHALL hold enumerations attribute; interpreting as empty";
        }

        if (anyOf.data.empty()) // use `enum`, all schemas type single values
            addEnum(schema, std::move(enm));

        else { // use `anyOf`, some schemas MAY type multiple values
            // add accumulated single-valued schemas as an option
            if (!enm.data.empty()) {
                so::Object enms;
                addEnum(enms, std::move(enm));
                so::emplace_unique(anyOf, std::move(enms));
            }

            addAnyOf(schema, std::move(anyOf));
        }

        return schema;
    }

    so::Object& renderSchemaSpecific(so::Object& schema, const NullElement& e, TypeAttributes options)
    {
        addType(schema, "null");
        return schema;
    }

    template <typename E>
    struct type_name;

    template <>
    struct type_name<StringElement> {
        constexpr static const char* value = "string";
    };

    template <>
    struct type_name<NumberElement> {
        constexpr static const char* value = "number";
    };

    template <>
    struct type_name<BooleanElement> {
        constexpr static const char* value = "boolean";
    };

    template <typename E>
    so::Object& renderSchemaPrimitive(so::Object& s, const E& e, TypeAttributes options)
    {
        options = updateTypeAttributes(e, options);

        if (options.test(FIXED_FLAG)) {
            if (!e.empty()) {
                if (options.test(NULLABLE_FLAG))
                    addEnum(s, so::Array{ so::from_list{}, so::Null{}, utils::instantiate(e.get()) });
                else
                    addEnum(s, so::Array{ so::from_list{}, utils::instantiate(e.get()) });
                return s;
            }
        }

        if (options.test(NULLABLE_FLAG)) {
            addAnyOf(s, so::Array{ so::from_list{}, nullSchema(), typeSchema(type_name<E>::value) });
            return s;
        }

        addType(s, type_name<E>::value);

        return s;
    }

    so::Object& renderSchemaSpecific(so::Object& s, const MemberElement& e, TypeAttributes options)
    {
        return errorByImpossibleSchema(s, e);
    }

    so::Object& renderSchemaSpecific(so::Object& s, const OptionElement& e, TypeAttributes options)
    {
        return errorByImpossibleSchema(s, e);
    }

    so::Object& renderSchemaSpecific(so::Object& s, const SelectElement& e, TypeAttributes options)
    {
        return errorByImpossibleSchema(s, e);
    }

    so::Object& renderSchemaSpecific(so::Object& s, const StringElement& e, TypeAttributes options)
    {
        return renderSchemaPrimitive(s, e, options);
    }

    so::Object& renderSchemaSpecific(so::Object& s, const NumberElement& e, TypeAttributes options)
    {
        return renderSchemaPrimitive(s, e, options);
    }

    so::Object& renderSchemaSpecific(so::Object& s, const BooleanElement& e, TypeAttributes options)
    {
        return renderSchemaPrimitive(s, e, options);
    }

    so::Object& renderSchemaSpecific(so::Object& s, const ExtendElement& e, TypeAttributes options)
    {
        auto merged = e.get().merge();
        renderSchema(s, *merged, options);
        return s;
    }

    struct RenderSchemaVisitor {
        so::Object* schemaPtr;
        TypeAttributes options;

        template <typename ElementT>
        void operator()(const ElementT& el)
        {
            renderSchemaSpecific(*schemaPtr, el, options);
        }
    };

    so::Object& renderSchema(so::Object& schema, const IElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering `" << e.element() << "` element to JSON Schema";

        refract::visit(e, RenderSchemaVisitor{ &schema, options });
        return schema;
    }
} // namespace

namespace
{
    template <typename T>
    void errorButSkipProperty(const T& element)
    {
        LOG(error) << "skipping invalid property element: " << element.element();
    }

    void renderPropertySpecific(ObjectSchema&, const ArrayElement& element, TypeAttributes)
    {
        errorButSkipProperty(element);
    }

    void renderPropertySpecific(ObjectSchema&, const BooleanElement& element, TypeAttributes)
    {
        errorButSkipProperty(element);
    }

    void renderPropertySpecific(ObjectSchema&, const EnumElement& element, TypeAttributes)
    {
        errorButSkipProperty(element);
    }

    void renderPropertySpecific(ObjectSchema&, const NullElement& element, TypeAttributes)
    {
        errorButSkipProperty(element);
    }

    void renderPropertySpecific(ObjectSchema&, const NumberElement& element, TypeAttributes)
    {
        errorButSkipProperty(element);
    }

    void renderPropertySpecific(ObjectSchema&, const StringElement& element, TypeAttributes)
    {
        errorButSkipProperty(element);
    }

    void renderPropertySpecific(ObjectSchema&, const OptionElement& element, TypeAttributes)
    {
        errorButSkipProperty(element);
    }

    void renderPropertySpecific(ObjectSchema& s, const MemberElement& e, TypeAttributes options)
    {
        if (hasFixedTypeAttr(e))
            options.set(FIXED_FLAG);

        options.set(FIXED_TYPE_FLAG, hasFixedTypeTypeAttr(e));
        options.set(NULLABLE_FLAG, hasNullableTypeAttr(e));

        if (hasRequiredTypeAttr(e))
            options.set(REQUIRED_FLAG);

        if (hasOptionalTypeAttr(e))
            options.reset(REQUIRED_FLAG);

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
                    renderPattern(*strKey, passFlags(options)),
                    makeSchema(*v, passFlags(options)));

            } else if (const auto& strKey = get<const StringElement>(k)) {

                emplace_unique(s.patternProperties, //
                    renderPattern(*strKey, passFlags(options)),
                    makeSchema(*v, passFlags(options)));

            } else {
                LOG(error) << "Unexpected element type in Member Element key: " << k->element();
                assert(false);
            }

        } else {
            auto strKey = key(e);

            s.properties.data.emplace_back(strKey, makeSchema(*v, passFlags(options)));

            if (options.test(REQUIRED_FLAG))
                s.required.data.emplace_back(so::String{ strKey });
        }
    }

    void renderPropertySpecific(ObjectSchema& s, const HolderElement& e, TypeAttributes options)
    {
        if (!e.empty() && e.get().data())
            renderProperty(s, *e.get().data(), passFlags(options));
    }

    void renderPropertySpecific(ObjectSchema& s, const RefElement& e, TypeAttributes options)
    {
        if (const IElement* resolved = resolve(e))
            renderProperty(s, *resolved, passFlags(options));
        LOG(warning) << "ignoring unresolved reference in json schema backend";
    }

    void renderPropertySpecific(ObjectSchema& s, const SelectElement& e, TypeAttributes options)
    {
        so::Array oneOfs{};
        for (const auto& option : e.get()) {

            if (option->empty()) {
                LOG(error) << "empty option element in backend";
                continue;
            }

            if (option->get().size() < 1) {
                LOG(warning) << "option element without children in backend";
                continue;
            }

            ObjectSchema optionSchema{};
            for (const auto& optionEntry : option->get()) {
                assert(optionEntry);
                renderProperty(optionSchema, *optionEntry, passFlags(options));
            }

            oneOfs.data.emplace_back(materialize(std::move(optionSchema)));
        }
        so::Object result{};
        addOneOf(result, std::move(oneOfs));
        s.allOf.data.emplace_back(std::move(result));
    }

    void renderPropertySpecific(ObjectSchema& s, const ObjectElement& e, TypeAttributes options)
    {
        if (hasFixedTypeAttr(e))
            options.set(FIXED_FLAG);

        if (e.empty())
            LOG(warning) << "empty data structure element in backend";
        else
            for (const auto& item : e.get()) {
                assert(item);
                renderProperty(s, *item, inheritFlags(options));
            }
    }

    void renderPropertySpecific(ObjectSchema& s, const ExtendElement& e, TypeAttributes options)
    {
        if (e.empty())
            LOG(warning) << "empty extend element in backend";

        auto merged = e.get().merge();
        renderProperty(s, *merged, passFlags(options));
    }

    struct RenderPropertyVisitor {
        ObjectSchema* schemaPtr;
        TypeAttributes options;

        template <typename ElementT>
        void operator()(const ElementT& el)
        {
            renderPropertySpecific(*schemaPtr, el, options);
        }
    };

    void renderProperty(ObjectSchema& s, const IElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering property `" << e.element() << "` as JSON Schema";

        refract::visit(e, RenderPropertyVisitor{ &s, options });
    }
} // namespace

namespace
{
    so::Array* findAnyOf(so::Object& schema)
    {
        if (so::Value* anys = so::find(schema, "anyOf"))
            return mpark::get_if<so::Array>(anys);
        return nullptr;
    }

    so::Object* flattenAnyOfs(so::Value& schema);

    so::Object* flattenAnyOfsSpecific(so::Object& schema)
    {
        if (so::Array* anyOf = findAnyOf(schema)) {

            so::Array newAnyOf{};
            for (auto& entry : anyOf->data)
                if (so::Object* subAnyOf = flattenAnyOfs(entry)) {
                    auto& subAnyOfArray = mpark::get<so::Array>(subAnyOf->data.back().second);
                    for (auto& subEntry : subAnyOfArray.data)
                        so::emplace_unique(newAnyOf, std::move(subEntry));

                } else {
                    newAnyOf.data.emplace_back(std::move(entry));
                }

            schema.data.back().second = std::move(newAnyOf);
            return &schema;
        }

        for (auto& entry : schema.data)
            flattenAnyOfs(entry.second);

        return nullptr;
    }

    template <typename T>
    so::Object* flattenAnyOfsSpecific(T& schema)
    {
        // do nothing
        return nullptr;
    }

    struct FlattenAnyOfsVisitor {
        template <typename Value>
        so::Object* operator()(const Value& s)
        {
            return flattenAnyOfsSpecific(s);
        }
    };

    so::Object* flattenAnyOfs(so::Value& schema)
    {
        return mpark::visit(FlattenAnyOfsVisitor{}, schema);
    }

    void reduce(so::Object& schema)
    {
        flattenAnyOfsSpecific(schema);
    }
} // namespace

so::Object schema::generateJsonSchema(const IElement& el)
{
    so::Object result{};

    addSchemaVersion(result);
    renderSchema(result, el, TypeAttributes{});

    reduce(result);

    return result;
}
