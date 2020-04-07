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
    public:
        using type_map = std::map<std::string, std::unique_ptr<IElement> >;

    private:
        type_map types_;

    public:
        Registry();

    public:
        const IElement* find(const std::string& name) const;

        bool add(std::unique_ptr<IElement> element);
        bool remove(const std::string& name);
        void clear();
    };

    const IElement* FindRootAncestor(const std::string& name, const Registry& registry);

} // namespace refract

#endif // #ifndef REFRACT_REGISTRY_H
