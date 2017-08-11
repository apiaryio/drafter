#include "catch.hpp"

#include "Element.h"
#include "TypeQueryVisitor.h"

#include "RefractElementFactory.h"

using namespace refract;
using namespace drafter;

TEST_CASE("Create empty primitive element", "[ElementFactory]")
{
    const RefractElementFactory& factory = FactoryFromType(mson::StringTypeName);
    IElement* e = factory.Create(std::string(), eValue);

    StringElement* str = TypeQueryVisitor::as<StringElement>(e);
    REQUIRE(str != NULL);
    REQUIRE(str->empty());
    REQUIRE(str->meta.empty());
    REQUIRE(str->attributes.empty());
    REQUIRE(str->element() == StringElement::TraitType::element());

    delete e;
}

TEST_CASE("Create primitive element w/ value", "[ElementFactory]")
{
    const RefractElementFactory& factory = FactoryFromType(mson::NumberTypeName);
    IElement* e = factory.Create("42", eValue);

    NumberElement* number = TypeQueryVisitor::as<NumberElement>(e);
    REQUIRE(number != NULL);
    REQUIRE(!number->empty());
    REQUIRE(number->meta.empty());
    REQUIRE(number->attributes.empty());
    REQUIRE(number->value == 42);

    delete e;
}

TEST_CASE("Create primitive element w/ sample", "[ElementFactory]")
{
    const RefractElementFactory& factory = FactoryFromType(mson::NumberTypeName);
    IElement* e = factory.Create("42", eSample);

    NumberElement* number = TypeQueryVisitor::as<NumberElement>(e);
    REQUIRE(number != NULL);
    REQUIRE(number->empty());
    REQUIRE(number->meta.empty());

    REQUIRE(number->attributes.size() == 1); // sample attr
    IElement::MemberElementCollection::const_iterator it = number->attributes.find("samples");
    REQUIRE((it != number->attributes.end()));

    delete e;
}

TEST_CASE("Create primitive element w/ element", "[ElementFactory]")
{
    const RefractElementFactory& factory = FactoryFromType(mson::NumberTypeName);
    IElement* e = factory.Create("NAMED", eElement);

    NumberElement* number = TypeQueryVisitor::as<NumberElement>(e);
    REQUIRE(number != NULL);
    REQUIRE(number->empty());
    REQUIRE(number->meta.empty());
    REQUIRE(number->attributes.empty());
    REQUIRE(number->element() == "NAMED");

    delete e;
}

TEST_CASE("Create empty complex element", "[ElementFactory]")
{
    const RefractElementFactory& factory = FactoryFromType(mson::EnumTypeName);
    IElement* e = factory.Create(std::string(), eValue);

    EnumElement* enm = TypeQueryVisitor::as<EnumElement>(e);
    REQUIRE(enm != NULL);
    REQUIRE(enm->empty());
    REQUIRE(enm->meta.empty());
    REQUIRE(enm->attributes.empty());
    REQUIRE(enm->element() == EnumElement::TraitType::element());

    delete e;
}

TEST_CASE("Create complex named element", "[ElementFactory]")
{
    const RefractElementFactory& factory = FactoryFromType(mson::ObjectTypeName);
    IElement* e = factory.Create("NAMED", eElement);

    ObjectElement* enm = TypeQueryVisitor::as<ObjectElement>(e);
    REQUIRE(enm != NULL);
    REQUIRE(enm->empty());
    REQUIRE(enm->meta.empty());
    REQUIRE(enm->attributes.empty());
    REQUIRE(enm->element() == "NAMED");

    delete e;
}

TEST_CASE("Create element as generic", "[ElementFactory]")
{
    const RefractElementFactory& factory = FactoryFromType(mson::EnumTypeName);
    IElement* e = factory.Create("Enumerator", eSample);

    StringElement* generic = TypeQueryVisitor::as<StringElement>(e);
    REQUIRE(generic != NULL);
    REQUIRE(!generic->empty());
    REQUIRE(generic->meta.empty());
    REQUIRE(generic->attributes.empty());
    REQUIRE(generic->element() == "generic");
    REQUIRE(generic->value == "Enumerator");

    delete e;
}
