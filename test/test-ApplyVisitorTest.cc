#include "catch.hpp"

#include "Visitor.h"
#include "Element.h"
#include "Query.h"
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

    Functor() : GCounter(0), SCounter(0) {}

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

// TEST_CASE("It should invoke specific operator for specialized element", "[Visitor]")
//{
//    refract::IElement* e = new refract::StringElement;
//    Functor f;
//    refract::Visitor a(f);
//
//    a.visit(*e);
//
//    REQUIRE(f.GCounter == 0);
//    REQUIRE(f.SCounter == 1);
//
//    delete e;
//}

TEST_CASE("It should invoke Functor for member of container elements", "[Visitor]")
{

    Functor f;
    refract::Visitor v(f);

    auto e = make_element<ArrayElement>();
    auto& content = e->get();
    content.push_back(from_primitive(3.0));
    content.push_back(from_primitive(false));
    content.push_back(from_primitive("Ehlo"));

    v.visit(*e);

    REQUIRE(f.GCounter == 1); // just array
    REQUIRE(f.SCounter == 0);
}

// TEST_CASE("It should recognize Element Type by `Is` type operand", "[Visitor]")
//{
//    IElement* e = IElement::Create("xxxx");
//
//    query::Is<StringElement> isString;
//    refract::Visitor sv(isString);
//    sv.visit(*e);
//    REQUIRE(isString);
//
//    query::Is<NumberElement> isNumber;
//    refract::Visitor nv(isNumber);
//    nv.visit(*e);
//    REQUIRE(!isNumber);
//
//    IElement* n = IElement::Create(42);
//    nv.visit(*n);
//    REQUIRE(isNumber);
//
//    delete n;
//    delete e;
//}

struct Fixture {

    static std::unique_ptr<IElement> Complex()
    {
        auto result = make_element<ObjectElement>();
        auto& content = result->get();

        {
            content.addMember("m1", from_primitive("Str1"));
        }

        {
            auto arr = make_element<ArrayElement>();
            {
                auto& c = arr->get();
                c.push_back(from_primitive("m2[0]"));
                c.push_back(from_primitive(2.1));
            }
            content.addMember("m2", std::move(arr));
        }

        {
            auto obj = make_element<ObjectElement>();
            {
                auto& c = obj->get();
                c.addMember("m3.1", from_primitive("Str3.1"));
                c.addMember("m3.2", from_primitive(3.2));

                auto arr = make_element<ArrayElement>();
                {
                    auto& arrc = arr->get();
                    arrc.push_back(from_primitive("m[3][3][0]"));
                    arrc.push_back(from_primitive(false));
                }
                c.addMember("m3.3", std::move(arr));

                auto subObj = make_element<ObjectElement>();
                {
                    auto& subObjc = subObj->get();
                    subObjc.addMember("m3.4.1", from_primitive("Str3/4/1"));
                    subObjc.addMember("m3.4.2", from_primitive(3.42));
                    subObjc.addMember("m3.4.2", make_empty<NullElement>());
                }
                c.addMember("m3.4", std::move(subObj));
            }
            content.addMember("m3", std::move(obj));
        }

        return std::move(result);
    }

    static std::unique_ptr<IElement> SimpleObject()
    {
        auto result = make_element<ObjectElement>();
        auto& content = result->get();

        content.addMember("m1", from_primitive("Str1"));
        content.addMember("m2", from_primitive("Str2"));
        content.addMember("m3", from_primitive(3.0));

        return std::move(result);
    }

    static std::unique_ptr<IElement> ObjectWithChild()
    {
        auto result = make_element<ObjectElement>();
        auto& content = result->get();

        content.addMember("m1", from_primitive("Str1"));

        auto child = make_element<ObjectElement>();
        {
            auto& childc = child->get();
            childc.addMember("m2.1", from_primitive("Str2/1"));
            childc.addMember("m2.2", make_empty<NullElement>());
        }
        content.addMember("m2", std::move(child));

        return std::move(result);
    }

    static std::unique_ptr<IElement> SimpleArray()
    {
        auto result = make_element<ArrayElement>();
        auto& content = result->get();

        content.push_back(from_primitive("1"));
        content.push_back(from_primitive(2.0));
        content.push_back(from_primitive("3"));

        return std::move(result);
    }

    static std::unique_ptr<IElement> ArrayWithChild()
    {
        auto result = make_element<ArrayElement>();
        auto& content = result->get();

        content.push_back(from_primitive("1"));

        auto child = make_element<ArrayElement>();
        {
            auto& childc = child->get();
            childc.push_back(from_primitive(1.0));
            childc.push_back(from_primitive(2.0));
        }
        content.push_back(std::move(child));

        content.push_back(from_primitive("3"));

        return std::move(result);
    }
};

TEST_CASE("Iterate<Recursive>", "[Visitor]")
{
    auto e = Fixture::SimpleArray();

    Functor f;
    Iterate<> i(f);
    i(*e);

    REQUIRE(f.GCounter == 2); // root array + number
    REQUIRE(f.SCounter == 2); // there are two strings
}

TEST_CASE("Iterate<Children> on array", "[Visitor]")
{
    auto e = Fixture::ArrayWithChild();

    Functor f;
    Iterate<Children> i(f);
    i(*e);

    REQUIRE(f.GCounter == 1); // embeded array
    REQUIRE(f.SCounter == 2); // there are two strings
}

TEST_CASE("Iterate<Children> on object", "[Visitor]")
{
    auto e = Fixture::ObjectWithChild();

    Functor f;
    Iterate<Children> i(f);
    i(*e);

    REQUIRE(f.GCounter == 2); // 2 members
    REQUIRE(f.SCounter == 0); // there are two strings
}

TEST_CASE("Iterate<Children> on string", "[Visitor]")
{
    auto e = from_primitive("string");

    Functor f;
    Iterate<Children> i(f);
    i(*e);

    // Functor is not invoked because string has no children
    REQUIRE(f.GCounter == 0);
    REQUIRE(f.SCounter == 0);
}

TEST_CASE("Query Element name", "[Visitor]")
{
    auto a = make_element<ArrayElement>();
    auto& ac = a->get();

    ac.push_back(from_primitive("str"));

    auto namedArrayPtr = [](auto& content) {
        auto namedArray = make_empty<ArrayElement>();
        namedArray->element("named");

        auto result = namedArray.get();
        content.push_back(std::move(namedArray));

        return result;
    }(ac);

    auto namedNumberPtr = [](auto& content) {
        auto namedNumber = make_empty<NumberElement>();
        namedNumber->element("named");

        auto result = namedNumber.get();
        content.push_back(std::move(namedNumber));

        return result;
    }(ac);

    ac.push_back(from_primitive("final"));

    FilterVisitor filter(refract::query::Element("named"));
    Iterate<> i(filter);
    i(*a);

    REQUIRE(filter.elements().size() == 2);
    REQUIRE(filter.elements()[0] == namedArrayPtr);
    REQUIRE(filter.elements()[1] == namedNumberPtr);
}
