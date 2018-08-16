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
#include "JsonUtils.h"
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
    void renderItemSpecific(so::Array& array, const T& element, TypeAttributes options)
    {
        LOG(debug) << "rendering item " << element.element() << "Element as JSON Value";
        array.data.emplace_back(renderValue(element, inheritOrPassFlags(options, element)));
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
        LOG(debug) << "rendering `" << element.element() << "` element to JSON Value";
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
    /// Search for sample, default or NULLABLE_FLAG, in that order, and
    ///     generate a matching simple object value
    ///
    /// @tparam Element type of Element sample or default shall be searched on
    ///
    /// @param element        element to be searched on
    /// @param options        type attributes to be applied to sample/default
    ///
    /// @return         pair of [<whether successful>, <rendered so value>]
    ///
    template <typename Element>
    std::pair<bool, so::Value> renderSampleOrDefaultOrNull(const Element& element, TypeAttributes options)
    {
        if (const auto& sampleValue = findFirstSample(element))
            return { true, renderValueSpecific(*sampleValue, options) };

        if (const auto& defaultValue = findDefault(element))
            return { true, renderValueSpecific(*defaultValue, options) };

        if (options.test(NULLABLE_FLAG))
            return { true, so::Null{} };

        return { false, so::Null{} };
    }
} // namespace

namespace
{
    template <typename Element>
    so::Value renderValuePrimitive(const Element& element, TypeAttributes options)
    {
        options = updateTypeAttributes(element, options);

        if (element.empty()) {
            auto alt = renderSampleOrDefaultOrNull(element, passFlags(options));
            if (alt.first)
                return std::move(alt.second);

            LOG(warning) << "no value found for element `" << element.element() << "`";
            return utils::instantiateEmpty(element);
        }

        return utils::instantiate(element.get());
    }

    so::Value renderValueSpecific(const StringElement& element, TypeAttributes options)
    {
        return renderValuePrimitive(element, passFlags(options));
    }

    so::Value renderValueSpecific(const NumberElement& element, TypeAttributes options)
    {
        return renderValuePrimitive(element, passFlags(options));
    }

    so::Value renderValueSpecific(const BooleanElement& element, TypeAttributes options)
    {
        return renderValuePrimitive(element, passFlags(options));
    }

    so::Value renderValueSpecific(const ObjectElement& element, TypeAttributes options)
    {
        so::Object result{};

        options = updateTypeAttributes(element, options);

        if (element.empty()) {
            auto alt = renderSampleOrDefaultOrNull(element, passFlags(options));
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
        options = updateTypeAttributes(element, options);

        so::Array result{};
        if (element.empty()) {
            auto alt = renderSampleOrDefaultOrNull(element, passFlags(options));
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
        options = updateTypeAttributes(element, options);

        if (element.empty()) {
            auto alt = renderSampleOrDefaultOrNull(element, passFlags(options));
            if (alt.first)
                return std::move(alt.second);

            auto enumerationsIt = element.attributes().find("enumerations");
            if (element.attributes().end() != enumerationsIt) {
                const auto enums = get<const ArrayElement>(enumerationsIt->second.get());
                assert(enums);
                if (!enums->empty())
                    for (const auto& enumEntry : enums->get()) {
                        assert(enumEntry);
                        return renderValue(*enumEntry, passFlags(options));
                    }
            }

            LOG(warning) << "no value found for EnumElement; using `null`";
            return so::Null{};
        }

        assert(element.get().value());
        return renderValue(*element.get().value(), inheritFlags(options));
    } // namespace

    so::Value renderValueSpecific(const NullElement& element, TypeAttributes options)
    {
        return so::Null{};
    }

    so::Value renderValueSpecific(const ExtendElement& element, TypeAttributes options)
    {
        auto merged = element.get().merge();
        assert(merged);
        return renderValue(*merged, options);
    }

} // namespace

namespace
{

    void renderProperty(so::Object& obj, const MemberElement& element, TypeAttributes options)
    {
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
        const auto& resolved = utils::resolve(element);
        renderProperty(obj, resolved, passFlags(options));
    }

    void renderProperty(so::Object& value, const SelectElement& element, TypeAttributes options)
    {
        so::Array oneOfs{};
        for (const auto& option : element.get()) {

            if (option->empty()) {
                LOG(error) << "unexpected empty option element in backend";
                assert(false);
                continue;
            }

            if (option->get().size() < 1) {
                LOG(warning) << "empty option element in backend; skipping";
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
        if (element.empty())
            LOG(warning) << "empty extend element in backend";

        auto merged = element.get().merge();
        assert(merged);
        renderProperty(value, *merged, passFlags(options));
    }

    void renderProperty(so::Object& value, const IElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering property `" << element.element() << "` as JSON Value";

        auto objPtr = &value;
        refract::visit(element, [objPtr, options](const auto& el) { //
            renderProperty(*objPtr, el, options);
        });
    }

    void renderItemSpecific(so::Array& array, const RefElement& element, TypeAttributes options)
    {
        const auto& resolved = utils::resolve(element);
        if (const auto& mixin = get<const ArrayElement>(&resolved)) {
            // OPTIM @tjanc@ avoid temporary container
            so::Value mixinValue = renderValueSpecific(*mixin, passFlags(options));
            if (const so::Array* mixinValueArray = get_if<so::Array>(mixinValue))
                std::move(mixinValueArray->data.begin(), mixinValueArray->data.end(), std::back_inserter(array.data));
        }
    }

    void renderItemSpecific(so::Array& array, const NumberElement& element, TypeAttributes options)
    {
        options = updateTypeAttributes(element, options);

        if ((options.test(FIXED_FLAG) || definesValue(element)))
            array.data.emplace_back(renderValue(element, inheritOrPassFlags(options, element)));
        else
            LOG(debug) << "skipping empty non-fixed primitive element in ArrayElement";
    }

    void renderItemSpecific(so::Array& array, const StringElement& element, TypeAttributes options)
    {
        options = updateTypeAttributes(element, options);

        if ((options.test(FIXED_FLAG) || definesValue(element)))
            array.data.emplace_back(renderValue(element, inheritOrPassFlags(options, element)));
        else
            LOG(debug) << "skipping empty non-fixed primitive element in ArrayElement";
    }

    void renderItemSpecific(so::Array& array, const BooleanElement& element, TypeAttributes options)
    {
        options = updateTypeAttributes(element, options);

        if ((options.test(FIXED_FLAG) || definesValue(element)))
            array.data.emplace_back(renderValue(element, inheritOrPassFlags(options, element)));
        else
            LOG(debug) << "skipping empty non-fixed primitive element in ArrayElement";
    }

    void renderItem(so::Array& array, const IElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering item `" << element.element() << "` element as JSON Value";
        auto aPtr = &array;
        refract::visit(element, [aPtr, options](const auto& el) { //
            renderItemSpecific(*aPtr, el, options);
        });
    }
} // namespace
