//
//  refract/SerializeSo.cc
//  librefract
//
//  Created by Thomas Jandecka on 11/07/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "SerializeSo.h"

#include "../utils/log/Trivial.h"
#include "Element.h"

using namespace refract;
using namespace serialize;
using namespace drafter::utils;
using namespace drafter::utils::log;

namespace
{
    so::Object serialize(const InfoElements& info, bool renderSourceMaps);

    so::Array serializeContent(const dsd::Object& e, bool renderSourceMaps);
    so::Array serializeContent(const dsd::Array& e, bool renderSourceMaps);
    so::Value serializeContent(const dsd::Enum& e, bool renderSourceMaps);
    so::Null serializeContent(const dsd::Null& e, bool renderSourceMaps);
    so::String serializeContent(const dsd::String& e, bool renderSourceMaps);
    so::Number serializeContent(const dsd::Number& e, bool renderSourceMaps);
    so::Value serializeContent(const dsd::Boolean& e, bool renderSourceMaps);
    so::Array serializeContent(const dsd::Extend& e, bool renderSourceMaps);
    so::Array serializeContent(const dsd::Select& e, bool renderSourceMaps);
    so::Array serializeContent(const dsd::Option& e, bool renderSourceMaps);
    so::Value serializeContent(const dsd::Holder& e, bool renderSourceMaps);
    so::Object serializeContent(const dsd::Member& e, bool renderSourceMaps);
    so::String serializeContent(const dsd::Ref& e, bool renderSourceMaps);

    struct SerializeContentVisitor {
        bool renderSourceMaps;

        template <typename ElementT>
        so::Value operator()(const ElementT& el) const
        {
            return serializeContent(el.get(), renderSourceMaps);
        }
    };

    so::Object serializeAny(const IElement& e, bool renderSourceMaps)
    {
        so::Object result;

        LOG(debug) << "Serializing element `" << e.element() << "`";
        result.data.emplace_back("element", so::String{ e.element() });

        {
            LOG(debug) << "Serializing meta of absolute length " << e.meta().size();
            auto meta = serialize(e.meta(), renderSourceMaps);
            if (!meta.data.empty())
                result.data.emplace_back("meta", std::move(meta));

            LOG(debug) << "Serializing meta of absolute length " << e.meta().size() << " [DONE]";
        }

        {
            LOG(debug) << "Serializing attribute of absolute length " << e.attributes().size();
            auto attr = serialize(e.attributes(), renderSourceMaps || e.element() == "annotation");
            if (!attr.data.empty())
                result.data.emplace_back("attributes", std::move(attr));

            LOG(debug) << "Serializing attribute of absolute length " << e.attributes().size() << " [DONE]";
        }

        if (!e.empty()) {
            result.data.emplace_back("content", visit(e, SerializeContentVisitor{ renderSourceMaps }));
        }

        return result;
    }
} // namespace

namespace
{
    so::Object serialize(const InfoElements& info, bool renderSourceMaps)
    {
        so::Object result;
        for (const auto& entry : info) {
            assert(entry.second);
            if (renderSourceMaps || entry.first != "sourceMap")
                result.data.emplace_back(entry.first, serializeAny(*entry.second, renderSourceMaps));
        }
        return result;
    }

    template <typename ValueT>
    so::Array serializeListContent(const ValueT& e, bool renderSourceMaps)
    {
        so::Array result;

        for (const auto& entry : e) {
            assert(entry);
            result.data.emplace_back(serializeAny(*entry, renderSourceMaps));
        }

        return result;
    }

} // namespace

namespace
{
    so::Array serializeContent(const dsd::Object& value, bool renderSourceMaps)
    {
        LOG(debug) << "Serializing ObjectElement content";
        return serializeListContent(value, renderSourceMaps);
    }

    so::Array serializeContent(const dsd::Array& value, bool renderSourceMaps)
    {
        LOG(debug) << "Serializing ArrayElement content";
        return serializeListContent(value, renderSourceMaps);
    }

    so::Value serializeContent(const dsd::Enum& value, bool renderSourceMaps)
    {
        LOG(debug) << "Serializing EnumElement content";
        assert(value.value());
        return serializeAny(*value.value(), renderSourceMaps);
    }

    so::Null serializeContent(const dsd::Null& value, bool)
    {
        LOG(debug) << "Serializing NullElement content";
        return so::Null{};
    }

    so::String serializeContent(const dsd::String& value, bool)
    {
        LOG(debug) << "Serializing StringElement content";
        return so::String{ value.get() };
    }

    so::Number serializeContent(const dsd::Number& value, bool)
    {
        LOG(debug) << "Serializing NumberElement content";
        return so::Number{ value.get() };
    }

    so::Value serializeContent(const dsd::Boolean& value, bool)
    {
        LOG(debug) << "Serializing BooleanElement content";
        if (value.get())
            return so::True{};
        return so::False{};
    }

    so::Array serializeContent(const dsd::Extend& value, bool renderSourceMaps)
    {
        LOG(debug) << "Serializing ExtendElement content";
        return serializeListContent(value, renderSourceMaps);
    }

    so::Array serializeContent(const dsd::Select& value, bool renderSourceMaps)
    {
        LOG(debug) << "Serializing SelectElement content";
        return serializeListContent(value, renderSourceMaps);
    }

    so::Array serializeContent(const dsd::Option& value, bool renderSourceMaps)
    {
        LOG(debug) << "Serializing OptionElement content";
        return serializeListContent(value, renderSourceMaps);
    }

    so::Value serializeContent(const dsd::Holder& value, bool renderSourceMaps)
    {
        LOG(debug) << "Serializing HolderElement content";
        assert(value.data());
        return serializeAny(*value.data(), renderSourceMaps);
    }

    so::Object serializeContent(const dsd::Member& value, bool renderSourceMaps)
    {
        LOG(debug) << "Serializing MemberElement content";
        so::Object result;

        assert(value.key());
        result.data.emplace_back("key", serializeAny(*value.key(), renderSourceMaps));

        if (const auto v = value.value())
            result.data.emplace_back("value", serializeAny(*v, renderSourceMaps));
        return result;
    }

    so::String serializeContent(const dsd::Ref& value, bool)
    {
        LOG(debug) << "Serializing RefElement content";
        return so::String{ value.symbol() };
    }

} // namespace

so::Value serialize::renderSo(const IElement& el, bool sourceMaps)
{
    LOG(info) << "Starting API Elements -> SO serialization";
    return serializeAny(el, sourceMaps);
}
