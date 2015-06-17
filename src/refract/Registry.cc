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

namespace refract
{

    refract::Registry DSRegistry;

    std::string Registry::getElementId(IElement* element) 
    {
        IElement::MemberElementCollection::const_iterator it = element->meta.find("id");
        if (it == element->meta.end()) {
            throw LogicError("Element has no ID");
        }

        SerializeCompactVisitor v;
        (*it)->value.second->content(v);
        // FIXME: it is really ugly,
        // make something like is_a()
        return v.value().str;
    }

    IElement* Registry::find(const std::string& name)
    {
        Map::iterator i = registrated.find(name);
        if(i == registrated.end()) {
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
        //std::cout << "ID: >" << id << "<"<< std::endl;

        if(isReserved(id)) {
            throw LogicError("You can not registrate basic element");
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
        if(i == registrated.end()) {
            return false;
        }
        registrated.erase(i);
        return true;
    }

    void Registry::clearAll(bool releaseElements) {
        if(releaseElements) {
            for (Map::iterator i = registrated.begin() ; i != registrated.end() ; ++i) {
                delete i->second;
            }
        }
        registrated.clear();
    }



}; // namespace refract
