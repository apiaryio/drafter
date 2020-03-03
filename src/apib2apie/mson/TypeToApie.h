//
//  apib2apie/mson/TypeToApie.h
//  apib2apie
//
//  Created by Thomas Jandecka on 11/07/19.
//  Copyright (c) 2019 Apiary Inc. All rights reserved.
//

#ifndef APIB2APIE_MSON_TYPE_TO_APIE_H
#define APIB2APIE_MSON_TYPE_TO_APIE_H

#include <MSON.h>
#include <MSONSourcemap.h>
#include <memory>
#include "refract/ElementIfc.h"
#include "NodeInfo.h"

namespace drafter
{
    class ConversionContext;
}

namespace apib2apie
{
    mson::BaseTypeName ResolveType(const mson::TypeSpecification& spec, ConversionContext& context);

    std::unique_ptr<refract::IElement> MsonTypeSectionToApie( //
        const mson::Element::Empty&,
        const snowcrash::SourceMap<mson::Element>*,
        ConversionContext&,
        const mson::BaseTypeName);

    std::unique_ptr<refract::IElement> MsonTypeSectionToApie( //
        const mson::Element::PropertyMemberSection&,
        const snowcrash::SourceMap<mson::Element>*,
        ConversionContext&,
        const mson::BaseTypeName);

    std::unique_ptr<refract::IElement> MsonTypeSectionToApie( //
        const mson::Element::ValueMemberSection&,
        const snowcrash::SourceMap<mson::Element>*,
        ConversionContext&,
        const mson::BaseTypeName);

    std::unique_ptr<refract::IElement> MsonTypeSectionToApie( //
        const mson::Element::MixinSection&,
        const snowcrash::SourceMap<mson::Element>*,
        ConversionContext&,
        const mson::BaseTypeName);

    std::unique_ptr<refract::IElement> MsonTypeSectionToApie( //
        const mson::Element::OneOfSection&,
        const snowcrash::SourceMap<mson::Element>*,
        ConversionContext&,
        const mson::BaseTypeName);

    std::unique_ptr<refract::IElement> MsonTypeSectionToApie( //
        const mson::Element::OneOfSection&,
        const snowcrash::SourceMap<mson::Element>*,
        ConversionContext&,
        const mson::BaseTypeName);

    std::unique_ptr<refract::IElement> MsonTypeSectionToApie( //
        const mson::Element::GroupSection&,
        const snowcrash::SourceMap<mson::Element>*,
        ConversionContext&,
        const mson::BaseTypeName);

    std::unique_ptr<refract::IElement> MsonTypeSectionToApie( //
        const mson::Element&,
        const snowcrash::SourceMap<mson::Element>*,
        ConversionContext&,
        const mson::BaseTypeName);
}

namespace apib2apie
{
    template <typename WhereTo>
    void MsonTypeSectionsToApie(const NodeInfo<mson::Elements>& elements,
        WhereTo& container,
        ConversionContext& context,
        mson::BaseTypeName defaultNestedType = mson::StringTypeName)
    {
        NodeInfoCollection<mson::Elements> elementsNodeInfo(elements);

        for (const auto& nodeInfo : elementsNodeInfo)
            if (auto apie = MsonTypeSectionToApie(*nodeInfo.node, nodeInfo.sourceMap, context, defaultNestedType))
                container.push_back(std::move(apie));
    }
}

#endif
