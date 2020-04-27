//
//  refract/Registry.cc
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "Registry.h"

#include "Element.h"
#include "Exception.h"
#include "TypeQueryVisitor.h"
#include <algorithm>

using namespace refract;

namespace
{
    Registry::type_map baseTypeMap()
    {
        Registry::type_map result;
        result.emplace(std::make_pair("boolean", make_empty<BooleanElement>()));
        result.emplace(std::make_pair("number", make_empty<NumberElement>()));
        result.emplace(std::make_pair("string", make_empty<StringElement>()));
        result.emplace(std::make_pair("array", make_empty<ArrayElement>()));
        result.emplace(std::make_pair("object", make_empty<ObjectElement>()));
        result.emplace(std::make_pair("enum", make_empty<EnumElement>()));
        result.emplace(std::make_pair("null", make_empty<NullElement>()));
        return result;
    }
}

Registry::Registry() : types_{baseTypeMap()} {}

const IElement* refract::FindRootAncestor(const std::string& name, const Registry& registry)
{
    const IElement* parent = registry.find(name);

    while (parent && !isReserved(parent->element())) {
        const IElement* next = registry.find(parent->element());

        if (!next || (next == parent)) {
            return parent;
        }

        parent = next;
    }

    return parent;
}

namespace
{
    std::string getElementId(const IElement& element)
    {
        auto it = element.meta().find("id");

        if (it == element.meta().end()) {
            throw LogicError("Element has no ID");
        }

        if (const StringElement* s = TypeQueryVisitor::as<const StringElement>(it->second.get())) {
            return s->get();
        }

        throw LogicError("Value of element meta 'id' is not StringElement");
    }
}

const IElement* Registry::find(const std::string& name) const
{
    auto i = types_.find(name);

    if (i == types_.end()) {
        return nullptr;
    }

    return i->second.get();
}

bool Registry::add(std::unique_ptr<IElement> element)
{
    assert(element);

    auto it = element->meta().find("id");

    if (it == element->meta().end()) {
        throw LogicError("Element has no ID");
    }

    std::string id = getElementId(*element);

    if (isReserved(id)) {
        throw LogicError("You can not register a basic element");
    }

    if (find(id)) {
        // there is already already element with given name
        return false;
    }

    types_[id] = std::move(element);
    return true;
}

bool Registry::remove(const std::string& name)
{
    auto i = types_.find(name);

    if (i == types_.end()) {
        return false;
    }

    types_.erase(i);
    return true;
}

void Registry::clear()
{
    types_.clear();
}
