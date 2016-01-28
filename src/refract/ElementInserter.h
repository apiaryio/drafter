//
//  refract/ElementInserter.h
//  librefract
//
//  Created by Jiri Kratochvil on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_ELEMENTINSERTER_H
#define REFRACT_ELEMENTINSERTER_H

#include <iterator>

namespace refract
{

    // Forward declarations of Elements
    struct IElement;

    template <typename T>
    struct ElementInsertIterator : public std::iterator<std::output_iterator_tag, void, void, void, void> {
        T& element;
        ElementInsertIterator(T& element) : element(element) {}

        ElementInsertIterator& operator=(refract::IElement* e) {
            element.push_back(e);
            return *this;
        }

        ElementInsertIterator& operator++() {
            return *this;
        }

        ElementInsertIterator& operator*() {
            return *this;
        }
        
    };

    template <typename T>
    ElementInsertIterator<T> ElementInserter(T& element) {
        return ElementInsertIterator<T>(element);
    }

}; // namespace refract

#endif // #ifndef REFRACT_ELEMENTINSERTER_H
