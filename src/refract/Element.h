//
//  refract/Element.h
//  librefract
//
//  Created by Thomas Jandecka on 04/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_ELEMENT_H
#define REFRACT_ELEMENT_H

#include <string>
#include "dsd/ElementData.h"
#include "dsd/Traits.h"
#include "ElementIfc.h"
#include "InfoElements.h"
#include "Visitor.h"
#include "Utils.h"
#include <memory>

namespace refract
{
    template <typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    ///
    /// Refract Element definition
    ///
    /// @tparam DataType    Data structure definition (DSD) of the Refract Element
    ///
    template <typename DataType>
    class Element final : public IElement
    {
        InfoElements meta_ = {};       //< Refract Element meta
        InfoElements attributes_ = {}; //< Refract Element attributes

        bool hasValue_ = false; //< Whether DSD is set
        DataType data_ = {};    //< DSD

        std::string name_ = { DataType::name }; //< Name of the Element

    public:
        using ValueType = DataType; //< DSD type definition

    public:
        ///
        /// Initialize a Refract Element with empty DSD
        /// @remark sets name of the element to DataType::name
        ///
        Element() = default;

        ///
        /// Initialize a Refract Element from a DSD
        /// @remark sets name of the element to DataType::name
        ///
        explicit Element(DataType data) : hasValue_(true), data_(std::move(data)), name_(DataType::name) {}

        ///
        /// Initialize a Refract Element from given name and DSD
        ///
        Element(const std::string& name, DataType data) : hasValue_(true), data_(data), name_(name) {}

        Element(Element&&) = default;
        Element(const Element&) = default;

        Element& operator=(Element&&) = default;
        Element& operator=(const Element&) = default;

    public:
        DataType& get() noexcept
        {
            assert(hasValue_);
            return data_;
        }

        const DataType& get() const noexcept
        {
            assert(hasValue_);
            return data_;
        }

        void set(DataType data = {})
        {
            hasValue_ = true;
            data_ = data;
        }

    public: // IElement
        InfoElements& meta() noexcept override
        {
            return meta_;
        }

        const InfoElements& meta() const noexcept override
        {
            return meta_;
        }

        InfoElements& attributes() noexcept override
        {
            return attributes_;
        }

        const InfoElements& attributes() const noexcept override
        {
            return attributes_;
        }

        std::string element() const override
        {
            return name_;
        }

        void element(const std::string& name) override
        {
            name_ = name;
        }

        void content(Visitor& v) const override
        {
            v.visit(*this);
        }

        void visit(IVisitor& v) const override
        {
            v(*this);
        }

        std::unique_ptr<IElement> clone(int flags = IElement::cAll) const override
        {
            auto el = refract::make_unique<Element>();

            if (flags & IElement::cElement)
                el->element(name_);
            if (flags & IElement::cAttributes)
                el->attributes_ = attributes_;
            if (flags & IElement::cMeta) {
                el->meta_ = meta_; // FIXME use copy_if rather than full copy with remove
                if (flags & IElement::cNoMetaId)
                    el->meta_.erase("id");
            }
            if (flags & IElement::cValue) {
                el->hasValue_ = hasValue_;
                el->data_ = data_;
            }

            return std::move(el);
        }

        bool empty() const override
        {
            return !hasValue_;
        }
    };

    ///
    /// Create an empty Element of given type
    /// @remark an empty Element has an empty data structure definition (DSD)
    ///
    template <typename ElementT>
    std::unique_ptr<ElementT> make_empty()
    {
        return refract::make_unique<ElementT>();
    }

    ///
    /// Create an Element of given type forwarding arguments to the constructor
    /// of its data structure definition (DSD)
    ///
    template <typename ElementT, typename... Args>
    std::unique_ptr<ElementT> make_element(Args&&... args)
    {
        using DataT = typename ElementT::ValueType;
        return refract::make_unique<ElementT>(DataT{ std::forward<Args>(args)... });
    }

    template <typename Primitive, typename DataT = typename dsd::data_of<Primitive>::type>
    std::unique_ptr<Element<DataT> > from_primitive(const Primitive& p)
    {
        return make_element<Element<DataT> >(p);
    }

    template <typename Primitive, typename DataT = typename dsd::data_of<Primitive>::type>
    std::unique_ptr<Element<DataT> > from_primitive_t(std::string name, const Primitive& p)
    {
        return refract::make_unique<Element<DataT> >(std::move(name), DataT{ p });
    }

    template <typename ElementT, typename ContentVisitor, typename... Args>
    std::unique_ptr<ElementT> generate_element(ContentVisitor visit, Args&&... visitorArgs)
    {
        auto element = make_element<ElementT>();
        visit(element->get(), std::forward<Args>(visitorArgs)...);
        return element;
    }

    bool isReserved(const char* w) noexcept;
    bool isReserved(const std::string& w) noexcept;
}

#endif
