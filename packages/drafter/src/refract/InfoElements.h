//
//  refract/InfoElements.h
//  librefract
//
//  Created by Thomas Jandecka on 21/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_INFO_ELEMENTS_H
#define REFRACT_INFO_ELEMENTS_H

#include <memory>
#include <string>
#include <vector>

#include "ElementIfc.h"

namespace refract
{
    class InfoElements final
    {
        using Container = std::vector<std::pair<std::string, std::unique_ptr<IElement> > >;
        Container elements;

    public:
        using iterator = typename Container::iterator;
        using const_iterator = typename Container::const_iterator;
        using value_type = typename Container::value_type;

    public:
        InfoElements();
        ~InfoElements() = default;

        InfoElements(const InfoElements&);
        InfoElements(InfoElements&&);

        InfoElements& operator=(InfoElements);

    public:
        friend void swap(InfoElements& lhs, InfoElements& rhs)
        {
            using std::swap;
            swap(lhs.elements, rhs.elements);
        }

    public:
        const_iterator begin() const noexcept;

        iterator begin() noexcept;

        const_iterator end() const noexcept;

        iterator end() noexcept;

        const_iterator find(const std::string& name) const;
        iterator find(const std::string& name);

        IElement& set(const std::string& key, std::unique_ptr<IElement> value);
        IElement& set(const std::string& key, const IElement& value);

        /// clone elements from `other` to `this`
        void clone(const InfoElements& other);

        void erase(const std::string& key);
        void erase(iterator it);

        std::unique_ptr<IElement> claim(const std::string& key);
        std::unique_ptr<IElement> claim(iterator it);

        void clear();

        bool empty() const noexcept;

        Container::size_type size() const noexcept;
    };
}

#endif
