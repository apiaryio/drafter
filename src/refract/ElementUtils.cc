//
//  refract/ElementUtils.cc
//  librefract
//
//  Created by Thomas Jandecka on 02/07/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "ElementUtils.h"

#include "../utils/log/Trivial.h"
#include "Element.h"
#include "Utils.h"

#include <algorithm>
#include <cassert>

using namespace refract;

template <typename Element>
Element* get(IElement* e)
{
    return dynamic_cast<Element*>(e);
}

template <typename Element>
Element* get(const IElement* e)
{
    return dynamic_cast<Element*>(e);
}

bool refract::inheritsFixed(const IElement& e)
{
    // a default guarantees to be a valid value
    if (hasDefault(e))
        return true;

    // otherwise interpret based on specific Element
    return refract::visit(e, [](const auto& el) { //
        return inheritsFixed(el);
    });
}

bool refract::inheritsFixed(const ObjectElement& e)
{
    if (e.empty())
        return false;

    return true;
}

bool refract::inheritsFixed(const ArrayElement& e)
{
    if (e.empty())
        return false;

    return true;
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
    return !e.empty() || hasDefault(e); // content or default
}

bool refract::inheritsFixed(const NumberElement& e)
{
    return !e.empty() || hasDefault(e); // content or default
}

bool refract::inheritsFixed(const BooleanElement& e)
{
    return !e.empty() || hasDefault(e); // content or default
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
            return e != std::find_if(b, e, [&name](const auto& el) { //
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
    auto typeAttrIt = e.attributes().find("typeAttributes");
    if (e.attributes().end() == typeAttrIt) {
        e.attributes().set("typeAttributes", make_element<ArrayElement>(from_primitive("fixed")));
    } else {
        if (auto* typeAttrs = get<ArrayElement>(typeAttrIt->second.get())) {
            const auto b = typeAttrs->get().begin();
            const auto e = typeAttrs->get().end();
            if (e == std::find_if(b, e, [](const auto& el) { //
                    const auto* entry = get<const StringElement>(el.get());
                    return entry && !entry->empty() && (entry->get().get() == "fixed");
                })) {
                typeAttrs->get().push_back(from_primitive("fixed"));
            }
        }
    }
}
