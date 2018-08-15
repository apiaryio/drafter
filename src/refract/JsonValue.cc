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

    TypeAttributes updateTypeAttributes(const IElement& element, TypeAttributes options) noexcept
    {
        if (hasFixedTypeAttr(element))
            options.set(FIXED_FLAG);

        if (hasFixedTypeTypeAttr(element))
            options.set(FIXED_TYPE_FLAG);

        if (hasNullableTypeAttr(element))
            options.set(NULLABLE_FLAG);

        if (hasRequiredTypeAttr(element))
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

    TypeAttributes inheritOrPassFlags(TypeAttributes options, const IElement& element)
    {
        auto result = inheritFlags(options);
        if (inheritsFixed(element)) {
            LOG(debug) << "\"" << element.element() << "\"-Element inherits fixed";
            return result;
        }
        return result.reset(FIXED_FLAG);
    }
} // namespace

namespace
{
    void renderProperty(so::Object& obj, const MemberElement& element, TypeAttributes options);
    void renderProperty(so::Object& obj, const RefElement& element, TypeAttributes options);
    void renderProperty(so::Object& obj, const SelectElement& element, TypeAttributes options);
    void renderProperty(so::Object& obj, const ObjectElement& element, TypeAttributes options);
    void renderProperty(so::Object& obj, const ExtendElement& element, TypeAttributes options);
    void renderProperty(so::Object& obj, const IElement& element, TypeAttributes options);

    void renderItemSpecific(so::Array& obj, const RefElement& element, TypeAttributes options);
    void renderItemSpecific(so::Array& obj, const NumberElement& element, TypeAttributes options);
    void renderItemSpecific(so::Array& obj, const StringElement& element, TypeAttributes options);
    void renderItemSpecific(so::Array& obj, const BooleanElement& element, TypeAttributes options);
    void renderItem(so::Array& obj, const IElement& element, TypeAttributes options);

    so::Value renderValueSpecific(const ObjectElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const ArrayElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const EnumElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const NullElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const StringElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const NumberElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const BooleanElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const ExtendElement& element, TypeAttributes options);
    so::Value renderValue(const IElement& element, TypeAttributes options);

    template <typename T>
    void renderProperty(so::Object&, const T& element, TypeAttributes)
    {
        LOG(error) << "invalid property element: " << element.element();
        assert(false);
    }

    template <typename T>
    void renderItemSpecific(so::Array& a, const T& element, TypeAttributes options)
    {
        LOG(debug) << "rendering item " << element.element() << "Element as JSON Value";
        a.data.emplace_back(renderValue(element, inheritOrPassFlags(options, element)));
    }

    template <typename T>
    so::Value renderValueSpecific(const T& element, TypeAttributes)
    {
        LOG(error) << "invalid top level element: " << element.element();
        assert(false);
        return so::Null{}; // unreachable
    }

    so::Value renderValue(const IElement& element, TypeAttributes options)
    {
        return refract::visit(element, [options](const auto& el) { //
            return renderValueSpecific(el, options);
        });
    }
} // namespace

so::Value refract::generateJsonValue(const IElement& el)
{
    return renderValue(el, TypeAttributes{});
}

namespace
{
    ///
    /// Searches for a sample or default, in that order, and renders it into
    ///     the given simple object value
    ///
    /// @tparam SoType  type of simple object to be rendered to
    /// @tparam Element type of Element sample or default shall be searched on
    ///
    /// @param element        element sample/default to be searched on
    /// @param options  type attributes to be inherited
    ///
    /// @return         whether a sample or default was found and rendered
    ///
    template <typename Element>
    std::pair<bool, so::Value> renderSampleOrDefault(const Element& element, TypeAttributes options)
    {
        if (const auto& sampleValue = findFirstSample(element)) {
            return { true, renderValueSpecific(*sampleValue, options) };
        }

        if (const auto& defaultValue = findDefault(element)) {
            return { true, renderValueSpecific(*defaultValue, options) };
        }

        return { false, so::Null{} };
    }

    const IElement& resolve(const RefElement& element)
    {
        const auto& resolvedEntry = element.attributes().find("resolved");
        if (resolvedEntry == element.attributes().end()) {
            LOG(error) << "expected all references to be resolved in backend";
            assert(false);
        }

        assert(resolvedEntry->second);
        return *resolvedEntry->second;
    }

} // namespace

namespace
{

    so::Value renderValueSpecific(const ObjectElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering ObjectElement to JSON Value";
        so::Object result{};

        options = updateTypeAttributes(element, options);

        if (element.empty()) {
            auto alt = renderSampleOrDefault(element, inheritFlags(options));
            if (alt.first)
                return std::move(alt.second);
        } else
            for (const auto& item : element.get()) {
                assert(item);
                if (options.test(FIXED_TYPE_FLAG) || options.test(FIXED_FLAG))
                    renderProperty(
                        result, *item, inheritOrPassFlags(options, *item) | TypeAttributes{}.set(REQUIRED_FLAG));
                else
                    renderProperty(result, *item, inheritOrPassFlags(options, *item));
            }

        return result;
    }

    so::Value renderValueSpecific(const ArrayElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering ArrayElement to JSON Value";

        options = updateTypeAttributes(element, options);

        so::Array result{};
        if (element.empty()) {
            auto alt = renderSampleOrDefault(element, inheritFlags(options));
            if (alt.first)
                return std::move(alt.second);
        } else
            for (const auto& entry : element.get()) {
                assert(entry);
                renderItem(result, *entry, inheritOrPassFlags(options, *entry));
                if (options.test(FIXED_TYPE_FLAG))
                    break;
            }

        return so::Value{ result };
    }

    so::Value renderValueSpecific(const EnumElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering EnumElement to JSON Value";

        options = updateTypeAttributes(element, options);

        if (element.empty()) {
            auto alt = renderSampleOrDefault(element, inheritFlags(options));
            if (alt.first)
                return std::move(alt.second);

            auto enumerationsIt = element.attributes().find("enumerations");
            if (element.attributes().end() != enumerationsIt) {
                const auto enums = get<const ArrayElement>(enumerationsIt->second.get());
                assert(enums);
                if (!enums->empty())
                    for (const auto& enumEntry : enums->get()) {
                        assert(enumEntry);
                        return renderValue(*enumEntry, inheritFlags(options));
                    }
            }

            if (options.test(NULLABLE_FLAG)) {
                return so::Null{};
            }

            LOG(warning) << "no value found for EnumElement; using `null`";
            return so::Null{};
        }

        assert(element.get().value());
        return renderValue(*element.get().value(), inheritFlags(options));
    } // namespace

    so::Value renderValueSpecific(const NullElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering NullElement to JSON Value";

        return so::Null{};
    }

    so::Value renderValueSpecific(const StringElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering StringElement to JSON Value";

        options = updateTypeAttributes(element, options);

        if (element.empty()) {
            if (options.test(NULLABLE_FLAG))
                return so::Null{};

            auto alt = renderSampleOrDefault(element, passFlags(options));
            if (alt.first)
                return std::move(alt.second);

            LOG(warning) << "no value for non-nullable empty StringElement; using `\"\"`";
            return so::String{};
        }

        return so::String{ element.get().get() };
    }

    so::Value renderValueSpecific(const NumberElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering NumberElement to JSON Value";

        options = updateTypeAttributes(element, options);

        if (element.empty()) {
            if (options.test(NULLABLE_FLAG))
                return so::Null{};

            auto alt = renderSampleOrDefault(element, passFlags(options));
            if (alt.first)
                return std::move(alt.second);

            LOG(warning) << "no value for non-nullable empty NumberElement; using `0`";
            return so::Number{};
        }

        return so::Number{ element.get().get() };
    }

    so::Value renderValueSpecific(const BooleanElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering BooleanElement to JSON Value";

        options = updateTypeAttributes(element, options);

        if (element.empty()) {
            if (options.test(NULLABLE_FLAG))
                return so::Null{};

            auto alt = renderSampleOrDefault(element, passFlags(options));
            if (alt.first)
                return std::move(alt.second);

            LOG(warning) << "no value for non-nullable empty NumberElement; using `false`";
            return so::False{};
        }

        if (element.get().get())
            return so::True{};
        return so::False{};
    }

    so::Value renderValueSpecific(const ExtendElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering ExtendElement to JSON Value";

        auto merged = element.get().merge();
        assert(merged);
        return renderValue(*merged, options);
    }

} // namespace

namespace
{

    void renderProperty(so::Object& obj, const MemberElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering property MemberElement as JSON Value";

        if (hasFixedTypeAttr(element))
            options.set(FIXED_FLAG);

        options.set(FIXED_TYPE_FLAG, hasFixedTypeTypeAttr(element));
        options.set(NULLABLE_FLAG, hasNullableTypeAttr(element));

        if (hasRequiredTypeAttr(element))
            options.set(REQUIRED_FLAG);

        if (hasOptionalTypeAttr(element))
            options.reset(REQUIRED_FLAG);

        const auto* elementKey = element.get().key();
        const auto* elementValue = element.get().value();

        assert(elementKey);

        if (hasOptionalTypeAttr(element))
            if (!definesValue(*elementValue)) {
                LOG(debug) << "omitting optional property while rendering value";
                return;
            }

        if (isVariable(element)) {
            if (const auto* strKeyEl = get<const StringElement>(elementKey)) {
                if (!strKeyEl->empty()) {
                    LOG(warning) << "improvised variable key; sample shall be used as key sample";
                    const auto& strKey = strKeyEl->get().get();
                    emplace_unique(obj, strKey, renderValue(*elementValue, passFlags(options)));
                    return;
                }

            } else if (const auto* extend = get<const ExtendElement>(elementKey)) {
                if (!extend->empty()) {
                    auto merged = extend->get().merge();
                    if (const auto* strKeyEl = get<const StringElement>(merged.get())) {
                        if (!strKeyEl->empty()) {
                            LOG(warning) << "improvised variable key; sample shall be used as key sample";
                            const auto& strKey = strKeyEl->get().get();
                            emplace_unique(obj, strKey, renderValue(*elementValue, passFlags(options)));
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
            auto strKey = key(element);
            emplace_unique(obj, strKey, renderValue(*elementValue, passFlags(options)));
        }
    }

    void renderProperty(so::Object& obj, const RefElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering property RefElement as JSON Value";

        const auto& resolvedEntry = element.attributes().find("resolved");
        if (resolvedEntry == element.attributes().end()) {
            LOG(error) << "expected all references to be resolved in backend";
            assert(false);
        }

        assert(resolvedEntry->second);
        renderProperty(obj, *resolvedEntry->second, passFlags(options));
    }

    void renderProperty(so::Object& value, const SelectElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering property SelectElement as JSON Value";

        so::Array oneOfs{};
        for (const auto& option : element.get()) {

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
                renderProperty(value, *optionEntry, passFlags(options));
            }

            return;
        }

        LOG(warning) << "no non-empty OptionElement in SelectElement; skipping property";
    }

    void renderProperty(so::Object& value, const ObjectElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering property ObjectElement as JSON Value";

        if (hasFixedTypeAttr(element))
            options.set(FIXED_FLAG);

        if (!element.empty())
            for (const auto& item : element.get()) {
                assert(item);
                renderProperty(value, *item, inheritFlags(options));
            }
    }

    void renderProperty(so::Object& value, const ExtendElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering property ExtendElement as JSON Value";

        if (element.empty())
            LOG(warning) << "empty data structure element in backend";

        auto merged = element.get().merge();
        assert(merged);
        renderProperty(value, *merged, passFlags(options));
    }

    void renderProperty(so::Object& value, const IElement& element, TypeAttributes options)
    {
        auto objPtr = &value;
        refract::visit(element, [objPtr, options](const auto& el) { //
            renderProperty(*objPtr, el, options);
        });
    }

    void renderItemSpecific(so::Array& a, const RefElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering item RefElement as JSON Value";

        const auto& resolved = resolve(element);
        if (const auto& mixin = get<const ArrayElement>(&resolved)) {
            if (!mixin->empty())
                for (const auto& item : mixin->get()) {
                    assert(item);
                    renderItem(a, *item, inheritOrPassFlags(options, *item));
                }
        }
    }

    void renderItemSpecific(so::Array& a, const NumberElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering item " << element.element() << "Element as JSON Value";

        options = updateTypeAttributes(element, options);

        if ((options.test(FIXED_FLAG) || definesValue(element)))
            a.data.emplace_back(renderValue(element, inheritOrPassFlags(options, element)));
        else
            LOG(debug) << "skipping empty non-fixed primitive element in ArrayElement";
    }

    void renderItemSpecific(so::Array& a, const StringElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering item " << element.element() << "Element as JSON Value";

        options = updateTypeAttributes(element, options);

        if ((options.test(FIXED_FLAG) || definesValue(element)))
            a.data.emplace_back(renderValue(element, inheritOrPassFlags(options, element)));
        else
            LOG(debug) << "skipping empty non-fixed primitive element in ArrayElement";
    }

    void renderItemSpecific(so::Array& a, const BooleanElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering item " << element.element() << "Element as JSON Value";

        options = updateTypeAttributes(element, options);

        if ((options.test(FIXED_FLAG) || definesValue(element)))
            a.data.emplace_back(renderValue(element, inheritOrPassFlags(options, element)));
        else
            LOG(debug) << "skipping empty non-fixed primitive element in ArrayElement";
    }

    void renderItem(so::Array& a, const IElement& element, TypeAttributes options)
    {
        auto aPtr = &a;
        refract::visit(element, [aPtr, options](const auto& el) { //
            renderItemSpecific(*aPtr, el, options);
        });
    }
} // namespace
