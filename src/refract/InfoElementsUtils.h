//
//  refract/InfoElementsUtils.h
//  librefract
//
//  Created by Jiri Kratochvil on 10/04/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_INFO_ELEMENTS_UTILS_H
#define REFRACT_INFO_ELEMENTS_UTILS_H

#include "InfoElements.h"
#include "TypeQueryVisitor.h"
#include "Element.h"

namespace refract
{

    template <typename ValueElementType, typename DSDType>
    void AppendInfoElement(InfoElements& ie, const std::string& key, DSDType&& value)
    {
        auto ta = ie.find(key);
        if (ta == ie.end()) {
            ie.set(key, make_element<ValueElementType>(from_primitive("fixed")));
        } else {
            auto arr = TypeQueryVisitor::as<ValueElementType>(ta->second.get());

            // not appropriate type of value
            assert(arr);

            if (arr->empty()) {
                arr->set(dsd::Array{ make_element<Element<DSDType> >(std::move(value)) });
                return;
            }

            const auto e = arr->get().end();
            if (e == std::find_if(arr->get().begin(), e, [&value](const auto& attr) {
                    if (const auto& str = TypeQueryVisitor::as<Element<DSDType> >(attr.get())) {
                        if (str->get() == value.get())
                            return true;
                    }

                    return false;
                })) { // there is no value
                arr->get().insert(arr->get().end(), make_element<Element<DSDType> >(std::move(value)));
            }
        }
    }
}

#endif // #ifndef REFRACT_INFO_ELEMENTS_UTILS_H
