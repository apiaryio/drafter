//
//  refract/ElementUtils.h
//  librefract
//
//  Created by Thomas Jandecka on 07/02/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_ELEMENT_UTILS_H
#define REFRACT_ELEMENT_UTILS_H

#include "ElementIfc.h"
#include "ElementFwd.h"

namespace refract
{
    ///
    /// Interpret an Element as a given type
    ///
    /// @param e    the Element to be cast
    /// @return     nullptr iff given Element is not of given type;
    ///             otherwise given Element cast to given type
    ///
    template <typename Element>
    Element* get(IElement* e)
    {
        return dynamic_cast<Element*>(e);
    }

    ///
    /// Interpret an Element as a given type
    ///
    /// @param e    the Element to be cast
    /// @return     nullptr iff given Element is not of given type;
    ///             otherwise given Element cast to given type
    ///
    template <typename Element>
    const Element* get(const IElement* e)
    {
        return dynamic_cast<const Element*>(e);
    }
} // namespace refract

namespace refract
{
    ///
    /// Check whether an Element inherits the fixed type attribute from
    ///     a hypothetical parent
    ///
    /// @param e    the Element in question
    ///
    bool inheritsFixed(const ObjectElement& e);
    bool inheritsFixed(const MemberElement& e);
    bool inheritsFixed(const ArrayElement& e);
    bool inheritsFixed(const EnumElement& e);
    bool inheritsFixed(const NullElement& e);
    bool inheritsFixed(const StringElement& e);
    bool inheritsFixed(const NumberElement& e);
    bool inheritsFixed(const BooleanElement& e);
    bool inheritsFixed(const ExtendElement& e);
    bool inheritsFixed(const IElement& e);

    template <typename T>
    bool inheritsFixed(const T& e)
    {
        return true;
    }
} // namespace refract

namespace refract
{

    ///
    /// Check whether an Element has a specific type attribute set
    ///
    /// @example Given the Element below, hasTypeAttr(e, "fixed") == true
    ///
    ///     element: string
    ///     attributes:
    ///         typeAttributes:
    ///             -
    ///             element: string
    ///             content: fixed
    ///
    /// @param e    the Element in question
    /// @param name the name of the type attribute to be found
    ///
    /// @return     whether given Element has given type attribute
    ///
    bool hasTypeAttr(const IElement& e, const char* name);

    bool hasFixedTypeAttr(const IElement& e);

    bool hasFixedTypeTypeAttr(const IElement& e);

    bool hasRequiredTypeAttr(const IElement& e);

    bool hasOptionalTypeAttr(const IElement& e);

    bool hasNullableTypeAttr(const IElement& e);

    bool isVariable(const IElement& e);

    bool hasDefault(const IElement& e);

    std::string key(const MemberElement& m);

    ///
    /// Find a default value for an Element
    ///
    /// @returns    an Element typing a single value representing the default;
    ///             nullptr iff default is not found
    ///
    template <typename Element>
    const Element* findDefault(const Element& e)
    {
        auto it = e.attributes().find("default");
        if (it != e.attributes().end()) {
            if (const auto& result = get<const Element>(it->second.get()))
                return result;
        }
        return nullptr;
    }

    ///
    /// Find a sample value for an Element
    ///
    /// @returns    an Element typing a single value representing the sample;
    ///             nullptr iff sample is not found
    ///
    template <typename Element>
    const Element* findSample(const Element& e)
    {
        {
            auto it = e.attributes().find("sample");
            if (it != e.attributes().end()) {
                if (auto result = get<const Element>(it->second.get()))
                    return result;
            }
        }
        {
            auto it = e.attributes().find("samples");
            if (it != e.attributes().end()) {
                if (const auto& samples = get<const ArrayElement>(it->second.get()))
                    if (!samples->empty() && !samples->get().empty())
                        if (const auto& result = get<const Element>(samples->get().begin()[0].get()))
                            return result;
            }
        }
        return nullptr;
    }
} // namespace refract

#endif
