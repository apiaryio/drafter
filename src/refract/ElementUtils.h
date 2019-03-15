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
#include <limits>

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
    void setFixedTypeAttribute(IElement& e);
    void setTypeAttribute(IElement& e, const std::string& typeAttribute);
    void setDefault(IElement& e, std::unique_ptr<IElement> deflt);
    void addSample(IElement& e, std::unique_ptr<IElement> sample);
    void addEnumeration(IElement& e, std::unique_ptr<IElement> enm);
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

    ///
    /// Find a default value for an Element
    ///
    /// @returns    an Element typing a single value representing the default;
    ///             nullptr iff default is not found
    ///
    const IElement* findDefault(const IElement& e);

    ///
    /// Find the first sample value for an Element
    ///
    /// @returns    an Element typing a single value representing the sample;
    ///             nullptr iff sample is not found
    ///
    const IElement* findFirstSample(const IElement& e);

    const IElement* findValue(const IElement& element);

    bool definesValue(const IElement& e);

} // namespace refract

namespace refract
{
    std::string renderKey(const IElement& element);
}

namespace refract
{
    class cardinal
    {
    public:
        using value_type = std::size_t;

    private:
        value_type data_ = 0;

    public:
        constexpr cardinal() noexcept = default;
        explicit constexpr cardinal(value_type d) noexcept : data_{ d } {}

        constexpr value_type data() const noexcept
        {
            return data_;
        }

    public:
        static constexpr cardinal open() noexcept
        {
            return cardinal{ std::numeric_limits<value_type>::max() };
        }

        static constexpr cardinal empty() noexcept
        {
            return cardinal{ 0 };
        }
    };

    constexpr bool operator==(cardinal lhs, cardinal rhs) noexcept
    {
        return lhs.data() == rhs.data();
    }

    constexpr bool operator!=(cardinal lhs, cardinal rhs) noexcept
    {
        return lhs.data() != rhs.data();
    }

    constexpr bool finite(cardinal a) noexcept
    {
        return a != cardinal::open();
    }

    constexpr cardinal operator+(cardinal lhs, cardinal rhs) noexcept
    {
        return (lhs == cardinal::open() || rhs == cardinal::open()) ? cardinal::open() :
                                                                      cardinal{ lhs.data() + rhs.data() };
    }

    constexpr cardinal operator*(cardinal lhs, cardinal rhs)noexcept
    {
        return (lhs == cardinal::empty() || rhs == cardinal::empty()) ?
            cardinal::empty() :
            ((lhs == cardinal::open() || rhs == cardinal::open()) ? cardinal::open() :
                                                                    cardinal{ lhs.data() * rhs.data() });
    }

    cardinal sizeOf(const ArrayElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const BooleanElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const EnumElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const ExtendElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const HolderElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const MemberElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const NullElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const NumberElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const ObjectElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const OptionElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const RefElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const SelectElement& e, bool inheritsFixed = false);
    cardinal sizeOf(const StringElement& e, bool inheritsFixed = false);

    cardinal sizeOf(const IElement& e, bool inheritsFixed = false);
}

#endif
