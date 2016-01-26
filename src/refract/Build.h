
//
//  refract/Build.h
//  librefract
//
//  Created by Jiri Kratochvil on 03/01/16.
//  Copyright (c) 2016 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_BUILD_H
#define REFRACT_BUILD_H

namespace refract
{
    /**
     * Simple concept based on operator() override. It 
     * allows tricky creating of complex refract structures
     * eg:
     * ```
        return Build(new ObjectElement)
           ("m1", IElement::Create("Value"))
           ("m2", Build(new ArrayElement)
                     (IElement::Create("m2[0]"))
           )
           ("m3", Build(new ObjectElement)
                     ("m3.1", IElement::Create("Str3.1"))
                     ("m3.2", IElement::Create(3.2))
                     ("m3.3", Build(new ObjectElement)
                                 ("m3.3.1", IElement::Create("Str3/4/1"))
                                 ("m3.3.2", IElement::Create(3.42))
                                 ("m3.3.2", new NullElement)
                     )
           );
     * ```
     */
    template <typename T>
    struct ElementBuilder {
        T* element;

        ElementBuilder(T* element) : element(element) {}

        operator T*() {
            return element;
        }

        ElementBuilder<T>& operator()(IElement* member) {
            element->push_back(member);
            return *this;
        }

        ElementBuilder<T>& operator()(IElement* key, IElement* value) {
            element->push_back(new MemberElement(key, value));
            return *this;
        }

        ElementBuilder<T>& operator()(const std::string& key, IElement* value) {
            element->push_back(new MemberElement(key, value));
            return *this;
        }
    };

    template <typename E>
    ElementBuilder<E> Build(E* e) {
        return ElementBuilder<E>(e);
    }
}; // namespace refract

#endif // #ifndef REFRACT_BUILD_H
