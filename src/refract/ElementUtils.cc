//
//  refract/ElementUtils.cc
//  librefract
//
//  Created by Thomas Jandecka on 02/07/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "ElementUtils.h"

#include "../utils/log/Trivial.h"
#include "JsonUtils.h"
#include "Element.h"
#include "Utils.h"
#include <algorithm>
#include <numeric>
#include <limits>
#include <cassert>

using namespace refract;

namespace
{
    struct InheritsFixedVisitor {
        template <typename ElementT>
        bool operator()(const ElementT& el) const noexcept
        {
            return inheritsFixed(el);
        }
    };
}

bool refract::inheritsFixed(const IElement& e)
{
    // otherwise interpret based on specific Element
    return refract::visit(e, InheritsFixedVisitor{});
}

bool refract::inheritsFixed(const ObjectElement& e)
{
    return true;
}

bool refract::inheritsFixed(const ArrayElement& e)
{
    return definesValue(e);
}

bool refract::inheritsFixed(const MemberElement& e)
{
    if (isVariable(e))
        return false;

    /// for consistency we shall check default on MemberElement as well
    if (e.empty())
        return hasDefault(e);

    assert(!e.empty());
    auto value = e.get().value();
    assert(value);
    return inheritsFixed(*value);
}

bool refract::inheritsFixed(const EnumElement& e)
{
    if (e.empty())
        return hasDefault(e);

    auto c = e.get().value();
    assert(c);
    return inheritsFixed(*c);
}

bool refract::inheritsFixed(const NullElement& e)
{
    return true; // null is bound to its only value
}

bool refract::inheritsFixed(const StringElement& e)
{
    return definesValue(e);
}

bool refract::inheritsFixed(const NumberElement& e)
{
    return definesValue(e);
}

bool refract::inheritsFixed(const BooleanElement& e)
{
    return definesValue(e);
}

bool refract::inheritsFixed(const ExtendElement& e)
{
    auto merged = e.get().merge();
    assert(merged);
    return inheritsFixed(*merged);
}

bool refract::hasTypeAttr(const IElement& e, const char* name)
{
    auto typeAttrIt = e.attributes().find("typeAttributes");

    if (typeAttrIt != e.attributes().end())
        if (const auto* typeAttrs = get<const ArrayElement>(typeAttrIt->second.get())) {
            const auto b = typeAttrs->get().begin();
            const auto e = typeAttrs->get().end();
            return e != std::find_if(b, e, [&name](const std::unique_ptr<IElement>& el) { //
                const auto* entry = get<const StringElement>(el.get());
                return entry && !entry->empty() && (entry->get().get() == name);
            });
        }
    return false;
}

bool refract::hasFixedTypeAttr(const IElement& e)
{
    return hasTypeAttr(e, "fixed");
}

bool refract::hasFixedTypeTypeAttr(const IElement& e)
{
    return hasTypeAttr(e, "fixedType");
}

bool refract::hasRequiredTypeAttr(const IElement& e)
{
    return hasTypeAttr(e, "required");
}

bool refract::hasOptionalTypeAttr(const IElement& e)
{
    return hasTypeAttr(e, "optional");
}

bool refract::hasNullableTypeAttr(const IElement& e)
{
    return hasTypeAttr(e, "nullable");
}

bool refract::isVariable(const IElement& e)
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

bool refract::hasDefault(const IElement& e)
{
    return findDefault(e) != nullptr;
}

std::string refract::renderKey(const IElement& element)
{
    if (const auto* stringElement = get<const StringElement>(&element)) {
        if (stringElement->empty()) {
            if (const auto& sampleValue = findFirstSample(*stringElement)) {
                return renderKey(*sampleValue);
            }

            if (const auto& defaultValue = findDefault(*stringElement)) {
                return renderKey(*defaultValue);
            }

        } else {
            return stringElement->get().get();
        }

    } else if (const auto* extendElement = get<const ExtendElement>(&element)) {
        auto merged = extendElement->get().merge();
        assert(merged);
        return renderKey(*merged);
    } else {
        LOG(error) << "expected key to resolve to string, got: " << element.element();
        assert(false);
    }

    return "";
}

void refract::setFixedTypeAttribute(IElement& e)
{
    setTypeAttribute(e, "fixed");
}

void refract::setTypeAttribute(IElement& e, const std::string& typeAttribute)
{
    auto typeAttrIt = e.attributes().find("typeAttributes");
    if (e.attributes().end() == typeAttrIt) {
        e.attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive(typeAttribute)));
    } else {
        if (auto* typeAttrs = get<ArrayElement>(typeAttrIt->second.get())) {
            const auto b = typeAttrs->get().begin();
            const auto e = typeAttrs->get().end();
            if (e == std::find_if(b, e, [&typeAttribute](const std::unique_ptr<IElement>& el) { //
                    const auto* entry = get<const StringElement>(el.get());
                    return entry && !entry->empty() && (entry->get().get() == typeAttribute);
                })) {
                typeAttrs->get().push_back(from_primitive(typeAttribute));
            }
        }
    }
}

void refract::setDefault(IElement& e, std::unique_ptr<IElement> deflt)
{
    e.attributes().set("default", std::move(deflt));
}

void refract::addSample(IElement& e, std::unique_ptr<IElement> sample)
{
    auto it = e.attributes().find("samples");
    if (it == e.attributes().end()) {
        LOG(info) << "creating new samples entry";
        e.attributes().set("samples", make_element<ArrayElement>(std::move(sample)));
    } else if (ArrayElement* samples = get<ArrayElement>(it->second.get())) {
        if (samples->empty()) {
            LOG(error) << "empty Array Element in samples";
            assert(false);
        }
        LOG(info) << "adding new sample";
        e.attributes().set("samples", make_element<ArrayElement>(std::move(sample)));
        samples->get().push_back(std::move(sample));
    } else {
        LOG(error) << "expected samples to be held in Array Element content";
        assert(false);
    }
}

void refract::addEnumeration(IElement& e, std::unique_ptr<IElement> enm)
{
    auto it = e.attributes().find("enumerations");
    if (it == e.attributes().end())
        e.attributes().set("enumerations", make_element<ArrayElement>(std::move(enm)));
    else if (ArrayElement* enums = get<ArrayElement>(it->second.get())) {
        if (enums->empty()) {
            LOG(error) << "empty Array Element in enumerations";
            assert(false);
        }
        enums->get().push_back(std::move(enm));
    } else {
        LOG(error) << "expected enumerations to be held in Array Element content";
        assert(false);
    }
}

const IElement* refract::findFirstSample(const IElement& e)
{
    auto it = e.attributes().find("samples");
    if (it != e.attributes().end()) {
        if (const auto& samples = get<const ArrayElement>(it->second.get()))
            if (!samples->empty() && !samples->get().empty())
                return samples->get().begin()[0].get();
    }
    return nullptr;
}

const IElement* refract::findDefault(const IElement& e)
{
    auto it = e.attributes().find("default");
    if (it != e.attributes().end())
        return it->second.get();
    return nullptr;
}

const IElement* refract::findValue(const IElement& element)
{
    if (!element.empty())
        return &element;
    if (const IElement* sample = findFirstSample(element))
        return sample;
    if (const IElement* dfault = findDefault(element))
        return dfault;
    return nullptr;
}

bool refract::definesValue(const IElement& e)
{
    return nullptr != findValue(e);
}

const IElement* refract::resolve(const RefElement& element)
{
    const auto resolvedEntry = element.attributes().find("resolved");
    if (resolvedEntry == element.attributes().end()) {
        LOG(error) << "expected all references to be resolved in backend";
        assert(false);
    }

    assert(resolvedEntry->second);
    return resolvedEntry->second.get();
}
