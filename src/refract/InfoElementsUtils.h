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

    /**
     * Helper function to add value into InfoElements
     *
     * You need no to check if there already exists InfoElement.key nor InfoElement.key.value
     *
     * @params:
     * - <ValueElementType> - type of InfoElement /see InfoElements::Container::value_type.second
     * - <DSDType> - dsd type of added element
     *
     * - ie - InfoElements, usually `element.attributes()` or `element.meta()`
     * - key - key of appended InfoElement /see InfoElements::Container::value_type.first
     * - value - dsd value of appended InfoElement /see InfoElements::Container::value_type.second
     *
     * Typically used for attributes[typeAttributes]. But it is generalized.
     * e.g.:
     * ```
     * AppendInfoElement<refract::ArrayElement>(e.attributes(), "typeAttributes", dsd::String{ "fixed" });
     * ```
     *
     * It will add `fixed' attribute into `typeAttributes` it works in following way
     * - if `key` does not exists, create new one typed as `ValueElementType`
     * - if `key` exists but has different type than `ValueElementType` it will end with `assert`
     * in next step
     * - look for existence of `Element<DSDType>` with value equal to `value`
     *   - if found, leave `attributes[key]` untouched
     *   - if not found add new one with `Element<DSDType>(value)`
     */

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
            if (e == std::find_if(arr->get().begin(), e, [&value](const std::unique_ptr<IElement>& attr) {
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
