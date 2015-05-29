//
//  RefractAST.h
//  drafter
//
//  Created by Jiri Kratochvil on 18/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_REFRACT_AST_H
#define DRAFTER_REFRACT_AST_H

#include "Serialize.h"

// for registry
#include <set>
#include "refract/Element.h"
// ---

namespace snowcrash {
    struct DataStructure;
}

namespace refract {

    struct IElement;

    class Registry {
        // FIXME: potentionally dangerous,
        // if element is deleted and not removed from registry
        // solution: std::shared_ptr<> || std::weak_ptr<>
        typedef std::map<std::string, IElement*> Map;
        Map registrated;
        std::set<std::string> reserved;

        void initReserved() {
            reserved.insert("null");
            reserved.insert("boolean");
            reserved.insert("number");
            reserved.insert("string");

            reserved.insert("array");
            reserved.insert("enum");
            reserved.insert("object");

            reserved.insert("ref");
            reserved.insert("select");
            reserved.insert("option");
            reserved.insert("extend");
        }

        std::string getElementId(IElement* element) 
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

        bool isReserved(const std::string& name) 
        {
            if (reserved.empty()) {
                initReserved();
            }
            return reserved.find(name) != reserved.end();
        }
    public:

        IElement* find(const std::string& name)
        {
            Map::iterator i = registrated.find(name);
            if(i == registrated.end()) {
                return NULL;
            }
            return i->second;
        }

        bool add(IElement* element) 
        {
            IElement::MemberElementCollection::const_iterator it = element->meta.find("id");
            if (it == element->meta.end()) {
                throw LogicError("Element has no ID");
            }

            std::string id = getElementId(element);
            std::cout << "REG: " << id << " - ";

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

        bool remove(const std::string& name) {
            Map::iterator i = registrated.find(name);
            if(i == registrated.end()) {
                return false;
            }
            registrated.erase(i);
            return true;
        }

        void clearAll(bool releaseElements = false) {
            if(releaseElements) {
                for (Map::iterator i = registrated.begin() ; i != registrated.end() ; ++i) {
                    delete i->second;
                }
            }
            registrated.clear();
        }

    };
};

// FIXME: remove global variable!!
static refract::Registry DSRegistry;

namespace drafter {

    refract::IElement* DataStructureToRefract(const snowcrash::DataStructure& dataStructure);

    sos::Object SerializeRefract(refract::IElement*);

}

#endif // #ifndef DRAFTER_REFRACT_AST_H

