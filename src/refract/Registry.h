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
#include <memory>

#include "ElementIfc.h"

namespace refract
{
    class Registry
    {
        // FIXME: potentionally dangerous,
        // if element is deleted and not removed from registry
        // solution: std::shared_ptr<> || std::weak_ptr<>
        typedef std::map<std::string, std::unique_ptr<IElement> > Map;
        Map registrated;

        std::string getElementId(IElement& element);

    public:
        const IElement* find(const std::string& name) const;

        bool add(std::unique_ptr<IElement> element);
        bool remove(const std::string& name);
        void clearAll(bool releaseElements = false);
    };

    const IElement* FindRootAncestor(const std::string& name, const Registry& registry);

}; // namespace refract

#endif // #ifndef REFRACT_REGISTRY_H
