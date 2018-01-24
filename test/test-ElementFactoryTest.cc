#include "catch.hpp"

#include "Element.h"
#include "TypeQueryVisitor.h"

#include "RefractElementFactory.h"

using namespace refract;
using namespace drafter;

TEST_CASE("Create empty primitive element", "[ElementFactory]")
{
    const RefractElementFactory& factory = FactoryFromType(mson::StringTypeName);
    auto e = factory.Create(std::string(), eValue);

    StringElement* str = TypeQueryVisitor::as<StringElement>(e.get());
    REQUIRE(str != NULL);
    REQUIRE(str->empty());
    REQUIRE(str->meta().empty());
    REQUIRE(str->attributes().empty());
    REQUIRE(str->element() == StringElement::ValueType::name);
}

TEST_CASE("Create primitive element w/ value", "[ElementFactory]")
{
    const RefractElementFactory& factory = FactoryFromType(mson::NumberTypeName);
    auto e = factory.Create("42", eValue);

    NumberElement* number = TypeQueryVisitor::as<NumberElement>(e.get());
    REQUIRE(number != NULL);
    REQUIRE(!number->empty());
    REQUIRE(number->meta().empty());
    REQUIRE(number->attributes().empty());
    REQUIRE(number->get() == 42.0);
}

TEST_CASE("Create primitive element w/ sample", "[ElementFactory]")
{
    const RefractElementFactory& factory = FactoryFromType(mson::NumberTypeName);
    auto e = factory.Create("42", eSample);

    NumberElement* number = TypeQueryVisitor::as<NumberElement>(e.get());
    REQUIRE(number != NULL);
    REQUIRE(number->empty());
    REQUIRE(number->meta().empty());

    REQUIRE(number->attributes().size() == 1); // sample attr
    auto it = number->attributes().find("samples");
    REQUIRE((it != number->attributes().end()));
}

TEST_CASE("Create primitive element w/ element", "[ElementFactory]")
{
    const RefractElementFactory& factory = FactoryFromType(mson::NumberTypeName);
    auto e = factory.Create("NAMED", eElement);

    NumberElement* number = TypeQueryVisitor::as<NumberElement>(e.get());
    REQUIRE(number != NULL);
    REQUIRE(number->empty());
    REQUIRE(number->meta().empty());
    REQUIRE(number->attributes().empty());
    REQUIRE(number->element() == "NAMED");
}

TEST_CASE("Create empty complex element", "[ElementFactory]")
{
    const RefractElementFactory& factory = FactoryFromType(mson::EnumTypeName);
    auto e = factory.Create(std::string(), eValue);

    EnumElement* enm = TypeQueryVisitor::as<EnumElement>(e.get());
    REQUIRE(enm != NULL);
    REQUIRE(enm->empty());
    REQUIRE(enm->meta().empty());
    REQUIRE(enm->attributes().empty());
    REQUIRE(enm->element() == EnumElement::ValueType::name);
}

TEST_CASE("Create complex named element", "[ElementFactory]")
{
    const RefractElementFactory& factory = FactoryFromType(mson::ObjectTypeName);
    auto e = factory.Create("NAMED", eElement);

    ObjectElement* enm = TypeQueryVisitor::as<ObjectElement>(e.get());
    REQUIRE(enm != NULL);
    REQUIRE(enm->empty());
    REQUIRE(enm->meta().empty());
    REQUIRE(enm->attributes().empty());
    REQUIRE(enm->element() == "NAMED");
}

TEST_CASE("Create element as generic", "[ElementFactory]")
{
    const RefractElementFactory& factory = FactoryFromType(mson::EnumTypeName);
    auto e = factory.Create("Enumerator", eSample);

    StringElement* generic = TypeQueryVisitor::as<StringElement>(e.get());
    REQUIRE(generic != NULL);
    REQUIRE(!generic->empty());
    REQUIRE(generic->meta().empty());
    REQUIRE(generic->attributes().empty());
    REQUIRE(generic->element() == "generic");
    REQUIRE(generic->get() == "Enumerator");
}
