//
//  refract/JsonSchema.cc
//  librefract
//
//  Created by Thomas Jandecka on 16/02/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "JsonSchema.h"

#include "../ElementData.h"
#include "Utils.h"
#include "VisitorUtils.h"
#include "../utils/log/Trivial.h"
#include "Element.h"
#include <bitset>
#include <cassert>
#include <algorithm>

using namespace refract;
using namespace schema;
using namespace drafter::utils;
using namespace drafter::utils::log;

namespace
{ // API Elements tools
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
            if (const auto* typeAttrs = TypeQueryVisitor::as<const ArrayElement>(typeAttrIt->second.get())) {
                const auto b = typeAttrs->get().begin();
                const auto e = typeAttrs->get().end();
                return e != std::find_if(b, e, [&name](const auto& el) { //
                    const auto* entry = TypeQueryVisitor::as<const StringElement>(el.get());
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

    const dsd::Array& getEnumerations(const EnumElement& e)
    {
        auto resultIt = e.attributes().find("enumerations");
        assert(e.attributes().end() != resultIt);

        const auto enums = TypeQueryVisitor::as<const ArrayElement>(resultIt->second.get());
        assert(enums);
        assert(!enums->empty());

        return enums->get();
    }
} // namespace

namespace
{
    using flags = std::bitset<1>;
    constexpr std::size_t FIXED_FLAG = 0;
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

    template <typename... Args>
    so::Object& addEnum(so::Object& schema, Args&&... args)
    {
        schema.data.emplace_back("enum", so::Array{ so::Value{ std::forward<Args>(args) }... });
        return schema;
    }
} // namespace

namespace
{
    so::Object& addRef(so::Object& schema, const std::string& type)
    {
        schema.data.emplace_back("$ref", so::String{ std::string{ "#/definitions/" } + type });
        return schema;
    }

    so::Object& addAdditionalItems(so::Object& schema, so::Value value)
    {
        schema.data.emplace_back("additionalItems", std::move(value));
        return schema;
    }

    so::Object& addMinItems(so::Object& schema, unsigned value)
    {
        schema.data.emplace_back("minItems", so::Number{ static_cast<double>(value) });
        return schema;
    }

    template <typename T>
    so::Object& chooseTarget(so::Object& s, so::Object& defs, const T& e)
    {
        if (e.element() == T::ValueType::name)
            return s;
        else {
            addRef(s, e.element());
            defs.data.emplace_back(e.element(), so::Object{});
            return get<so::Object>(defs.data.back().second);
        }
    }
} // namespace

namespace
{
    so::Object& renderSchema(so::Object& schema, so::Object& defs, const ObjectElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, so::Object& defs, const ArrayElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, so::Object& defs, const EnumElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, so::Object& defs, const NullElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, so::Object& defs, const StringElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, so::Object& defs, const NumberElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, so::Object& defs, const BooleanElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, so::Object& defs, const MemberElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, so::Object& defs, const ExtendElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, so::Object& defs, const OptionElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, so::Object& defs, const SelectElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, so::Object& defs, const RefElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, so::Object& defs, const HolderElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, so::Object& defs, const IElement& e, flags options = 0);

    so::Object makeSchema(const IElement& e, so::Object& defs, flags options = 0);

    so::Object& renderSchema(so::Object& schema, so::Object& defs, const ObjectElement& e, flags options)
    {
        assert(false); // TODO @tjanc@ not implemented
        return schema;
    }

    so::Object& renderSchema(so::Object& s, so::Object& defs, const ArrayElement& e, flags options)
    {
        constexpr const char* TYPE_NAME = "array";

        if (hasFixedAttr(e))
            options.set(FIXED_FLAG);

        if (hasFixedTypeAttr(e)) { // array of any of types

            if (e.empty() || e.get().empty()) {
                auto& schema = chooseTarget(s, defs, e);
                addType(schema, TYPE_NAME);
                addEnum(schema, so::Array{});

            } else if (e.get().size() == 1) {
                auto item_schema = makeSchema(*e.get().begin()[0], defs, options);
                auto& schema = chooseTarget(s, defs, e);
                addType(schema, TYPE_NAME);
                addItems(schema, std::move(item_schema));

            } else {
                so::Array items{};
                for (const auto& entry : e.get())
                    items.data.emplace_back(makeSchema(*entry, defs, options));
                auto& schema = chooseTarget(s, defs, e);
                addType(schema, TYPE_NAME);
                addItems(schema, so::Object{ std::make_pair("anyOf", std::move(items)) });
            }

        } else if (options.test(FIXED_FLAG)) { // tuple of N constants/types

            if (e.empty() || e.get().empty()) {
                auto& schema = chooseTarget(s, defs, e);
                addType(schema, TYPE_NAME);
                addEnum(schema, so::Array{});

            } else {
                so::Array items{};
                for (const auto& item : e.get())
                    items.data.emplace_back(makeSchema(*item, defs, options));

                auto& schema = chooseTarget(s, defs, e);
                addType(schema, TYPE_NAME);
                addMinItems(schema, items.data.size());  // minimum of N entries
                addItems(schema, std::move(items));      // schemas of tuple entries
                addAdditionalItems(schema, so::False{}); // no more entries
            }
        } else {
            auto& schema = chooseTarget(s, defs, e);
            addType(schema, TYPE_NAME);
        }

        return s;
    }

    so::Object& renderSchema(so::Object& s, so::Object& defs, const EnumElement& e, flags options)
    {
        if (hasFixedAttr(e))
            options.set(FIXED_FLAG);

        so::Array anyOf{};
        for (const auto& enumEntry : getEnumerations(e))
            anyOf.data.emplace_back(makeSchema(*enumEntry, defs, options));

        // write to a definitions entry iff element e is a named type
        auto& schema = chooseTarget(s, defs, e);
        schema.data.emplace_back("anyOf", std::move(anyOf));
        return schema;
    }

    so::Object& renderSchema(so::Object& s, so::Object& defs, const NullElement& e, flags options)
    {
        // write to a definitions entry iff element e is a named type
        auto& schema = chooseTarget(s, defs, e);

        addType(schema, "null");
        return schema;
    }

    so::Object& renderSchema(so::Object& s, so::Object& defs, const StringElement& e, flags options)
    {
        // write to a definitions entry iff element e is a named type
        auto& schema = chooseTarget(s, defs, e);

        addType(schema, "string");

        if (options.test(FIXED_FLAG) || hasFixedAttr(e))
            if (!e.empty())
                addEnum(schema, instantiate(e));

        return schema;
    }

    so::Object& renderSchema(so::Object& s, so::Object& defs, const NumberElement& e, flags options)
    {
        // write to a definitions entry iff element e is a named type
        auto& schema = chooseTarget(s, defs, e);

        addType(schema, "number");

        if (options.test(FIXED_FLAG) || hasFixedAttr(e))
            if (!e.empty())
                addEnum(schema, instantiate(e));

        return schema;
    }

    so::Object& renderSchema(so::Object& s, so::Object& defs, const BooleanElement& e, flags options)
    {
        // write to a definitions entry iff element e is a named type
        auto& schema = chooseTarget(s, defs, e);

        addType(schema, "boolean");

        if (options.test(FIXED_FLAG) || hasFixedAttr(e))
            if (!e.empty())
                addEnum(schema, instantiate(e));

        return schema;
    }

    so::Object& renderSchema(so::Object& s, so::Object& defs, const MemberElement& e, flags options)
    {
        assert(false); // TODO @tjanc@ not implemented
        return s;
    }

    so::Object& renderSchema(so::Object& s, so::Object& defs, const ExtendElement& e, flags options)
    {
        assert(false); // TODO @tjanc@ not implemented
        return s;
    }

    so::Object& renderSchema(so::Object& s, so::Object& defs, const OptionElement& e, flags options)
    {
        assert(false); // TODO @tjanc@ not implemented
        return s;
    }

    so::Object& renderSchema(so::Object& s, so::Object& defs, const SelectElement& e, flags options)
    {
        assert(false); // TODO @tjanc@ not implemented
        return s;
    }

    so::Object& renderSchema(so::Object& s, so::Object& defs, const RefElement& e, flags options)
    {
        assert(false); // TODO @tjanc@ not implemented
        return s;
    }

    so::Object& renderSchema(so::Object& s, so::Object&, const HolderElement&, flags)
    {
        LOG(error) << "HolderElement encountered when generating JSON schema";
        assert(false);
        return s;
    }

    so::Object& renderSchema(so::Object& schema, so::Object& defs, const IElement& e, flags options /* = 0*/)
    {
        auto schemaPtr = &schema;
        auto defsPtr = &defs;
        refract::visit(e, [schemaPtr, defsPtr, options](const auto& el) { //
            renderSchema(*schemaPtr, *defsPtr, el, options);
        });
        return schema;
    }

    so::Object makeSchema(const IElement& e, so::Object& defs, flags options /* = 0*/)
    {
        so::Object result{};
        renderSchema(result, defs, e, options);
        return result;
    }
} // namespace

so::Object schema::generateJsonSchema(const IElement& el)
{
    so::Object result{};
    addSchemaVersion(result);

    so::Object defs{};
    renderSchema(result, defs, el);

    if (defs.data.size() > 0)
        result.data.insert(result.data.begin() + 1, std::make_pair("definitions", std::move(defs)));

    return result;
}
