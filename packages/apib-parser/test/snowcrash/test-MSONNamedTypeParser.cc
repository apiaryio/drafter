//
//  test-MSONNamedTypeParser.cc
//  snowcrash
//
//  Created by Pavan Kumar Sunkara on 11/5/14.
//  Copyright (c) 2014 Apiary Inc. All rights reserved.
//

#include "snowcrashtest.h"
#include "MSONNamedTypeParser.h"

using namespace snowcrash;
using namespace snowcrashtest;

TEST_CASE("Parse canonical mson named type", "[mson][named_type]")
{
    mdp::ByteBuffer source
        = "# User (object)\n"
          "- login: pksunkara (string)\n"
          "- name: Pavan Kumar Sunkara (string, optional)\n"
          "- admin: false (boolean, default)\n"
          "- orgs: confy, apiary (array)\n"
          "- plan (object)\n"
          "    The subscription plan of the user\n"
          "    - Properties\n"
          "        - stripe: 1284 (number)\n"
          "        - name: Medium (string)\n";

    ParseResult<mson::NamedType> namedType;
    SectionParserHelper<mson::NamedType, MSONNamedTypeParser>::parse(
        source, MSONNamedTypeSectionType, namedType, ExportSourcemapOption);

    REQUIRE(namedType.report.error.code == Error::OK);
    REQUIRE(namedType.report.warnings.empty());

    REQUIRE(namedType.node.name.symbol.literal == "User");
    REQUIRE(namedType.node.name.base == mson::UndefinedTypeName);
    REQUIRE(namedType.node.typeDefinition.attributes == 0);
    REQUIRE(namedType.node.typeDefinition.typeSpecification.name.base == mson::ObjectTypeName);
    REQUIRE(namedType.node.typeDefinition.typeSpecification.name.symbol.empty());
    REQUIRE(namedType.node.typeDefinition.baseType == mson::ObjectBaseType);
    REQUIRE(namedType.node.sections.size() == 1);
    REQUIRE(namedType.node.sections[0].klass == mson::TypeSection::MemberTypeClass);
    REQUIRE(namedType.node.sections[0].content.elements().size() == 5);

    {
        const auto* element = namedType.node.sections[0].content.elements().at(0).property();
        REQUIRE(element);
        REQUIRE(element->name.literal == "login");
        REQUIRE(element->valueDefinition.values.size() == 1);
        REQUIRE(element->valueDefinition.values[0].literal == "pksunkara");
        REQUIRE(element->valueDefinition.typeDefinition.baseType == mson::PrimitiveBaseType);
        REQUIRE(element->valueDefinition.typeDefinition.typeSpecification.name.base == mson::StringTypeName);
        REQUIRE(element->sections.empty());
    }

    {
        const auto* element = namedType.node.sections[0].content.elements().at(1).property();
        REQUIRE(element);
        REQUIRE(element->name.literal == "name");
        REQUIRE(element->valueDefinition.values.size() == 1);
        REQUIRE(element->valueDefinition.values[0].literal == "Pavan Kumar Sunkara");
        REQUIRE(element->valueDefinition.typeDefinition.baseType == mson::PrimitiveBaseType);
        REQUIRE(element->valueDefinition.typeDefinition.typeSpecification.name.base == mson::StringTypeName);
        REQUIRE(element->sections.empty());
    }

    {
        const auto* element = namedType.node.sections[0].content.elements().at(2).property();
        REQUIRE(element);
        REQUIRE(element->name.literal == "admin");
        REQUIRE(element->valueDefinition.values.size() == 1);
        REQUIRE(element->valueDefinition.values[0].literal == "false");
        REQUIRE(element->valueDefinition.typeDefinition.baseType == mson::PrimitiveBaseType);
        REQUIRE(element->valueDefinition.typeDefinition.typeSpecification.name.base == mson::BooleanTypeName);
        REQUIRE(element->sections.empty());
    }

    {
        const auto* element = namedType.node.sections[0].content.elements().at(3).property();
        REQUIRE(element);
        REQUIRE(element->name.literal == "orgs");
        REQUIRE(element->valueDefinition.values.size() == 2);
        REQUIRE(element->valueDefinition.values[0].literal == "confy");
        REQUIRE(element->valueDefinition.values[1].literal == "apiary");
        REQUIRE(element->valueDefinition.typeDefinition.baseType == mson::ValueBaseType);
        REQUIRE(element->valueDefinition.typeDefinition.typeSpecification.name.base == mson::ArrayTypeName);
        REQUIRE(element->sections.empty());
    }

    {
        const auto* element = namedType.node.sections[0].content.elements().at(4).property();
        REQUIRE(element);
        REQUIRE(element->name.literal == "plan");
        REQUIRE(element->valueDefinition.values.empty());
        REQUIRE(element->valueDefinition.typeDefinition.baseType == mson::ObjectBaseType);
        REQUIRE(element->valueDefinition.typeDefinition.typeSpecification.name.base == mson::ObjectTypeName);
        REQUIRE(element->sections.size() == 2);
        REQUIRE(element->sections[0].klass == mson::TypeSection::BlockDescriptionClass);
        REQUIRE(element->sections[0].content.description == "The subscription plan of the user");
        REQUIRE(element->sections[1].klass == mson::TypeSection::MemberTypeClass);
        REQUIRE(element->sections[1].content.elements().size() == 2);

        {
            const auto* subelement = element->sections[1].content.elements().at(0).property();
            REQUIRE(subelement);
            REQUIRE(subelement->name.literal == "stripe");
            REQUIRE(subelement->valueDefinition.typeDefinition.typeSpecification.name.base == mson::NumberTypeName);
            REQUIRE(subelement->valueDefinition.values.size() == 1);
            REQUIRE(subelement->valueDefinition.values[0].literal == "1284");
        }

        {
            const auto* subelement = element->sections[1].content.elements().at(1).property();
            REQUIRE(subelement);
            REQUIRE(subelement->name.literal == "name");
            REQUIRE(subelement->valueDefinition.typeDefinition.typeSpecification.name.base == mson::StringTypeName);
            REQUIRE(subelement->valueDefinition.values.size() == 1);
            REQUIRE(subelement->valueDefinition.values[0].literal == "Medium");
        }
    }

    SourceMapHelper::check(namedType.sourceMap.name.sourceMap, 0, 16);
    SourceMapHelper::check(namedType.sourceMap.typeDefinition.sourceMap, 0, 16);
    REQUIRE(namedType.sourceMap.sections.collection.size() == 1);
    REQUIRE(namedType.sourceMap.sections.collection[0].elements().collection.size() == 5);

    {
        const auto& elementSM = namedType.sourceMap.sections.collection[0].elements().collection[0];
        SourceMapHelper::check(elementSM.property.name.sourceMap, 18, 26);
        SourceMapHelper::check(elementSM.property.valueDefinition.sourceMap, 18, 26);
        REQUIRE(elementSM.property.sections.collection.empty());
    }

    {
        const auto& elementSM = namedType.sourceMap.sections.collection[0].elements().collection[1];
        SourceMapHelper::check(elementSM.property.name.sourceMap, 46, 45);
        SourceMapHelper::check(elementSM.property.valueDefinition.sourceMap, 46, 45);
        REQUIRE(elementSM.property.sections.collection.empty());
    }

    {
        const auto& elementSM = namedType.sourceMap.sections.collection[0].elements().collection[2];
        SourceMapHelper::check(elementSM.property.name.sourceMap, 93, 32);
        SourceMapHelper::check(elementSM.property.valueDefinition.sourceMap, 93, 32);
        REQUIRE(elementSM.property.sections.collection.empty());
    }

    {
        const auto& elementSM = namedType.sourceMap.sections.collection[0].elements().collection[3];
        SourceMapHelper::check(elementSM.property.name.sourceMap, 127, 28);
        SourceMapHelper::check(elementSM.property.valueDefinition.sourceMap, 127, 28);
        REQUIRE(elementSM.property.sections.collection.empty());
    }

    {
        const auto& elementSM = namedType.sourceMap.sections.collection[0].elements().collection[4];
        SourceMapHelper::check(elementSM.property.name.sourceMap, 157, 14, 175, 34);
        SourceMapHelper::check(elementSM.property.valueDefinition.sourceMap, 157, 14, 175, 34);
        REQUIRE(elementSM.property.sections.collection.size() == 2);
        SourceMapHelper::check(elementSM.property.sections.collection[0].description.sourceMap, 157, 14, 175, 34);
        REQUIRE(elementSM.property.sections.collection[1].elements().collection.size() == 2);

        {
            const auto& subelementSM = elementSM.property.sections.collection[1].elements().collection[0];
            SourceMapHelper::check(subelementSM.property.name.sourceMap, 236, 22);
            SourceMapHelper::check(subelementSM.property.valueDefinition.sourceMap, 236, 22);
            REQUIRE(subelementSM.property.sections.collection.empty());
        }

        {
            const auto& subelementSM = elementSM.property.sections.collection[1].elements().collection[1];
            SourceMapHelper::check(subelementSM.property.name.sourceMap, 268, 22);
            SourceMapHelper::check(subelementSM.property.valueDefinition.sourceMap, 268, 22);
            REQUIRE(subelementSM.property.sections.collection.empty());
        }
    }
}

TEST_CASE("Parse named type with a type section", "[mson][named_type]")
{
    mdp::ByteBuffer source
        = "# User (string)\n"
          "## Sample: pksunkara";

    ParseResult<mson::NamedType> namedType;
    SectionParserHelper<mson::NamedType, MSONNamedTypeParser>::parse(
        source, MSONNamedTypeSectionType, namedType, ExportSourcemapOption);

    REQUIRE(namedType.report.error.code == Error::OK);
    REQUIRE(namedType.report.warnings.empty());

    REQUIRE(namedType.node.name.symbol.literal == "User");
    REQUIRE(namedType.node.name.base == mson::UndefinedTypeName);
    REQUIRE(namedType.node.typeDefinition.attributes == 0);
    REQUIRE(namedType.node.typeDefinition.typeSpecification.name.base == mson::StringTypeName);
    REQUIRE(namedType.node.typeDefinition.typeSpecification.name.symbol.empty());
    REQUIRE(namedType.node.typeDefinition.baseType == mson::PrimitiveBaseType);
    REQUIRE(namedType.node.sections.size() == 1);
    REQUIRE(namedType.node.sections[0].klass == mson::TypeSection::SampleClass);
    REQUIRE(namedType.node.sections[0].content.value == "pksunkara");

    SourceMapHelper::check(namedType.sourceMap.name.sourceMap, 0, 16);
    SourceMapHelper::check(namedType.sourceMap.typeDefinition.sourceMap, 0, 16);
    REQUIRE(namedType.sourceMap.sections.collection.size() == 1);
    SourceMapHelper::check(namedType.sourceMap.sections.collection[0].value.sourceMap, 16, 20);
}

TEST_CASE("Parse named type without type specification", "[mson][named_type]")
{
    mdp::ByteBuffer source
        = "# User\n"
          "## Sample: pksunkara";

    ParseResult<mson::NamedType> namedType;
    SectionParserHelper<mson::NamedType, MSONNamedTypeParser>::parse(
        source, MSONNamedTypeSectionType, namedType, ExportSourcemapOption);

    REQUIRE(namedType.report.error.code == Error::OK);
    REQUIRE(namedType.report.warnings.size() == 1);
    REQUIRE(namedType.report.warnings[0].code == LogicalErrorWarning);

    REQUIRE(namedType.node.name.symbol.literal == "User");
    REQUIRE(namedType.node.typeDefinition.attributes == 0);
    REQUIRE(namedType.node.typeDefinition.typeSpecification.name.empty());
    REQUIRE(namedType.node.typeDefinition.baseType == mson::ImplicitObjectBaseType);
    REQUIRE(namedType.node.sections.size() == 1);
    REQUIRE(namedType.node.sections[0].klass == mson::TypeSection::SampleClass);
    REQUIRE(namedType.node.sections[0].baseType == mson::ImplicitObjectBaseType);

    SourceMapHelper::check(namedType.sourceMap.name.sourceMap, 0, 7);
    REQUIRE(namedType.sourceMap.typeDefinition.sourceMap.empty());
    REQUIRE(namedType.sourceMap.sections.collection.size() == 1);
    REQUIRE(namedType.sourceMap.sections.collection[0].value.sourceMap.size() == 1);
}

TEST_CASE("Parse type attributes on named type", "[mson][named_type]")
{
    mdp::ByteBuffer source
        = "# User (fixed)\n"
          "+ p1: a\n"
          "+ p2: b\n";

    ParseResult<mson::NamedType> namedType;
    SectionParserHelper<mson::NamedType, MSONNamedTypeParser>::parse(
        source, MSONNamedTypeSectionType, namedType, ExportSourcemapOption);

    REQUIRE(namedType.report.error.code == Error::OK);
    REQUIRE(namedType.report.warnings.empty());

    REQUIRE(namedType.node.name.symbol.literal == "User");
    REQUIRE(namedType.node.typeDefinition.attributes == 4);
    REQUIRE(namedType.node.typeDefinition.typeSpecification.name.empty());
    REQUIRE(namedType.node.typeDefinition.baseType == mson::ImplicitObjectBaseType);
    REQUIRE(namedType.node.sections.size() == 1);
    REQUIRE(namedType.node.sections[0].klass == mson::TypeSection::MemberTypeClass);
}

TEST_CASE("Parse variable named type", "[mson][named_type]")
{
    mdp::ByteBuffer source = "# *User* (string)";

    ParseResult<mson::NamedType> namedType;
    SectionParserHelper<mson::NamedType, MSONNamedTypeParser>::parse(
        source, MSONNamedTypeSectionType, namedType, ExportSourcemapOption);

    REQUIRE(namedType.report.error.code == Error::OK);
    REQUIRE(namedType.report.warnings.empty());

    REQUIRE(namedType.node.name.symbol.literal == "User");
    REQUIRE(namedType.node.name.symbol.variable == true);
}

TEST_CASE("Parse variable named type with reserved characters", "[mson][named_type][335]")
{
    mdp::ByteBuffer source = "# *User (string)*";

    ParseResult<mson::NamedType> namedType;
    SectionParserHelper<mson::NamedType, MSONNamedTypeParser>::parse(
        source, MSONNamedTypeSectionType, namedType, ExportSourcemapOption);

    REQUIRE(namedType.report.error.code == Error::OK);
    REQUIRE(namedType.report.warnings.size() == 1);

    REQUIRE(namedType.node.name.symbol.literal == "User (string)");
    REQUIRE(namedType.node.name.symbol.variable == true);
}

TEST_CASE("Parse variable named type with escaped reserved characters", "[mson][named_type]")
{
    mdp::ByteBuffer source = "# `User-A` (string)";

    ParseResult<mson::NamedType> namedType;
    SectionParserHelper<mson::NamedType, MSONNamedTypeParser>::parse(
        source, MSONNamedTypeSectionType, namedType, ExportSourcemapOption);

    REQUIRE(namedType.report.error.code == Error::OK);
    REQUIRE(namedType.report.warnings.empty());

    REQUIRE(namedType.node.name.symbol.literal == "User-A");
    REQUIRE(namedType.node.name.symbol.variable == false);
}
