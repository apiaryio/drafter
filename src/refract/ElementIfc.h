//
//  refract/ElementIfc.h
//  librefract
//
//  Created by Thomas Jandecka on 21/08/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef REFRACT_ELEMENTIFC_H
#define REFRACT_ELEMENTIFC_H

#include <string>
#include <memory>

namespace refract
{
    class InfoElements;
    class Visitor;
    struct IVisitor; // XXX @tjanc@ experimental

    ///
    /// Refract Element interface definition
    ///
    struct IElement {

        ///
        /// Composable clone flags
        ///
        typedef enum
        {
            cMeta = 0x01,                                   //< Clone meta
            cAttributes = 0x02,                             //< Clone attributes
            cValue = 0x04,                                  //< Clone value
            cElement = 0x08,                                //< Clone name
            cAll = cMeta | cAttributes | cValue | cElement, //< Clone everything
            cNoMetaId = 0x10,                               //< Don't clone the meta `id` element
        } cloneFlags;

        ///
        /// Query the meta of this Element
        ///
        /// @return named Elements representing meta information
        ///
        virtual InfoElements& meta() noexcept = 0;
        //
        ///
        /// Query the meta of this Element
        ///
        /// @return named Elements representing meta information
        ///
        virtual const InfoElements& meta() const noexcept = 0;

        ///
        /// Query the attributes of this Element
        ///
        /// @return named Elements representing attributes
        ///
        virtual InfoElements& attributes() noexcept = 0;

        ///
        /// Query the attributes of this Element
        ///
        /// @return named Elements representing attributes
        ///
        virtual const InfoElements& attributes() const noexcept = 0;

        ///
        /// Query name of this Element
        ///
        /// @return Element name
        ///
        virtual std::string element() const = 0;

        ///
        /// Set name of this Element
        ///
        /// @param new name
        ///
        virtual void element(const std::string&) = 0;

        ///
        /// Visit the data structure representation (DSD) of this Element
        /// NOTE: probably rename to Accept
        ///
        /// @param v    a functor accepting any DSD
        ///
        virtual void content(Visitor& v) const = 0;

        ///
        /// Visit the implementation
        ///
        /// @param v    a functor accepting any DSD
        ///
        virtual void visit(IVisitor& v) const = 0;

        ///
        /// Clone (parts) of this Element
        ///
        /// @param flags    clone flags
        ///
        /// @return the new Element
        ///
        virtual std::unique_ptr<IElement> clone(int flags = cAll) const = 0;

        ///
        /// Query whether the Element was set
        ///
        /// @return true iff the Element was set
        ///
        virtual bool empty() const = 0;

        virtual ~IElement() = default;
    };

    ///
    /// Clone an Element
    ///
    /// @param el       Element to be cloned from
    /// @param flags    clone flags to be used
    ///
    /// @returns a clone of the given Element
    ///
    template <typename ElementT, typename = std::enable_if<std::is_base_of<IElement, ElementT>::value> >
    std::unique_ptr<ElementT> clone(const ElementT& el, int flags = IElement::cAll)
    {
        return std::unique_ptr<ElementT>(static_cast<ElementT*>(el.clone(flags).release()));
    }
}

#endif
