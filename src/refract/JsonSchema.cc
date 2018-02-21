//
//  refract/JsonSchema.cc
//  librefract
//
//  Created by Thomas Jandecka on 16/02/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "JsonSchema.h"

#include <cassert>
#include <bitset>
#include "Element.h"
#include "../ElementData.h"
#include "Utils.h"
#include "../utils/log/Trivial.h"

#include "VisitorUtils.h" // GetValue

using namespace refract;
using namespace schema;
using namespace drafter::utils;
using namespace drafter::utils::log;

namespace
{ // API Elements tools
    so::String instanciate(const StringElement& e)
    {
        assert(!e.empty());
        return so::String{ e.get().get() };
    }

    so::Number instanciate(const NumberElement& e)
    {
        assert(!e.empty());
        return so::Number{ e.get().get() };
    }

    so::Value instanciate(const BooleanElement& e)
    {
        assert(!e.empty());
        return e.get() ? //
            so::Value{ in_place_type<so::True>{} } :
            so::Value{ in_place_type<so::False>{} };
    }

    bool has_fixed_attr(const IElement& e)
    {
        return e.attributes().end() != e.attributes().find("fixed");
    }

    bool has_fixed_type_attr(const IElement& e)
    {
        return e.attributes().end() != e.attributes().find("fixedType");
    }

    template <typename T>
    const T* get_if_default_attr(const T& e)
    {
        auto it = e.attributes().find("default");
        if (e.attributes().end() != it)
            return TypeQueryVisitor::as<const T>(it->second.get());
        return nullptr;
    }

    const dsd::Array& get_enumerations(const EnumElement& e)
    {
        auto resultIt = e.attributes().find("enumerations");
        assert(e.attributes().end() != resultIt);

        const auto enums = TypeQueryVisitor::as<const ArrayElement>(resultIt->second.get());
        assert(enums);
        assert(!enums->empty());

        return enums->get();
    }
}

namespace
{
    using flags = std::bitset<1>;
    constexpr std::size_t fixed_flag = 0;
}

namespace
{ // JSON Schema tools
    so::Object& add_schema_version(so::Object& schema)
    {
        schema.data.emplace_back("$schema", so::String{ "http://json-schema.org/draft-04/schema#" });
        return schema;
    }

    so::Object& add_type(so::Object& schema, const char* type)
    {
        schema.data.emplace_back("type", so::String{ type });
        return schema;
    }

    so::Object& add_items(so::Object& schema, so::Value value)
    {
        schema.data.emplace_back("items", std::move(value));
        return schema;
    }

    template <typename... Args>
    so::Object& add_enum(so::Object& schema, Args&&... args)
    {
        schema.data.emplace_back("enum", so::Array{ so::Value{ std::forward<Args>(args) }... });
        return schema;
    }
}

namespace
{
    so::Object& render_schema(so::Object& schema, const ObjectElement& e, flags options);
    so::Object& render_schema(so::Object& schema, const ArrayElement& e, flags options);
    so::Object& render_schema(so::Object& schema, const EnumElement& e, flags options);
    so::Object& render_schema(so::Object& schema, const NullElement& e, flags options);
    so::Object& render_schema(so::Object& schema, const StringElement& e, flags options);
    so::Object& render_schema(so::Object& schema, const NumberElement& e, flags options);
    so::Object& render_schema(so::Object& schema, const BooleanElement& e, flags options);
    so::Object& render_schema(so::Object& schema, const MemberElement& e, flags options);
    so::Object& render_schema(so::Object& schema, const ExtendElement& e, flags options);
    so::Object& render_schema(so::Object& schema, const OptionElement& e, flags options);
    so::Object& render_schema(so::Object& schema, const SelectElement& e, flags options);
    so::Object& render_schema(so::Object& schema, const RefElement& e, flags options);
    so::Object& render_schema(so::Object& schema, const HolderElement& e, flags options);
    so::Object& render_schema(so::Object& schema, const IElement& e, flags options = 0);

    so::Object make_schema(const IElement& e, flags options = 0);

    so::Object& render_schema(so::Object& schema, const ObjectElement& e, flags options)
    {
        assert(false); // TODO @tjanc@ not implemented
        return schema;
    }

    so::Object& render_schema(so::Object& schema, const ArrayElement& e, flags options)
    {
        add_type(schema, "array");

        // UPDATE OPTIONS
        if (has_fixed_attr(e))
            options.set(fixed_flag);

        if (has_fixed_type_attr(e)) { // array of any of types

            if (e.empty() || e.get().empty())
                add_enum(schema, so::Array{});

            else if (e.get().size() == 1)
                add_items(schema, make_schema(*e.get().begin()[0], options));

            else {
                so::Array items{};
                for (const auto& entry : e.get())
                    items.data.emplace_back(make_schema(*entry, options));
                add_items(schema, so::Object{ std::make_pair("anyOf", std::move(items)) });
            }

        } else if (options.test(fixed_flag)) { // tuple of constants/types

            if (e.empty() || e.get().empty())
                add_enum(schema, so::Array{});
            else {
                so::Array items{};
                for (const auto& item : e.get())
                    items.data.emplace_back(make_schema(*item, options));
                add_items(schema, std::move(items));
            }
        }

        return schema;
    }

    so::Object& render_schema(so::Object& schema, const EnumElement& e, flags options)
    {
        if (has_fixed_attr(e))
            options.set(fixed_flag);

        so::Array anyOf{};
        for (const auto& enumEntry : get_enumerations(e))
            anyOf.data.emplace_back(make_schema(*enumEntry, options));

        schema.data.emplace_back("anyOf", std::move(anyOf));

        return schema;
    }

    so::Object& render_schema(so::Object& schema, const NullElement& e, flags options)
    {
        add_type(schema, "null");
        return schema;
    }

    so::Object& render_schema(so::Object& schema, const StringElement& e, flags options)
    {
        add_type(schema, "string");

        if (options.test(fixed_flag) || has_fixed_attr(e))
            if (!e.empty())
                add_enum(schema, instanciate(e));

        return schema;
    }

    so::Object& render_schema(so::Object& schema, const NumberElement& e, flags options)
    {
        add_type(schema, "number");

        if (options.test(fixed_flag) || has_fixed_attr(e))
            if (!e.empty())
                add_enum(schema, instanciate(e));

        return schema;
    }

    so::Object& render_schema(so::Object& schema, const BooleanElement& e, flags options)
    {
        add_type(schema, "boolean");

        if (options.test(fixed_flag) || has_fixed_attr(e))
            if (!e.empty())
                add_enum(schema, instanciate(e));

        return schema;
    }

    so::Object& render_schema(so::Object& schema, const MemberElement& e, flags options)
    {
        assert(false); // TODO @tjanc@ not implemented
        return schema;
    }

    so::Object& render_schema(so::Object& schema, const ExtendElement& e, flags options)
    {
        assert(false); // TODO @tjanc@ not implemented
        return schema;
    }

    so::Object& render_schema(so::Object& schema, const OptionElement& e, flags options)
    {
        assert(false); // TODO @tjanc@ not implemented
        return schema;
    }

    so::Object& render_schema(so::Object& schema, const SelectElement& e, flags options)
    {
        assert(false); // TODO @tjanc@ not implemented
        return schema;
    }

    so::Object& render_schema(so::Object& schema, const RefElement& e, flags options)
    {
        assert(false); // TODO @tjanc@ not implemented
        return schema;
    }

    so::Object& render_schema(so::Object& schema, const HolderElement& e, flags options)
    {
        LOG(error) << "HolderElement encountered when generating JSON schema";
        assert(false);
        return schema;
    }

    so::Object& render_schema(so::Object& schema, const IElement& e, flags options /* = 0*/)
    {
        auto schemaPtr = &schema;
        refract::visit(e, [schemaPtr, options](const auto& el) { //
            render_schema(*schemaPtr, el, options);
        });
        return schema;
    }

    so::Object make_schema(const IElement& e, flags options /* = 0*/)
    {
        so::Object result{};
        render_schema(result, e, options);
        return result;
    }
}

Schema schema::generateJsonSchema(const IElement& el)
{
    so::Object result{};
    add_schema_version(result);
    render_schema(result, el);

    return Schema{ std::move(result) };
}
