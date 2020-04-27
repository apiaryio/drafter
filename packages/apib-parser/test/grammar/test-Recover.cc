#include "grammar/recover.h"

#include <catch2/catch.hpp>

#include <iostream>

using namespace Catch;
using namespace tao;

using namespace apib::parser;

struct name : pegtl::seq< pegtl::plus< pegtl::range<'a','z'> > > {};

struct sep : pegtl::one<','> {};
struct G1 : recover::list<
               name,
               sep,
               pegtl::eolf
             > {};


using S = std::vector<std::string>;
using E = std::vector<bool>;

template <typename Rule>
struct A : pegtl::nothing<Rule> {
};

template<> struct A< G1::item > {
   template< typename Input >
   static void apply( const Input& in, S& s, E& e)
   {
       s.emplace_back(in.string());
       e.push_back(true);
   }
};

template<> struct A< G1::invalid > {
   template< typename Input >
   static void apply( const Input& in, S& s, E& e)
   {
       s.emplace_back(in.string());
       e.push_back(false);
   }
};


TEST_CASE("LIST") {
    SECTION("a") {
        pegtl::memory_input<> in("a","");
        S s;
        E e;
        CHECK(pegtl::parse<G1, A>(in, s, e));

        REQUIRE(s.size() == 1);
        CHECK(s[0] == "a");
        CHECK(e[0]);
    }

    SECTION("a,b") {
        pegtl::memory_input<> in("a,b","");
        S s;
        E e;
        CHECK(pegtl::parse<G1, A>(in, s, e));

        REQUIRE(s.size() == 2);
        CHECK(s[0] == "a");
        CHECK(e[0]);
        CHECK(s[1] == "b");
        CHECK(e[1]);
    }

    SECTION("A,b") {
        pegtl::memory_input<> in("A,b","");
        S s;
        E e;
        CHECK(pegtl::parse<G1, A>(in, s, e));

        REQUIRE(s.size() == 2);
        CHECK(s[0] == "A");
        CHECK_FALSE(e[0]);
        CHECK(s[1] == "b");
        CHECK(e[1]);
    }

    SECTION("Aa,b") {
        pegtl::memory_input<> in("Aa,b","");
        S s;
        E e;
        CHECK(pegtl::parse<G1, A>(in, s, e));

        REQUIRE(s.size() == 2);
        CHECK(s[0] == "Aa");
        CHECK_FALSE(e[0]);
        CHECK(s[1] == "b");
        CHECK(e[1]);
    }

    SECTION("a,B") {
        pegtl::memory_input<> in("a,B","");
        S s;
        E e;
        CHECK(pegtl::parse<G1, A>(in, s, e));

        REQUIRE(s.size() == 2);
        CHECK(s[0] == "a");
        CHECK(e[0]);
        CHECK(s[1] == "B");
        CHECK_FALSE(e[1]);
    }

    SECTION("aA,b") {
        pegtl::memory_input<> in("aA,b","");
        S s;
        E e;
        CHECK(pegtl::parse<G1, A>(in, s, e));

        REQUIRE(s.size() == 2);
        CHECK(s[0] == "aA");
        CHECK_FALSE(e[0]);
        CHECK(s[1] == "b");
        CHECK(e[1]);
    }

    SECTION("a,") {
        pegtl::memory_input<> in("a,","");
        S s;
        E e;
        CHECK(pegtl::parse<G1, A>(in, s, e));
        
        REQUIRE(s.size() == 2);
        CHECK(s[0] == "a");
        CHECK(e[0]);
        CHECK(s[1] == "");
        CHECK_FALSE(e[1]);
    }

    SECTION("a,,b") {
        pegtl::memory_input<> in("a,,b","");
        S s;
        E e;
        CHECK(pegtl::parse<G1, A>(in, s, e));

        REQUIRE(s.size() == 3);
        CHECK(s[0] == "a");
        CHECK(e[0]);
        CHECK(s[1] == "");
        CHECK_FALSE(e[1]);
        CHECK(s[2] == "b");
        CHECK(e[2]);
    }

    SECTION(",a") {
        pegtl::memory_input<> in(",a","");
        S s;
        E e;
        CHECK(pegtl::parse<G1, A>(in, s, e));

        REQUIRE(s.size() == 2);
        CHECK(s[0] == "");
        CHECK_FALSE(e[0]);
        CHECK(s[1] == "a");
        CHECK(e[1]);
    }

    SECTION("") {
        pegtl::memory_input<> in("","");
        S s;
        E e;
        CHECK(pegtl::parse<G1, A>(in, s, e));

        REQUIRE(s.size() == 1);
        CHECK(s[0] == "");
        CHECK_FALSE(e[0]);
    }

    SECTION(",") {
        pegtl::memory_input<> in(",","");
        S s;
        E e;
        CHECK(pegtl::parse<G1, A>(in, s, e));

        REQUIRE(s.size() == 2);
        CHECK(s[0] == "");
        CHECK_FALSE(e[0]);
        CHECK(s[1] == "");
        CHECK_FALSE(e[1]);
    }

    SECTION("a,b,c,") {
        pegtl::memory_input<> in("a,b,c,","");
        S s;
        E e;

        CHECK(pegtl::parse<G1, A>(in, s, e));

        REQUIRE(s.size() == 4);
        CHECK(s[0] == "a");
        CHECK(e[0]);
        CHECK(s[1] == "b");
        CHECK(e[1]);
        CHECK(s[2] == "c");
        CHECK(e[2]);
        CHECK(s[3] == "");
        CHECK_FALSE(e[3]);
    }
}

struct G2 : recover::list<
               name,
               pegtl::one<',',':'>,
               pegtl::sor< pegtl::one<'}'> >
             > {};

template<> struct A< G2::item > {
   template< typename Input >
   static void apply( const Input& in, S& s, E& e)
   {
       s.emplace_back(in.string());
       e.push_back(true);
   }
};

template<> struct A< G2::invalid > {
   template< typename Input >
   static void apply( const Input& in, S& s, E& e)
   {
       s.emplace_back(in.string());
       e.push_back(false);
   }
};

TEST_CASE("grammar with two separators, ending by non `eolf`") {

    SECTION("c}") {
        pegtl::memory_input<> in("c}","");
        S s;
        E e;
        CHECK(pegtl::parse<G2, A>(in, s, e));

        REQUIRE(s.size() == 1);
        CHECK(s[0] == "c");
        CHECK(e[0]);
    }

    SECTION("ab,c}") {
        pegtl::memory_input<> in("ab,c}","");
        S s;
        E e;
        CHECK(pegtl::parse<G2, A>(in, s, e));

        REQUIRE(s.size() == 2);
        CHECK(s[0] == "ab");
        CHECK(e[0]);
        CHECK(s[1] == "c");
        CHECK(e[1]);
    }

    SECTION("a:b,c}") {
        pegtl::memory_input<> in("a:b,c}","");
        S s;
        E e;
        CHECK(pegtl::parse<G2, A>(in, s, e));

        REQUIRE(s.size() == 3);
        CHECK(s[0] == "a");
        CHECK(e[0]);
        CHECK(s[1] == "b");
        CHECK(e[1]);
        CHECK(s[2] == "c");
        CHECK(e[2]);
    }

    SECTION("a:b:c:") {
        pegtl::memory_input<> in("a:b:c:","");
        S s;
        E e;
        CHECK_FALSE(pegtl::parse<G2, A>(in, s, e));

        REQUIRE(s.size() == 4);
        CHECK(s[0] == "a");
        CHECK(e[0]);
        CHECK(s[1] == "b");
        CHECK(e[1]);
        CHECK(s[2] == "c");
        CHECK(e[2]);
        CHECK(s[3] == "");
        CHECK_FALSE(e[3]);
    }

    SECTION("a:b:c") {
        pegtl::memory_input<> in("a:b:c","");
        S s;
        E e;
        CHECK_FALSE(pegtl::parse<G2, A>(in, s, e));

        REQUIRE(s.size() == 3);
        CHECK(s[0] == "a");
        CHECK(e[0]);
        CHECK(s[1] == "b");
        CHECK(e[1]);
        CHECK(s[2] == "c");
        CHECK_FALSE(e[2]); // not finished by ':'
    }

    SECTION("a:b:c}:") {
        pegtl::memory_input<> in("a:b:c}:","");
        S s;
        E e;
        CHECK(pegtl::parse<G2, A>(in, s, e));

        REQUIRE(s.size() == 3);
        CHECK(s[0] == "a");
        CHECK(e[0]);
        CHECK(s[1] == "b");
        CHECK(e[1]);
        CHECK(s[2] == "c");
        CHECK(e[2]);
    }

}

struct G3 : recover::ensure<
               name,
               pegtl::one<','>
             > {};

template<> struct A< G3::content > {
   template< typename Input >
   static void apply( const Input& in, S& s, E& e)
   {
       s.emplace_back(in.string());
       e.push_back(true);
   }
};

template<> struct A< G3::invalid > {
   template< typename Input >
   static void apply( const Input& in, S& s, E& e)
   {
       s.emplace_back(in.string());
       e.push_back(false);
   }
};

TEST_CASE("ensure grammar") {

    SECTION("abc") {
        pegtl::memory_input<> in("abc,","");
        S s;
        E e;
        CHECK(pegtl::parse<G3, A>(in, s, e));

        REQUIRE(s.size() == 1);
        CHECK(s[0] == "abc");
        CHECK(e[0]);
    }

    SECTION("abc,def") {
        pegtl::memory_input<> in("abc,def","");
        S s;
        E e;
        CHECK(pegtl::parse<G3, A>(in, s, e));

        REQUIRE(s.size() == 1);
        CHECK(s[0] == "abc");
        CHECK(e[0]);
    }

    SECTION("ab c") {
        pegtl::memory_input<> in("ab c,","");
        S s;
        E e;
        CHECK(pegtl::parse<G3, A>(in, s, e));

        REQUIRE(s.size() == 1);
        CHECK(s[0] == "ab c");
        CHECK_FALSE(e[0]);
    }

    SECTION("ab c,def") {
        pegtl::memory_input<> in("ab c,def","");
        S s;
        E e;
        CHECK(pegtl::parse<G3, A>(in, s, e));

        REQUIRE(s.size() == 1);
        CHECK(s[0] == "ab c");
        CHECK_FALSE(e[0]);
    }

    SECTION("abc:def") {
        pegtl::memory_input<> in("abc:def","");
        S s;
        E e;
        CHECK_FALSE(pegtl::parse<G3, A>(in, s, e));

        REQUIRE(s.empty());
    }
}

