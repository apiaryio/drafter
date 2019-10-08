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
    using TypeAttributes = std::bitset<2>;
    constexpr std::size_t FIXED_FLAG = 0;
    constexpr std::size_t NULLABLE_FLAG = 1;

    TypeAttributes updateTypeAttributes(const IElement& element, TypeAttributes options) noexcept
    {
        if (hasFixedTypeAttr(element))
            options.set(FIXED_FLAG);

        if (hasNullableTypeAttr(element))
            options.set(NULLABLE_FLAG);

        return options;
    }

    TypeAttributes passFlags(TypeAttributes options) noexcept
    {
        return options;
    }

    TypeAttributes inheritFlags(TypeAttributes options) noexcept
    {
        options.reset(NULLABLE_FLAG);
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
    void renderPropertySpecific(so::Object& obj, const ArrayElement& element, TypeAttributes options);
    void renderPropertySpecific(so::Object& obj, const BooleanElement& element, TypeAttributes options);
    void renderPropertySpecific(so::Object& obj, const EnumElement& element, TypeAttributes options);
    void renderPropertySpecific(so::Object& obj, const ExtendElement& element, TypeAttributes options);
    void renderPropertySpecific(so::Object& obj, const HolderElement& element, TypeAttributes options);
    void renderPropertySpecific(so::Object& obj, const MemberElement& element, TypeAttributes options);
    void renderPropertySpecific(so::Object& obj, const NullElement& element, TypeAttributes options);
    void renderPropertySpecific(so::Object& obj, const NumberElement& element, TypeAttributes options);
    void renderPropertySpecific(so::Object& obj, const ObjectElement& element, TypeAttributes options);
    void renderPropertySpecific(so::Object& obj, const OptionElement& element, TypeAttributes options);
    void renderPropertySpecific(so::Object& obj, const RefElement& element, TypeAttributes options);
    void renderPropertySpecific(so::Object& obj, const SelectElement& element, TypeAttributes options);
    void renderPropertySpecific(so::Object& obj, const StringElement& element, TypeAttributes options);
    void renderProperty(so::Object& obj, const IElement& element, TypeAttributes options);

    so::Value renderValueSpecific(const ArrayElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const BooleanElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const EnumElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const ExtendElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const HolderElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const MemberElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const NullElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const NumberElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const ObjectElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const OptionElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const RefElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const SelectElement& element, TypeAttributes options);
    so::Value renderValueSpecific(const StringElement& element, TypeAttributes options);
    so::Value renderValue(const IElement& element, TypeAttributes options);

    void renderItemSpecific(so::Array& array, const ArrayElement& element, TypeAttributes options);
    void renderItemSpecific(so::Array& array, const BooleanElement& element, TypeAttributes options);
    void renderItemSpecific(so::Array& array, const EnumElement& element, TypeAttributes options);
    void renderItemSpecific(so::Array& array, const ExtendElement& element, TypeAttributes options);
    void renderItemSpecific(so::Array& array, const HolderElement& element, TypeAttributes options);
    void renderItemSpecific(so::Array& array, const MemberElement& element, TypeAttributes options);
    void renderItemSpecific(so::Array& array, const NullElement& element, TypeAttributes options);
    void renderItemSpecific(so::Array& array, const NumberElement& element, TypeAttributes options);
    void renderItemSpecific(so::Array& array, const ObjectElement& element, TypeAttributes options);
    void renderItemSpecific(so::Array& array, const OptionElement& element, TypeAttributes options);
    void renderItemSpecific(so::Array& array, const RefElement& element, TypeAttributes options);
    void renderItemSpecific(so::Array& array, const SelectElement& element, TypeAttributes options);
    void renderItemSpecific(so::Array& array, const StringElement& element, TypeAttributes options);
    void renderItem(so::Array& array, const IElement& element, TypeAttributes options);
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
            return { true, renderValue(*sampleValue, options) };

        if (const auto& defaultValue = findDefault(element))
            return { true, renderValue(*defaultValue, options) };

        if (options.test(NULLABLE_FLAG))
            return { true, so::Null{} };

        return { false, so::Null{} };
    }
} // namespace

namespace
{
    template <typename T>
    so::Null errorByNull(const T& element)
    {
        LOG(error) << "invalid element, interpreting as null: " << element.element();
        return so::Null{};
    }

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

    so::Value renderValueSpecific(const MemberElement& element, TypeAttributes options)
    {
        return errorByNull(element);
    }

    so::Value renderValueSpecific(const OptionElement& element, TypeAttributes options)
    {
        return errorByNull(element);
    }

    so::Value renderValueSpecific(const SelectElement& element, TypeAttributes options)
    {
        return errorByNull(element);
    }

    so::Value renderValueSpecific(const HolderElement& element, TypeAttributes options)
    {
        if (!element.empty() && element.get().data())
            return renderValue(*element.get().data(), passFlags(options));
        return so::Null{};
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

            LOG(info) << "no value found for EnumElement; searching in `enumerations`";
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
    }

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

    so::Value renderValueSpecific(const RefElement& element, TypeAttributes options)
    {
        if (const IElement* resolved = resolve(element))
            return renderValue(*resolved, passFlags(options));
        LOG(warning) << "ignoring unresolved reference in json value backend";
        return so::Null{};
    }

    struct RenderValueVisitor {
        TypeAttributes options;

        template <typename ElementT>
        so::Value operator()(const ElementT& el) const
        {
            return renderValueSpecific(el, options);
        }
    };

    so::Value renderValue(const IElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering `" << element.element() << "` element to JSON Value";
        return refract::visit(element, RenderValueVisitor{ options });
    }

} // namespace

namespace
{
    template <typename T>
    void errorButSkipProperty(const T& element)
    {
        LOG(error) << "skipping invalid property element: " << element.element();
    }

    void renderPropertySpecific(so::Object& obj, const ArrayElement& element, TypeAttributes options)
    {
        errorButSkipProperty(element);
    }

    void renderPropertySpecific(so::Object& obj, const BooleanElement& element, TypeAttributes options)
    {
        errorButSkipProperty(element);
    }

    void renderPropertySpecific(so::Object& obj, const EnumElement& element, TypeAttributes options)
    {
        errorButSkipProperty(element);
    }

    void renderPropertySpecific(so::Object& obj, const NullElement& element, TypeAttributes options)
    {
        errorButSkipProperty(element);
    }

    void renderPropertySpecific(so::Object& obj, const NumberElement& element, TypeAttributes options)
    {
        errorButSkipProperty(element);
    }

    void renderPropertySpecific(so::Object& obj, const StringElement& element, TypeAttributes options)
    {
        errorButSkipProperty(element);
    }

    void renderPropertySpecific(so::Object& obj, const OptionElement& element, TypeAttributes options)
    {
        errorButSkipProperty(element);
    }

    void renderPropertySpecific(so::Object& obj, const HolderElement& element, TypeAttributes options)
    {
        if (!element.empty() && element.get().data())
            renderProperty(obj, *element.get().data(), options);
        else
            errorButSkipProperty(element);
    }

    void renderPropertySpecific(so::Object& obj, const MemberElement& element, TypeAttributes options)
    {
        options = updateTypeAttributes(element, options);

        const auto* elementKey = element.get().key();
        const auto* elementValue = element.get().value();

        assert(elementKey);

        if (hasOptionalTypeAttr(element))
            if (!definesValue(*elementValue)) {
                LOG(debug) << "omitting optional property while rendering value";
                return;
            }

        auto strKey = renderKey(*elementKey);

        if (!strKey.empty())
            emplace_unique(obj, strKey, renderValue(*elementValue, passFlags(options)));
    }

    void renderPropertySpecific(so::Object& obj, const RefElement& element, TypeAttributes options)
    {
        if (const IElement* resolved = resolve(element))
            renderProperty(obj, *resolved, passFlags(options));
        else
            LOG(warning) << "ignoring unresolved reference in json value backend";
    }

    void renderPropertySpecific(so::Object& value, const SelectElement& element, TypeAttributes options)
    {
        so::Array oneOfs{};
        for (const auto& option : element.get()) {

            if (option->empty()) {
                LOG(error) << "skipping unexpected empty option element in backend";
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

    void renderPropertySpecific(so::Object& value, const ObjectElement& element, TypeAttributes options)
    {
        // OPTIM @tjanc@ avoid temporary container
        so::Value mixinValue = renderValueSpecific(element, passFlags(options));
        if (so::Object* mixinValueObject = mpark::get_if<so::Object>(&mixinValue))
            for (auto& property : mixinValueObject->data)
                emplace_unique(value, std::move(property));
    }

    void renderPropertySpecific(so::Object& value, const ExtendElement& element, TypeAttributes options)
    {
        if (element.empty())
            LOG(warning) << "empty extend element in backend";

        auto merged = element.get().merge();
        assert(merged);
        renderProperty(value, *merged, passFlags(options));
    }

    struct RenderPropertyVisitor {
        so::Object* objPtr;
        TypeAttributes options;

        template <typename ElementT>
        void operator()(const ElementT& el)
        {
            renderPropertySpecific(*objPtr, el, options);
        }
    };

    void renderProperty(so::Object& value, const IElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering property `" << element.element() << "` as JSON Value";
        refract::visit(element, RenderPropertyVisitor{ &value, options });
    }

}

namespace
{
    template <typename T>
    void errorButSkipItem(const T& element)
    {
        LOG(error) << "skipping invalid item element: " << element.element();
    }

    template <typename Element>
    void renderItemPrimitive(so::Array& array, const Element& element, TypeAttributes options)
    {
        options = updateTypeAttributes(element, options);

        if ((options.test(FIXED_FLAG) || definesValue(element)))
            array.data.emplace_back(renderValueSpecific(element, inheritOrPassFlags(options, element)));
        else
            LOG(debug) << "skipping empty non-fixed primitive element in ArrayElement";
    }

    void renderItemSpecific(so::Array& array, const ArrayElement& element, TypeAttributes options)
    {
        array.data.emplace_back(renderValue(element, inheritOrPassFlags(options, element)));
    }

    void renderItemSpecific(so::Array& array, const EnumElement& element, TypeAttributes options)
    {
        array.data.emplace_back(renderValue(element, inheritOrPassFlags(options, element)));
    }

    void renderItemSpecific(so::Array& array, const ExtendElement& element, TypeAttributes options)
    {
        array.data.emplace_back(renderValueSpecific(element, inheritOrPassFlags(options, element)));
    }

    void renderItemSpecific(so::Array& array, const NullElement& element, TypeAttributes options)
    {
        array.data.emplace_back(renderValueSpecific(element, inheritOrPassFlags(options, element)));
    }

    void renderItemSpecific(so::Array& array, const ObjectElement& element, TypeAttributes options)
    {
        array.data.emplace_back(renderValueSpecific(element, inheritOrPassFlags(options, element)));
    }

    void renderItemSpecific(so::Array& array, const HolderElement& element, TypeAttributes options)
    {
        array.data.emplace_back(renderValueSpecific(element, inheritOrPassFlags(options, element)));
    };

    void renderItemSpecific(so::Array& array, const MemberElement& element, TypeAttributes options)
    {
        errorButSkipItem(element);
    };

    void renderItemSpecific(so::Array& array, const OptionElement& element, TypeAttributes options)
    {
        errorButSkipItem(element);
    };

    void renderItemSpecific(so::Array& array, const SelectElement& element, TypeAttributes options)
    {
        errorButSkipItem(element);
    };

    void renderItemSpecific(so::Array& array, const RefElement& element, TypeAttributes options)
    {
        const IElement* resolved = resolve(element);
        if (!resolved) {
            LOG(warning) << "ignoring unresolved reference in json value backend";
        } else if (const auto& mixin = get<const ArrayElement>(resolved)) {
            // OPTIM @tjanc@ avoid temporary container
            so::Value mixinValue = renderValueSpecific(*mixin, passFlags(options));
            if (const so::Array* mixinValueArray = mpark::get_if<so::Array>(&mixinValue))
                std::move(mixinValueArray->data.begin(), mixinValueArray->data.end(), std::back_inserter(array.data));
        }
    }

    void renderItemSpecific(so::Array& array, const NumberElement& element, TypeAttributes options)
    {
        renderItemPrimitive(array, element, options);
    }

    void renderItemSpecific(so::Array& array, const StringElement& element, TypeAttributes options)
    {
        renderItemPrimitive(array, element, options);
    }

    void renderItemSpecific(so::Array& array, const BooleanElement& element, TypeAttributes options)
    {
        renderItemPrimitive(array, element, options);
    }

    struct RenderItemVisitor {
        so::Array* aPtr;
        TypeAttributes options;

        template <typename ElementT>
        void operator()(const ElementT& el)
        {
            renderItemSpecific(*aPtr, el, options);
        }
    };

    void renderItem(so::Array& array, const IElement& element, TypeAttributes options)
    {
        LOG(debug) << "rendering item `" << element.element() << "` element as JSON Value";
        refract::visit(element, RenderItemVisitor{ &array, options });
    }
} // namespace

so::Value refract::generateJsonValue(const IElement& el)
{
    return renderValue(el, TypeAttributes{});
}
