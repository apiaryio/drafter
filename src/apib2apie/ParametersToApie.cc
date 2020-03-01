//
//  ParametersToApie.cc
//  apib2apie
//
//  Created by Thomas Jandecka on 25/02/20.
//  Copyright (c) 2020 Apiary Inc. All rights reserved.
//
//  Note @tjanc: moved from RefractAPI.cc

#include "ParametersToApie.h"

#include <SourceAnnotation.h>

#include "../RefractDataStructure.h"
#include "../Render.h"
#include "../RefractSourceMap.h"

#include "../ConversionContext.h"

#include "CollectionToApie.h"

using namespace drafter;
using namespace refract;
using namespace apib2apie;

namespace
{
    std::unique_ptr<IElement> ParameterValuesToRefract(
        const NodeInfo<snowcrash::Parameter>& parameter, ConversionContext& context)
    {
        // Add sample value
        auto element = parameter.node->exampleValue.empty() ? //
            make_empty<EnumElement>() :
            make_element<EnumElement>(LiteralToRefract(MAKE_NODE_INFO(parameter, exampleValue), context));

        // Add default value
        if (!parameter.node->defaultValue.empty()) {
            element->attributes().set(SerializeKey::Default,
                make_element<EnumElement>(LiteralToRefract(MAKE_NODE_INFO(parameter, defaultValue), context)));
        }

        // Add enumerations
        element->attributes().set(SerializeKey::Enumerations,
            CollectionToApie<ArrayElement>(MAKE_NODE_INFO(parameter, values), context, LiteralToRefract));

        return std::move(element);
    }

    // NOTE: We removed type specific templates from here in https://github.com/apiaryio/drafter/pull/447
    std::unique_ptr<IElement> ExtractParameter(
        const NodeInfo<snowcrash::Parameter>& parameter, ConversionContext& context)
    {
        std::unique_ptr<IElement> element = nullptr;

        if (parameter.node->values.empty()) {
            auto element = parameter.node->exampleValue.empty() ? //
                make_empty<StringElement>() :
                LiteralToRefract(MAKE_NODE_INFO(parameter, exampleValue), context);

            if (!parameter.node->defaultValue.empty()) {
                element->attributes().set(
                    SerializeKey::Default, PrimitiveToRefract(MAKE_NODE_INFO(parameter, defaultValue)));
            }

            return std::move(element);
        } else {
            return ParameterValuesToRefract(parameter, context);
        }
    }

    std::unique_ptr<IElement> ParameterToRefract(
        const NodeInfo<snowcrash::Parameter>& parameter, ConversionContext& context)
    {
        auto element = make_element<MemberElement>(
            PrimitiveToRefract(MAKE_NODE_INFO(parameter, name)), ExtractParameter(parameter, context));

        // Description
        if (!parameter.node->description.empty()) {
            element->meta().set(SerializeKey::Description, PrimitiveToRefract(MAKE_NODE_INFO(parameter, description)));
        }

        if (!parameter.node->type.empty()) {
            element->meta().set(SerializeKey::Title, PrimitiveToRefract(MAKE_NODE_INFO(parameter, type)));
        }

        // Parameter use
        {

            std::string use = SerializeKey::Required;

            if (parameter.node->use == snowcrash::OptionalParameterUse) {
                use = SerializeKey::Optional;
            }

            element->attributes().set(SerializeKey::TypeAttributes, make_element<ArrayElement>(from_primitive(use)));
        }

        return std::move(element);
    }
}

std::unique_ptr<IElement> apib2apie::ParametersToApie( //
    const NodeInfo<snowcrash::Parameters>& parameters, //
    ConversionContext& context)
{
    return CollectionToApie<ObjectElement>(parameters, context, ParameterToRefract, SerializeKey::HrefVariables);
}
