//
//  MsonTypeSectionToApie.cc
//  drafter
//
//  Created by Thomas Jandecka on 11/07/19.
//  Copyright (c) 2019 Apiary Inc. All rights reserved.
//

#include "MsonTypeSectionToApie.h"

#include "ConversionContext.h"
#include "MsonMemberToApie.h"
#include "MsonOneOfSectionToApie.h"
#include "SerializeKey.h"
#include "SourceAnnotation.h" // snowcrash::Error
#include "refract/Element.h"
#include "refract/TypeQueryVisitor.h" // NamedTypeFromElement

using namespace refract;
using namespace drafter;

namespace
{
    // OPTIM @tjanc@ suspicious switch after visit
    mson::BaseTypeName NamedTypeFromElement(const IElement& element)
    {
        TypeQueryVisitor type;
        Visit(type, element);

        switch (type.get()) {
            case TypeQueryVisitor::Boolean:
                return mson::BooleanTypeName;

            case TypeQueryVisitor::Number:
                return mson::NumberTypeName;

            case TypeQueryVisitor::String:
                return mson::StringTypeName;

            case TypeQueryVisitor::Array:
                return mson::ArrayTypeName;

            case TypeQueryVisitor::Enum:
                return mson::EnumTypeName;

            case TypeQueryVisitor::Object:
                return mson::ObjectTypeName;

            default:
                return mson::UndefinedTypeName;
        }

        return mson::UndefinedTypeName;
    }
}

mson::BaseTypeName drafter::ResolveType(const mson::TypeSpecification& spec, ConversionContext& context)
{
    // OPTIM @tjanc@ make nameType const
    mson::BaseTypeName nameType = spec.name.base;
    const std::string& parent = spec.name.symbol.literal;

    if (nameType == mson::UndefinedTypeName && !parent.empty()) {
        const IElement* base = FindRootAncestor(parent, context.GetNamedTypesRegistry());
        if (base) {
            nameType = NamedTypeFromElement(*base);
        }
    }

    return nameType;
}

std::unique_ptr<IElement> drafter::MsonTypeSectionToApie( //
    const mson::MemberType::Empty&,
    const snowcrash::SourceMap<mson::MemberType>*,
    ConversionContext&,
    const mson::BaseTypeName)
{
    // OPTIM @tjanc@ avoid throwing exceptions
    throw snowcrash::Error("unknown type of mson element", snowcrash::ApplicationError);
}

std::unique_ptr<IElement> drafter::MsonTypeSectionToApie( //
    const mson::MemberType::PropertyMemberSection& section,
    const snowcrash::SourceMap<mson::MemberType>* sourceMap,
    ConversionContext& context,
    const mson::BaseTypeName defaultNestedType)
{
    return MsonMemberToApie( //
        MakeNodeInfo(section, sourceMap->property),
        context,
        ResolveType(section.valueDefinition.typeDefinition.typeSpecification, context),
        defaultNestedType);
}

std::unique_ptr<IElement> drafter::MsonTypeSectionToApie( //
    const mson::MemberType::ValueMemberSection& section,
    const snowcrash::SourceMap<mson::MemberType>* sourceMap,
    ConversionContext& context,
    const mson::BaseTypeName defaultNestedType)
{
    return MsonMemberToApie( //
        MakeNodeInfo(section, sourceMap->value),
        context,
        ResolveType(section.valueDefinition.typeDefinition.typeSpecification, context),
        defaultNestedType);
}

std::unique_ptr<IElement> drafter::MsonTypeSectionToApie( //
    const mson::MemberType::MixinSection& section,
    const snowcrash::SourceMap<mson::MemberType>* sourceMap,
    ConversionContext&,
    const mson::BaseTypeName)
{
    auto ref = make_element<RefElement>(section.typeSpecification.name.symbol.literal);
    ref->attributes().set(SerializeKey::Path, from_primitive(SerializeKey::Content));
    return std::move(ref);
}

std::unique_ptr<IElement> drafter::MsonTypeSectionToApie( //
    const mson::MemberType::OneOfSection& section,
    const snowcrash::SourceMap<mson::MemberType>* sourceMap,
    ConversionContext& context,
    const mson::BaseTypeName);

std::unique_ptr<IElement> drafter::MsonTypeSectionToApie( //
    const mson::MemberType::OneOfSection& section,
    const snowcrash::SourceMap<mson::MemberType>* sourceMap,
    ConversionContext& context,
    const mson::BaseTypeName)
{
    return MsonOneOfSectionToApie(*section, sourceMap ? &sourceMap->oneOf() : nullptr, context);
}

std::unique_ptr<IElement> drafter::MsonTypeSectionToApie( //
    const mson::MemberType::GroupSection&,
    const snowcrash::SourceMap<mson::MemberType>*,
    ConversionContext& context,
    const mson::BaseTypeName)
{
    // TODO avoid throwing exceptions...
    throw snowcrash::Error("unable to handle element group", snowcrash::ApplicationError);
}

struct MsonTypeSectionToElementsLambda {
    std::unique_ptr<IElement>& result;
    const snowcrash::SourceMap<mson::MemberType>* sourceMap;
    ConversionContext& context;
    mson::BaseTypeName defaultNestedType;

    template <typename Section>
    void operator()(const Section& s)
    {
        result = MsonTypeSectionToApie(s, sourceMap, context, defaultNestedType);
    }
};

std::unique_ptr<IElement> drafter::MsonTypeSectionToApie( //
    const mson::MemberType& element,
    const snowcrash::SourceMap<mson::MemberType>* sourceMap,
    ConversionContext& context,
    const mson::BaseTypeName defaultNestedType)
{
    std::unique_ptr<IElement> result;
    element.visit(MsonTypeSectionToElementsLambda{ result, sourceMap, context, defaultNestedType });
    return result;
}
