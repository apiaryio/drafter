#include "catch.hpp"

#include "Visitor.h"
#include "Element.h"
#include "Query.h"
#include "Build.h"
#include "Iterate.h"
#include "FilterVisitor.h"

using namespace refract;

static int Fcounter = 0;
void Function(const refract::IElement&)
{
    Fcounter++;
}

TEST_CASE("It should accept and invoke function as functor ", "[Visitor]")
{
    refract::IElement* e = new refract::StringElement;
    refract::Visitor a(Function);
    a.visit(*e);
    REQUIRE(Fcounter == 1);

    delete e;
}

struct Functor {
    int GCounter;
    int SCounter;

    Functor() : GCounter(0), SCounter(0)
    {
    }

    void operator()(const refract::IElement& e)
    {
        GCounter++;
    }

    void operator()(const refract::StringElement& e)
    {
        SCounter++;
    }
};

TEST_CASE("It should accept Functor", "[Visitor]")
{
    refract::IElement* e = new refract::StringElement;
    Functor f;
    refract::Visitor a(f);
    delete e;
}

TEST_CASE("It should invoke generalized operator for non specialized element", "[Visitor]")
{
    refract::IElement* e = new refract::NumberElement;
    Functor f;
    refract::Visitor a(f);

    a.visit(*e);

    REQUIRE(f.GCounter == 1);
    REQUIRE(f.SCounter == 0);

    delete e;
}

TEST_CASE("It should invoke specific operator for specialized element", "[Visitor]")
{
#if 0
    refract::IElement* e = new refract::StringElement;
    Functor f;
    refract::Visitor a(f);

    a.visit(*e);

    REQUIRE(f.GCounter == 0);
    REQUIRE(f.SCounter == 1);

    delete e;
#endif
}

TEST_CASE("It should invoke Functor for member of container elements", "[Visitor]")
{

    Functor f;
    refract::Visitor v(f);

    refract::IElement* e
        = Build(new ArrayElement)(IElement::Create(3))(IElement::Create(false))(IElement::Create("Ehlo"));
    v.visit(*e);

    REQUIRE(f.GCounter == 1); // just array
    REQUIRE(f.SCounter == 0);

    delete e;
}

TEST_CASE("It should recognize Element Type by `Is` type operand", "[Visitor]")
{
#if 0
    IElement* e = IElement::Create("xxxx");

    query::Is<StringElement> isString;
    refract::Visitor sv(isString);
    sv.visit(*e);
    REQUIRE(isString);

    query::Is<NumberElement> isNumber;
    refract::Visitor nv(isNumber);
    nv.visit(*e);
    REQUIRE(!isNumber);

    IElement* n = IElement::Create(42);
    nv.visit(*n);
    REQUIRE(isNumber);

    delete n;
    delete e;
#endif
}

struct Fixture {

    static IElement* Complex()
    {
        return Build(new ObjectElement)("m1", IElement::Create("Str1"))(
            "m2", Build(new ArrayElement)(IElement::Create("m2[0]"))(IElement::Create(2.1)))("m3",
            Build(new ObjectElement)("m3.1", IElement::Create("Str3.1"))("m3.2", IElement::Create(3.2))(
                "m3.3", Build(new ArrayElement)(IElement::Create("m[3][3][0]"))(IElement::Create(false)))("m3.4",
                Build(new ObjectElement)("m3.4.1", IElement::Create("Str3/4/1"))("m3.4.2", IElement::Create(3.42))(
                    "m3.4.2", new NullElement)));
    }

    static IElement* SimpleObject()
    {
        return Build(new ObjectElement)("m1", IElement::Create("Str1"))("m2", IElement::Create("Str2"))(
            "m3", IElement::Create(3));
    }

    static IElement* ObjectWithChild()
    {
        return Build(new ObjectElement)("m1", IElement::Create("Str1"))(
            "m2", Build(new ObjectElement)("m2.1", IElement::Create("Str2/1"))("m2.2", new NullElement));
    }

    static IElement* SimpleArray()
    {
        return Build(new ArrayElement)(IElement::Create("1"))(IElement::Create(2))(IElement::Create("3"));
    }

    static IElement* ArrayWithChild()
    {
        return Build(new ArrayElement)(IElement::Create("1"))(
            Build(new ArrayElement())(IElement::Create(1))(IElement::Create(2)))(IElement::Create("3"));
    }
};

TEST_CASE("Iterate<Recursive>", "[Visitor]")
{
    IElement* e = Fixture::SimpleArray();

    Functor f;
    Iterate<> i(f);
    i(*e);

    REQUIRE(f.GCounter == 2); // root array + number
    REQUIRE(f.SCounter == 2); // there are two strings

    delete e;
}

TEST_CASE("Iterate<Children> on array", "[Visitor]")
{
    IElement* e = Fixture::ArrayWithChild();

    Functor f;
    Iterate<Children> i(f);
    i(*e);

    REQUIRE(f.GCounter == 1); // embeded array
    REQUIRE(f.SCounter == 2); // there are two strings

    delete e;
}

TEST_CASE("Iterate<Children> on object", "[Visitor]")
{
    IElement* e = Fixture::ObjectWithChild();

    Functor f;
    Iterate<Children> i(f);
    i(*e);

    REQUIRE(f.GCounter == 2); // 2 members
    REQUIRE(f.SCounter == 0); // there are two strings

    delete e;
}

TEST_CASE("Iterate<Children> on string", "[Visitor]")
{
    IElement* e = IElement::Create("string");

    Functor f;
    Iterate<Children> i(f);
    i(*e);

    // Functor is not invoked because string has no children
    REQUIRE(f.GCounter == 0);
    REQUIRE(f.SCounter == 0);

    delete e;
}

TEST_CASE("Query Element name", "[Visitor]")
{
    ArrayElement* a = new ArrayElement;

    a->push_back(IElement::Create("str"));

    ArrayElement* namedArray = new ArrayElement;
    namedArray->element("named");
    a->push_back(namedArray);

    NumberElement* namedNumber = new NumberElement;
    namedNumber->element("named");
    a->push_back(namedNumber);

    a->push_back(IElement::Create("final"));

    FilterVisitor filter(refract::query::Element("named"));
    Iterate<> i(filter);
    i(*a);

    REQUIRE(filter.elements().size() == 2);
    REQUIRE(filter.elements()[0] == namedArray);
    REQUIRE(filter.elements()[1] == namedNumber);

    delete a;
}
