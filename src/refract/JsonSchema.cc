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
#include "VisitorUtils.h"
#include "../utils/log/Trivial.h"

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

    bool hasFixedAttr(const IElement& e)
    {
        return e.attributes().end() != e.attributes().find("fixed");
    }

    bool hasFixedTypeAttr(const IElement& e)
    {
        return e.attributes().end() != e.attributes().find("fixedType");
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
    so::Object& renderSchema(so::Object& schema, const ObjectElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, const ArrayElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, const EnumElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, const NullElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, const StringElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, const NumberElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, const BooleanElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, const MemberElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, const ExtendElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, const OptionElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, const SelectElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, const RefElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, const HolderElement& e, flags options);
    so::Object& renderSchema(so::Object& schema, const IElement& e, flags options = 0);

    so::Object makeSchema(const IElement& e, flags options = 0);

    so::Object& renderSchema(so::Object& schema, const ObjectElement& e, flags options)
    {
        assert(false); // TODO @tjanc@ not implemented
        return schema;
    }

    so::Object& renderSchema(so::Object& schema, const ArrayElement& e, flags options)
    {
        addType(schema, "array");

        // UPDATE OPTIONS
        if (hasFixedAttr(e))
            options.set(FIXED_FLAG);

        if (hasFixedTypeAttr(e)) { // array of any of types

            if (e.empty() || e.get().empty())
                addEnum(schema, so::Array{});

            else if (e.get().size() == 1)
                addItems(schema, makeSchema(*e.get().begin()[0], options));

            else {
                so::Array items{};
                for (const auto& entry : e.get())
                    items.data.emplace_back(makeSchema(*entry, options));
                addItems(schema, so::Object{ std::make_pair("anyOf", std::move(items)) });
            }

        } else if (options.test(FIXED_FLAG)) { // tuple of constants/types

            if (e.empty() || e.get().empty())
                addEnum(schema, so::Array{});
            else {
                so::Array items{};
                for (const auto& item : e.get())
                    items.data.emplace_back(makeSchema(*item, options));
                addItems(schema, std::move(items));
            }
        }

        return schema;
    }

    so::Object& renderSchema(so::Object& schema, const EnumElement& e, flags options)
    {
        if (hasFixedAttr(e))
            options.set(FIXED_FLAG);

        so::Array anyOf{};
        for (const auto& enumEntry : getEnumerations(e))
            anyOf.data.emplace_back(makeSchema(*enumEntry, options));

        schema.data.emplace_back("anyOf", std::move(anyOf));

        return schema;
    }

    so::Object& renderSchema(so::Object& schema, const NullElement& e, flags options)
    {
        addType(schema, "null");
        return schema;
    }

    so::Object& renderSchema(so::Object& schema, const StringElement& e, flags options)
    {
        addType(schema, "string");

        if (options.test(FIXED_FLAG) || hasFixedAttr(e))
            if (!e.empty())
                addEnum(schema, instantiate(e));

        return schema;
    }

    so::Object& renderSchema(so::Object& schema, const NumberElement& e, flags options)
    {
        addType(schema, "number");

        if (options.test(FIXED_FLAG) || hasFixedAttr(e))
            if (!e.empty())
                addEnum(schema, instantiate(e));

        return schema;
    }

    so::Object& renderSchema(so::Object& schema, const BooleanElement& e, flags options)
    {
        addType(schema, "boolean");

        if (options.test(FIXED_FLAG) || hasFixedAttr(e))
            if (!e.empty())
                addEnum(schema, instantiate(e));

        return schema;
    }

    so::Object& renderSchema(so::Object& schema, const MemberElement& e, flags options)
    {
        assert(false); // TODO @tjanc@ not implemented
        return schema;
    }

    so::Object& renderSchema(so::Object& schema, const ExtendElement& e, flags options)
    {
        assert(false); // TODO @tjanc@ not implemented
        return schema;
    }

    so::Object& renderSchema(so::Object& schema, const OptionElement& e, flags options)
    {
        assert(false); // TODO @tjanc@ not implemented
        return schema;
    }

    so::Object& renderSchema(so::Object& schema, const SelectElement& e, flags options)
    {
        assert(false); // TODO @tjanc@ not implemented
        return schema;
    }

    so::Object& renderSchema(so::Object& schema, const RefElement& e, flags options)
    {
        assert(false); // TODO @tjanc@ not implemented
        return schema;
    }

    so::Object& renderSchema(so::Object& schema, const HolderElement& e, flags options)
    {
        LOG(error) << "HolderElement encountered when generating JSON schema";
        assert(false);
        return schema;
    }

    so::Object& renderSchema(so::Object& schema, const IElement& e, flags options /* = 0*/)
    {
        auto schemaPtr = &schema;
        refract::visit(e, [schemaPtr, options](const auto& el) { //
            renderSchema(*schemaPtr, el, options);
        });
        return schema;
    }

    so::Object makeSchema(const IElement& e, flags options /* = 0*/)
    {
        so::Object result{};
        renderSchema(result, e, options);
        return result;
    }
} // namespace

Schema schema::generateJsonSchema(const IElement& el)
{
    so::Object result{};
    addSchemaVersion(result);
    renderSchema(result, el);

    return Schema{ std::move(result) };
}
