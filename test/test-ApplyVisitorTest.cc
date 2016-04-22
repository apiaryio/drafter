#include "catch.hpp"

#include "Visitors.h"
#include "Element.h"
#include "Query.h"
#include "Build.h"
#include "Iterate.h"

using namespace refract;

static int Fcounter = 0;
void Function (const refract::IElement&) {
    Fcounter++;
}

TEST_CASE("It should accept and invoke function as functor ","[ApplyVisitor]") {
    refract::IElement* e = new refract::StringElement;
    refract::ApplyVisitor a(Function);
    a.visit(*e);
    REQUIRE(Fcounter == 1);

    delete e;
}

struct Functor {
    int GCounter;
    int SCounter;

    Functor() : GCounter(0), SCounter(0) {}

    void operator()(const refract::IElement& e) {
        GCounter++;
    }

    void operator()(const refract::StringElement& e) {
        SCounter++;
    }
};

TEST_CASE("It should accept Functor","[ApplyVisitor]") {
    refract::IElement* e = new refract::StringElement;
    Functor f;
    refract::ApplyVisitor a(f);
    delete e;
}

TEST_CASE("It should invoke generalized operator for non specialized element","[ApplyVisitor]") {
    refract::IElement* e = new refract::NumberElement;
    Functor f;
    refract::ApplyVisitor a(f);

    a.visit(*e);

    REQUIRE(f.GCounter == 1);
    REQUIRE(f.SCounter == 0);

    delete e;
}

TEST_CASE("It should invoke specific operator for specialized element","[ApplyVisitor]") {
    refract::IElement* e = new refract::StringElement;
    Functor f;
    refract::ApplyVisitor a(f);

    a.visit(*e);

    REQUIRE(f.GCounter == 0);
    REQUIRE(f.SCounter == 1);

    delete e;
}

TEST_CASE("It should invoke Functor for member of container elements","[ApplyVisitor]") {

    Functor f;
    refract::ApplyVisitor v(f);

    refract::IElement* e = Build(new ArrayElement)
                                (IElement::Create(3))
                                (IElement::Create(false))
                                (IElement::Create("Ehlo"));
    v.visit(*e);

    REQUIRE(f.GCounter == 1); // just array
    REQUIRE(f.SCounter == 0);

    delete e;
}

TEST_CASE("It should recognize Element Type by `Is` type operand","[ApplyVisitor]") {

    IElement* e = IElement::Create("xxxx");

    query::Is<StringElement> isString;
    refract::ApplyVisitor sv(isString);
    sv.visit(*e);
    REQUIRE(isString);

    query::Is<NumberElement> isNumber;
    refract::ApplyVisitor nv(isNumber);
    nv.visit(*e);
    REQUIRE(!isNumber);

    IElement* n = IElement::Create(42);
    nv.visit(*n);
    REQUIRE(isNumber);

    delete n;
    delete e;
}

struct Fixture {

    static IElement* Complex()
    {
        return Build(new ObjectElement)
                    ("m1", IElement::Create("Str1"))
                    ("m2", Build(new ArrayElement)
                                (IElement::Create("m2[0]"))
                                (IElement::Create(2.1))
                    )
                    ("m3", Build(new ObjectElement)
                                ("m3.1", IElement::Create("Str3.1"))
                                ("m3.2", IElement::Create(3.2))
                                ("m3.3", Build(new ArrayElement)
                                              (IElement::Create("m[3][3][0]"))
                                              (IElement::Create(false))
                     )
                     ("m3.4", Build(new ObjectElement)
                                   ("m3.4.1", IElement::Create("Str3/4/1"))
                                   ("m3.4.2", IElement::Create(3.42))
                                   ("m3.4.2", new NullElement))
                     );
    }

    static IElement* SimpleObject()
    {
        return Build(new ObjectElement)
                    ("m1", IElement::Create("Str1"))
                    ("m2", IElement::Create("Str2"))
                    ("m3", IElement::Create(3))
        ;
    }

    static IElement* SimpleArray()
    {
        return Build(new ArrayElement)
                    (IElement::Create("1"))
                    (IElement::Create(2))
                    (IElement::Create("3"))
        ;
    }
};

TEST_CASE("Matcher with Is<>","[ApplyVisitor]")
{
    IElement* e = Fixture::SimpleArray();

    Functor f;
    Iterate<> i(f);
    i(*e);

    REQUIRE(f.GCounter == 2); // array + number
    REQUIRE(f.SCounter == 2); // there are two strings

    delete e;
}

