#include "catch.hpp"

#include "refract/Visitors.h"
#include "refract/Element.h"

#include "RefractElementFactory.h"


using namespace refract;
using namespace drafter;

TEST_CASE("Create empty primitive element","[ElementFactory]") {
    RefractElementFactory& factory = FactoryFromType(mson::StringTypeName);
    IElement* e = factory.Create(std::string(), false);

    StringElement* str = TypeQueryVisitor::as<StringElement>(e);
    REQUIRE(str != NULL);
    REQUIRE(str->empty());
    REQUIRE(str->meta.empty());
    REQUIRE(str->attributes.empty());
    REQUIRE(str->element() == StringElement::TraitType::element());

    delete e;
}

TEST_CASE("Create primitive element w/ value","[ElementFactory]") {
    RefractElementFactory& factory = FactoryFromType(mson::NumberTypeName);
    IElement* e = factory.Create("42", false);

    NumberElement* number = TypeQueryVisitor::as<NumberElement>(e);
    REQUIRE(number != NULL);
    REQUIRE(!number->empty());
    REQUIRE(number->meta.empty());
    REQUIRE(number->attributes.empty());
    REQUIRE(number->value == 42);

    delete e;
}

TEST_CASE("Create primitive element w/ sample","[ElementFactory]") {
    RefractElementFactory& factory = FactoryFromType(mson::NumberTypeName);
    IElement* e = factory.Create("42", true);

    NumberElement* number = TypeQueryVisitor::as<NumberElement>(e);
    REQUIRE(number != NULL);
    REQUIRE(number->empty());
    REQUIRE(number->meta.empty());

    REQUIRE(number->attributes.size() == 1); // sample attr
    IElement::MemberElementCollection::const_iterator it = number->attributes.find("samples");
    REQUIRE((it != number->attributes.end()));

    delete e;
}

TEST_CASE("Create empty complex element","[ElementFactory]") {
    RefractElementFactory& factory = FactoryFromType(mson::EnumTypeName);
    IElement* e = factory.Create(std::string(), false);

    EnumElement* enm = TypeQueryVisitor::as<EnumElement>(e);
    REQUIRE(enm != NULL);
    REQUIRE(enm->empty());
    REQUIRE(enm->meta.empty());
    REQUIRE(enm->attributes.empty());
    REQUIRE(enm->element() == EnumElement::TraitType::element());

    delete e;
}

TEST_CASE("Create element as generic","[ElementFactory]") {
    RefractElementFactory& factory = FactoryFromType(mson::EnumTypeName);
    IElement* e = factory.Create("Enumerator", true);

    StringElement* generic = TypeQueryVisitor::as<StringElement>(e);
    REQUIRE(generic != NULL);
    REQUIRE(!generic->empty());
    REQUIRE(generic->meta.empty());
    REQUIRE(generic->attributes.empty());
    REQUIRE(generic->element() == "generic");
    REQUIRE(generic->value == "Enumerator");

    delete e;
}
