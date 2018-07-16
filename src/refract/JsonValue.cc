//
//  refract/JsonValue.cc
//  librefract
//
//  Created by Thomas Jandecka on 12/07/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "JsonValue.h"

#include "../ElementData.h"
#include "../utils/log/Trivial.h"
#include "../utils/so/JsonIo.h"
#include "Element.h"
#include "ElementUtils.h"
#include "Utils.h"
#include <algorithm>
#include <bitset>
#include <cassert>
#include <regex>

using namespace refract;
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

    TypeAttributes pass_flags(TypeAttributes options) noexcept
    {
        return options;
    }

    TypeAttributes inherit_flags(TypeAttributes options) noexcept
    {
        options.reset(FIXED_TYPE_FLAG);
        options.reset(NULLABLE_FLAG);
        options.reset(REQUIRED_FLAG);
        return options;
    }

    TypeAttributes inherit_or_pass_flags(TypeAttributes options, const IElement& e)
    {
        auto result = inherit_flags(options);
        if (inheritsFixed(e)) {
            LOG(debug) << "\"" << e.element() << "\"-Element inherits fixed";
            return result;
        }
        return result.reset(FIXED_FLAG);
    }
} // namespace

namespace
{
    void renderProperty(so::Object& obj, const MemberElement& e, TypeAttributes options);
    void renderProperty(so::Object& obj, const RefElement& e, TypeAttributes options);
    void renderProperty(so::Object& obj, const SelectElement& e, TypeAttributes options);
    void renderProperty(so::Object& obj, const ObjectElement& e, TypeAttributes options);
    void renderProperty(so::Object& obj, const ExtendElement& e, TypeAttributes options);
    void renderProperty(so::Object& obj, const IElement& e, TypeAttributes options);

    void renderItem(so::Array& obj, const RefElement& e, TypeAttributes options);
    void renderItem(so::Array& obj, const IElement& e, TypeAttributes options);

    so::Value renderValue(const ObjectElement& e, TypeAttributes options);
    so::Array renderValue(const ArrayElement& e, TypeAttributes options);
    so::Value renderValue(const EnumElement& e, TypeAttributes options);
    so::Value renderValue(const NullElement& e, TypeAttributes options);
    so::Value renderValue(const StringElement& e, TypeAttributes options);
    so::Value renderValue(const NumberElement& e, TypeAttributes options);
    so::Value renderValue(const BooleanElement& e, TypeAttributes options);
    so::Value renderValue(const ExtendElement& e, TypeAttributes options);
    so::Value renderValue(const IElement& e, TypeAttributes options);

    template <typename T>
    void renderProperty(so::Object&, const T& e, TypeAttributes)
    {
        LOG(error) << "invalid property element: " << e.element();
        assert(false);
    }

    template <typename T>
    void renderItem(so::Array& a, const T& e, TypeAttributes options)
    {
        LOG(debug) << "rendering item " << e.element() << "Element as JSON Value";
        a.data.emplace_back(renderValue(e, inherit_or_pass_flags(options, e)));
    }

    template <typename T>
    so::Value renderValue(const T& e, TypeAttributes)
    {
        LOG(error) << "invalid top level element: " << e.element();
        assert(false);
        return so::Null{}; // unreachable
    }

    so::Value renderValue(const IElement& e, TypeAttributes options)
    {
        return refract::visit(e, [options](const auto& el) { //
            return renderValue(el, options);
        });
    }
} // namespace

so::Value refract::generateJsonValue(const IElement& el)
{
    return renderValue(el, TypeAttributes{});
}

namespace
{

    so::Value renderValue(const ObjectElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering ObjectElement to JSON Value";

        options = updateTypeAttributes(e, options);

        so::Object result{};
        if (!e.empty())
            for (const auto& item : e.get()) {
                assert(item);
                if (options.test(FIXED_TYPE_FLAG) || options.test(FIXED_FLAG))
                    renderProperty(
                        result, *item, inherit_or_pass_flags(options, *item) | TypeAttributes{}.set(REQUIRED_FLAG));
                else
                    renderProperty(result, *item, inherit_or_pass_flags(options, *item));
            }

        // TODO handle nullable?

        return result;
    }

    const IElement& resolve(const RefElement& e)
    {
        const auto& resolvedEntry = e.attributes().find("resolved");
        if (resolvedEntry == e.attributes().end()) {
            LOG(error) << "expected all references to be resolved in backend";
            assert(false);
        }

        assert(resolvedEntry->second);
        return *resolvedEntry->second;
    }

    so::Array renderValue(const ArrayElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering ArrayElement to JSON Value";

        options = updateTypeAttributes(e, options);

        so::Array result{};
        if (!e.empty())
            for (const auto& entry : e.get()) {
                assert(entry);
                renderItem(result, *entry, inherit_or_pass_flags(options, *entry));
                if (options.test(FIXED_TYPE_FLAG))
                    break;
            }

        return result;
    }

    so::Value renderValue(const EnumElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering EnumElement to JSON Value";

        options = updateTypeAttributes(e, options);

        if (e.empty()) {
            auto enumerationsIt = e.attributes().find("enumerations");
            if (e.attributes().end() != enumerationsIt) {
                const auto enums = get<const ArrayElement>(enumerationsIt->second.get());
                assert(enums);
                if (!enums->empty())
                    for (const auto& enumEntry : enums->get()) {
                        assert(enumEntry);
                        return renderValue(*enumEntry, inherit_flags(options));
                    }
            }

            if (options.test(NULLABLE_FLAG)) {
                return so::Null{};
            }

            LOG(warning) << "no value found for EnumElement; using `null`";
            return so::Null{};
        }

        assert(e.get().value());
        return renderValue(*e.get().value(), inherit_flags(options));
    } // namespace

    so::Value renderValue(const NullElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering NullElement to JSON Value";

        return so::Null{};
    }

    so::Value renderValue(const StringElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering StringElement to JSON Value";

        options = updateTypeAttributes(e, options);

        if (e.empty()) {
            if (options.test(NULLABLE_FLAG))
                return so::Null{};

            if (const auto& sampleValue = findSample(e)) {
                return renderValue(*sampleValue, options);
            }

            if (const auto& defaultValue = findDefault(e)) {
                return renderValue(*defaultValue, options);
            }

            LOG(warning) << "no value for non-nullable empty StringElement; using `\"\"`";
            return so::String{};
        }

        return so::String{ e.get().get() };
    }

    so::Value renderValue(const NumberElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering NumberElement to JSON Value";

        options = updateTypeAttributes(e, options);

        if (e.empty()) {
            if (options.test(NULLABLE_FLAG))
                return so::Null{};

            if (const auto& sampleValue = findSample(e)) {
                return renderValue(*sampleValue, options);
            }

            if (const auto& defaultValue = findDefault(e)) {
                return renderValue(*defaultValue, options);
            }

            LOG(warning) << "no value for non-nullable empty NumberElement; using `0`";
            return so::Number{};
        }

        return so::Number{ e.get().get() };
    }

    so::Value renderValue(const BooleanElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering BooleanElement to JSON Value";

        options = updateTypeAttributes(e, options);

        if (e.empty()) {
            if (options.test(NULLABLE_FLAG))
                return so::Null{};

            if (const auto& sampleValue = findSample(e)) {
                return renderValue(*sampleValue, options);
            }

            if (const auto& defaultValue = findDefault(e)) {
                return renderValue(*defaultValue, options);
            }

            LOG(warning) << "no value for non-nullable empty NumberElement; using `false`";
            return so::False{};
        }

        if (e.get().get())
            return so::True{};
        return so::False{};
    }

    so::Value renderValue(const ExtendElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering ExtendElement to JSON Value";

        auto merged = e.get().merge();
        assert(merged);
        return renderValue(*merged, options);
    }

} // namespace

namespace
{

    void renderProperty(so::Object& obj, const MemberElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering property MemberElement as JSON Value";

        if (hasFixedTypeAttr(e))
            options.set(FIXED_FLAG);

        options.set(FIXED_TYPE_FLAG, hasFixedTypeTypeAttr(e));
        options.set(NULLABLE_FLAG, hasNullableTypeAttr(e));

        if (hasRequiredTypeAttr(e))
            options.set(REQUIRED_FLAG);

        if (hasOptionalTypeAttr(e))
            options.reset(REQUIRED_FLAG);

        const auto* k = e.get().key();
        const auto* v = e.get().value();

        assert(k);

        if (hasOptionalTypeAttr(e))
            if (!definesValue(*v)) {
                LOG(debug) << "omitting optional property while rendering value";
                return;
            }

        if (isVariable(e)) {
            if (const auto* strKeyEl = get<const StringElement>(k)) {
                if (!strKeyEl->empty()) {
                    LOG(warning) << "improvised variable key; sample shall be used as key sample";
                    const auto& strKey = strKeyEl->get().get();
                    emplace_unique(obj, strKey, renderValue(*v, pass_flags(options)));
                    return;
                }

            } else if (const auto* extend = get<const ExtendElement>(k)) {
                if (!extend->empty()) {
                    auto merged = extend->get().merge();
                    if (const auto* strKeyEl = get<const StringElement>(merged.get())) {
                        if (!strKeyEl->empty()) {
                            LOG(warning) << "improvised variable key; sample shall be used as key sample";
                            const auto& strKey = strKeyEl->get().get();
                            emplace_unique(obj, strKey, renderValue(*v, pass_flags(options)));
                            return;
                        }
                    } else {
                        LOG(warning) << "variable property key does not resolve to String Element";
                        assert(false);
                    }
                }
            }

            LOG(info) << "omitting variable property while rendering value";

        } else {
            auto strKey = key(e);
            emplace_unique(obj, strKey, renderValue(*v, pass_flags(options)));
        }
    }

    void renderProperty(so::Object& obj, const RefElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering property RefElement as JSON Value";

        const auto& resolvedEntry = e.attributes().find("resolved");
        if (resolvedEntry == e.attributes().end()) {
            LOG(error) << "expected all references to be resolved in backend";
            assert(false);
        }

        assert(resolvedEntry->second);
        renderProperty(obj, *resolvedEntry->second, pass_flags(options));
    }

    void renderProperty(so::Object& s, const SelectElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering property SelectElement as JSON Value";

        so::Array oneOfs{};
        for (const auto& option : e.get()) {

            if (option->empty()) {
                LOG(error) << "unexpected empty option element in backend";
                assert(false);
            }

            if (option->get().size() < 1) {
                LOG(warning) << "empty option element in backend; skipping it";
                continue;
            }

            for (const auto& optionEntry : option->get()) {
                assert(optionEntry);
                renderProperty(s, *optionEntry, pass_flags(options));
            }

            return;
        }

        LOG(warning) << "no non-empty OptionElement in SelectElement; skipping property";
    }

    void renderProperty(so::Object& s, const ObjectElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering property ObjectElement as JSON Value";

        if (hasFixedTypeAttr(e))
            options.set(FIXED_FLAG);

        if (!e.empty())
            for (const auto& item : e.get()) {
                assert(item);
                renderProperty(s, *item, inherit_flags(options));
            }
    }

    void renderProperty(so::Object& s, const ExtendElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering property ExtendElement as JSON Value";

        if (e.empty())
            LOG(warning) << "empty data structure element in backend";

        auto merged = e.get().merge();
        assert(merged);
        renderProperty(s, *merged, pass_flags(options));
    }

    void renderProperty(so::Object& s, const IElement& e, TypeAttributes options)
    {
        auto objPtr = &s;
        refract::visit(e, [objPtr, options](const auto& el) { //
            renderProperty(*objPtr, el, options);
        });
    }

    void renderItem(so::Array& a, const RefElement& e, TypeAttributes options)
    {
        LOG(debug) << "rendering item RefElement as JSON Value";

        const auto& resolved = resolve(e);
        if (const auto& mixin = get<const ArrayElement>(&resolved)) {
            if (!mixin->empty())
                for (const auto& item : mixin->get()) {
                    assert(item);
                    renderItem(a, *item, inherit_or_pass_flags(options, *item));
                }
        }
    }

    void renderItem(so::Array& a, const IElement& e, TypeAttributes options)
    {
        auto aPtr = &a;
        refract::visit(e, [aPtr, options](const auto& el) { //
            renderItem(*aPtr, el, options);
        });
    }
} // namespace
