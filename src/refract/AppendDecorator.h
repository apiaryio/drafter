//
//  refract/AppendDecorator.h
//  librefract
//
//  Created by Jiri Kratochvil on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_APPENDDECORATOR_H
#define REFRACT_APPENDDECORATOR_H

#include <iterator>

namespace refract
{

    // Forward declarations of Elements
    struct IElement;

    template <typename T, typename V = typename T::ValueType>
    struct AppendDecorator {
        typedef T ElementType;
        typedef V ValueType;
        ElementType*& element;
        AppendDecorator(ElementType*& e) : element(e)
        {
        }

        void operator()(const V& value)
        {
            // FIXME: snowcrash warn about "Primitive type can not have member"
            // but in real it create "empty" member
            //
            // solution for now: set if element has no already value, otherwise silently ignore
            if (element->empty()) {
                element->set(value);
            }
        }
    };

    template <typename T>
    struct AppendDecorator<T, std::vector<refract::IElement*> > {
        typedef T ElementType;
        typedef typename T::ValueType ValueType;
        ElementType*& element;

        AppendDecorator(ElementType*& e) : element(e)
        {
        }

        void operator()(const ValueType& value)
        {
            for_each(value.begin(), value.end(), std::bind1st(std::mem_fun(&ElementType::push_back), element));
        }
    };

}; // namespace refract

#endif // #ifndef REFRACT_APPENDDECORATOR_H
