//
//  MsonOneOfSectionToApie.cc
//  drafter
//
//  Created by Thomas Jandecka on 11/07/19.
//  Copyright (c) 2019 Apiary Inc. All rights reserved.
//

#include "MsonOneOfSectionToApie.h"

#include "ConversionContext.h"
#include "MsonTypeSectionToApie.h"
#include "refract/Element.h"

using namespace refract;
using namespace drafter;

namespace
{
    std::unique_ptr<OptionElement> WrapOption(std::unique_ptr<IElement> el)
    {
        if (el)
            return make_element<OptionElement>(std::move(el));
        return nullptr;
    }
}

namespace
{
    std::unique_ptr<OptionElement> MsonOneOfEntryToElements(
        const mson::Element::Empty&, const snowcrash::SourceMap<mson::Element>*, ConversionContext&)
    {
        throw snowcrash::Error("unknown type of mson element", snowcrash::ApplicationError);
    }

    std::unique_ptr<OptionElement> MsonOneOfEntryToElements(const mson::Element::PropertyMemberSection& section,
        const snowcrash::SourceMap<mson::Element>* sourceMap,
        ConversionContext& context)
    {
        return WrapOption(MsonTypeSectionToApie(section, sourceMap, context, mson::StringTypeName));
    }

    std::unique_ptr<OptionElement> MsonOneOfEntryToElements(const mson::Element::ValueMemberSection& section,
        const snowcrash::SourceMap<mson::Element>* sourceMap,
        ConversionContext& context)
    {
        return WrapOption(MsonTypeSectionToApie(section, sourceMap, context, mson::StringTypeName));
    }

    std::unique_ptr<OptionElement> MsonOneOfEntryToElements(const mson::Element::MixinSection& section,
        const snowcrash::SourceMap<mson::Element>* sourceMap,
        ConversionContext& context)
    {
        return WrapOption(MsonTypeSectionToApie(section, sourceMap, context, mson::StringTypeName));
    }

    std::unique_ptr<OptionElement> MsonOneOfEntryToElements(const mson::Element::OneOfSection& section,
        const snowcrash::SourceMap<mson::Element>* sourceMap,
        ConversionContext& context)
    {
        return WrapOption(MsonTypeSectionToApie(section, sourceMap, context, mson::StringTypeName));
    }

    std::unique_ptr<OptionElement> MsonOneOfEntryToElements(const mson::Element::GroupSection& section,
        const snowcrash::SourceMap<mson::Element>* sourceMap,
        ConversionContext& context)
    {
        auto option = make_element<OptionElement>();
        MsonTypeSectionsToApie( //
            MakeNodeInfo(*section, sourceMap->elements()),
            option->get(),
            context);
        return option;
    }

    struct MsonOneOfEntryToElementsLambda {
        const snowcrash::SourceMap<mson::Element>* sourceMap;
        ConversionContext& context;
        SelectElement& select;

        template <typename Section>
        void operator()(const Section& s) const
        {
            if (auto option = MsonOneOfEntryToElements(s, sourceMap, context))
                select.get().push_back(std::move(option));
            else
                select.get().push_back(make_element<OptionElement>()); // TODO see mson/issue-699
        }
    };
}

std::unique_ptr<refract::IElement> drafter::MsonOneOfSectionToApie( //
    const mson::OneOf& section,
    const snowcrash::SourceMap<mson::OneOf>* sourceMap,
    ConversionContext& context)
{
    auto result = section.empty() ? make_empty<SelectElement>() : make_element<SelectElement>();

    NodeInfoCollection<mson::OneOf> oneOfNodeInfo(MakeNodeInfo(&section, sourceMap));

    for (const auto& oneOfInfo : oneOfNodeInfo) {
        oneOfInfo.node->visit(MsonOneOfEntryToElementsLambda{ oneOfInfo.sourceMap, context, *result });
    }

    return std::move(result);
}
