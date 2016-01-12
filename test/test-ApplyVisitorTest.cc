#include "catch.hpp"

#include "refract/Visitors.h"
#include "refract/Element.h"
#include "refract/Query.h"
#include "refract/Build.h"

#include <iostream>

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

    refract::IElement* e = Build(new refract::ArrayElement)
                                (refract::IElement::Create(3));
                                (refract::IElement::Create(false));
                                (refract::IElement::Create("Ehlo"));
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

