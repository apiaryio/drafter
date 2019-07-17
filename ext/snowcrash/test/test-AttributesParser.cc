//
//  test-AttributesParser.cc
//  snowcrash
//
//  Created by Pavan Kumar Sunkara on 11/25/14.
//  Copyright (c) 2014 Apiary Inc. All rights reserved.
//

#include "snowcrashtest.h"
#include "AttributesParser.h"

using namespace snowcrash;
using namespace snowcrashtest;

const mdp::ByteBuffer AttributesFixture = "+ Attributes (array[[Coupon](#coupon)])";

TEST_CASE("Recognize explicit attributes signature", "[attributes]")
{
    mdp::MarkdownParser markdownParser;
    mdp::MarkdownNode markdownAST;
    SectionType sectionType;
    markdownParser.parse(AttributesFixture, markdownAST);

    REQUIRE(!markdownAST.children().empty());
    sectionType = SectionProcessor<Attributes>::sectionType(markdownAST.children().begin());
    REQUIRE(sectionType == AttributesSectionType);
}

TEST_CASE("Parse canonical attributes", "[attributes]")
{
    ParseResult<Attributes> attributes;
    SectionParserHelper<Attributes, AttributesParser>::parse(
        AttributesFixture, AttributesSectionType, attributes, ExportSourcemapOption);

    REQUIRE(attributes.report.error.code == Error::OK);
    REQUIRE(attributes.report.warnings.empty());

    REQUIRE(attributes.node.name.empty());
    REQUIRE(attributes.node.sections.empty());
    REQUIRE(attributes.node.typeDefinition.attributes == 0);
    REQUIRE(attributes.node.typeDefinition.typeSpecification.name.base == mson::ArrayTypeName);
    REQUIRE(attributes.node.typeDefinition.typeSpecification.nestedTypes.size() == 1);
    REQUIRE(attributes.node.typeDefinition.typeSpecification.nestedTypes[0].symbol.literal == "Coupon");
    REQUIRE(attributes.node.typeDefinition.baseType == mson::ValueBaseType);

    REQUIRE(attributes.sourceMap.name.sourceMap.empty());
    SourceMapHelper::check(attributes.sourceMap.typeDefinition.sourceMap, 2, 37);
    REQUIRE(attributes.sourceMap.sections.collection.empty());
}

TEST_CASE("Parse attributes with nested members", "[attributes]")
{
    mdp::ByteBuffer source
        = "+ attribute\n"
          "    + message (string) - The blog post article\n"
          "    + author: john@appleseed.com (string) - Author of the blog post";

    ParseResult<Attributes> attributes;
    SectionParserHelper<Attributes, AttributesParser>::parse(
        source, AttributesSectionType, attributes, ExportSourcemapOption);

    REQUIRE(attributes.report.error.code == Error::OK);
    REQUIRE(attributes.report.warnings.empty());

    REQUIRE(attributes.node.name.empty());
    REQUIRE(attributes.node.typeDefinition.empty());
    REQUIRE(attributes.node.typeDefinition.baseType == mson::ImplicitObjectBaseType);
    REQUIRE(attributes.node.sections.size() == 1);
    REQUIRE(attributes.node.sections[0].klass == mson::TypeSection::MemberTypeClass);
    REQUIRE(attributes.node.sections[0].content.elements().size() == 2);

    {
        const auto* property = attributes.node.sections[0].content.elements().at(0).property();
        REQUIRE(property);
        REQUIRE(property->name.literal == "message");
        REQUIRE(property->description == "The blog post article");
        REQUIRE(property->valueDefinition.values.empty());
        REQUIRE(property->valueDefinition.typeDefinition.typeSpecification.name.base == mson::StringTypeName);
    }

    {
        const auto* property = attributes.node.sections[0].content.elements().at(1).property();
        REQUIRE(property);
        REQUIRE(property->name.literal == "author");
        REQUIRE(property->description == "Author of the blog post");
        REQUIRE(property->valueDefinition.values.size() == 1);
        REQUIRE(property->valueDefinition.values[0].literal == "john@appleseed.com");
        REQUIRE(property->valueDefinition.typeDefinition.typeSpecification.name.base == mson::StringTypeName);
    }

    REQUIRE(attributes.sourceMap.name.sourceMap.empty());
    REQUIRE(attributes.sourceMap.typeDefinition.sourceMap.empty());
    REQUIRE(attributes.sourceMap.sections.collection.size() == 1);
    REQUIRE(attributes.sourceMap.sections.collection[0].elements().collection.size() == 2);

    SourceMap<mson::MemberType> elementSM = attributes.sourceMap.sections.collection[0].elements().collection[0];
    SourceMapHelper::check(elementSM.property.name.sourceMap, 18, 41);
    SourceMapHelper::check(elementSM.property.description.sourceMap, 18, 41);
    SourceMapHelper::check(elementSM.property.valueDefinition.sourceMap, 18, 41);

    elementSM = attributes.sourceMap.sections.collection[0].elements().collection[1];
    SourceMapHelper::check(elementSM.property.name.sourceMap, 65, 61);
    SourceMapHelper::check(elementSM.property.description.sourceMap, 65, 61);
    SourceMapHelper::check(elementSM.property.valueDefinition.sourceMap, 65, 61);
}

TEST_CASE("Parse attributes with block description", "[attributes]")
{
    mdp::ByteBuffer source
        = "+ Attributes\n"
          "    Awesome description\n\n"
          "    + With list\n"
          "    + Properties\n"
          "        + message (string)";

    ParseResult<Attributes> attributes;
    SectionParserHelper<Attributes, AttributesParser>::parse(
        source, AttributesSectionType, attributes, ExportSourcemapOption);

    REQUIRE(attributes.report.error.code == Error::OK);
    REQUIRE(attributes.report.warnings.empty());

    REQUIRE(attributes.node.name.empty());
    REQUIRE(attributes.node.typeDefinition.empty());
    REQUIRE(attributes.node.typeDefinition.baseType == mson::ImplicitObjectBaseType);
    REQUIRE(attributes.node.sections.size() == 2);
    REQUIRE(attributes.node.sections[0].klass == mson::TypeSection::BlockDescriptionClass);
    REQUIRE(attributes.node.sections[0].content.description == "Awesome description\n\n+ With list");
    REQUIRE(attributes.node.sections[1].klass == mson::TypeSection::MemberTypeClass);
    REQUIRE(attributes.node.sections[1].content.elements().size() == 1);

    REQUIRE(attributes.sourceMap.name.sourceMap.empty());
    REQUIRE(attributes.sourceMap.typeDefinition.sourceMap.empty());
    REQUIRE(attributes.sourceMap.sections.collection.size() == 2);
    SourceMapHelper::check(attributes.sourceMap.sections.collection[0].description.sourceMap, 2, 11, 1);
    SourceMapHelper::check(attributes.sourceMap.sections.collection[0].description.sourceMap, 17, 20, 2);
    SourceMapHelper::check(attributes.sourceMap.sections.collection[0].description.sourceMap, 42, 12, 3);
    REQUIRE(attributes.sourceMap.sections.collection[1].elements().collection.size() == 1);
}

TEST_CASE("Parse type attributes on attributes section", "[attributes]")
{
    mdp::ByteBuffer source
        = "+ Attributes (fixed)\n"
          "    + a: b";

    ParseResult<Attributes> attributes;
    SectionParserHelper<Attributes, AttributesParser>::parse(
        source, AttributesSectionType, attributes, ExportSourcemapOption);

    REQUIRE(attributes.report.error.code == Error::OK);
    REQUIRE(attributes.report.warnings.empty());

    REQUIRE(attributes.node.name.empty());
    REQUIRE(attributes.node.typeDefinition.attributes == 4);
    REQUIRE(attributes.node.typeDefinition.typeSpecification.name.empty());
    REQUIRE(attributes.node.typeDefinition.baseType == mson::ImplicitObjectBaseType);
    REQUIRE(attributes.node.sections.size() == 1);
    REQUIRE(attributes.node.sections[0].klass == mson::TypeSection::MemberTypeClass);
}
