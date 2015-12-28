//
//  refract/Registry.h
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_REGISTRY_H
#define REFRACT_REGISTRY_H

#include <map>
#include <string>

namespace refract
{

    // Forward declarations of IElement
    struct IElement;

    class Registry
    {
        // FIXME: potentionally dangerous,
        // if element is deleted and not removed from registry
        // solution: std::shared_ptr<> || std::weak_ptr<>
        typedef std::map<std::string, IElement*> Map;
        Map registrated;

        std::string getElementId(IElement* element);

    public:

        IElement* find(const std::string& name) const;

        bool add(IElement* element) ;
        bool remove(const std::string& name);
        void clearAll(bool releaseElements = false);
    };

    IElement* FindRootAncestor(const std::string& name, const Registry& registry);

}; // namespace refract

#endif // #ifndef REFRACT_REGISTRY_H
