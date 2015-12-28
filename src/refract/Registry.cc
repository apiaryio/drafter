//
//  refract/Registry.cc
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "Registry.h"
#include "Element.h"
#include "Visitors.h"

#include <algorithm>

namespace refract
{

    IElement* FindRootAncestor(const std::string& name, const Registry& registry)
    {
        IElement* parent = registry.find(name);

        while (parent && !isReserved(parent->element())) {
            IElement* next = registry.find(parent->element());

            if (!next || (next == parent)) {
                return parent;
            }

            parent = next;
        }

        return parent;
    }

    std::string Registry::getElementId(IElement* element)
    {
        IElement::MemberElementCollection::const_iterator it = element->meta.find("id");

        if (it == element->meta.end()) {
            throw LogicError("Element has no ID");
        }

        SerializeCompactVisitor v;
        (*it)->value.second->content(v);

        if (StringElement* s = TypeQueryVisitor::as<StringElement>((*it)->value.second)) {
            return s->value;
        }

        throw LogicError("Value of element meta 'id' is not StringElement");
    }

    IElement* Registry::find(const std::string& name) const
    {
        Map::const_iterator i = registrated.find(name);

        if (i == registrated.end()) {
            return NULL;
        }

        return i->second;
    }

    bool Registry::add(IElement* element)
    {
        IElement::MemberElementCollection::const_iterator it = element->meta.find("id");

        if (it == element->meta.end()) {
            throw LogicError("Element has no ID");
        }

        std::string id = getElementId(element);

        if (isReserved(id)) {
            throw LogicError("You can not register a basic element");
        }

        if (find(id)) {
            // there is already already element with given name
            return false;
        }

        registrated[id] = element;
        return true;
    }

    bool Registry::remove(const std::string& name) {
        Map::iterator i = registrated.find(name);

        if (i == registrated.end()) {
            return false;
        }

        registrated.erase(i);
        return true;
    }

    template<typename T>
    static void DeleteSecond(T& pair)
    {
        delete pair.second;
    }

    void Registry::clearAll(bool releaseElements) {
        if (releaseElements) {
            std::for_each(registrated.begin(), registrated.end(), DeleteSecond<Map::value_type>);
        }

        registrated.clear();
    }

}; // namespace refract
