#ifndef DRAFTER_DRAFTERTEST_H
#define DRAFTER_DRAFTERTEST_H

#include <bitset>

#include <catch2/catch.hpp>
#include "dtl.hpp"

#include "snowcrash.h"

#include "RefractAPI.h"
#include "RefractDataStructure.h"
#include "ConversionContext.h"

#include "stream.h"

#include "refract/SerializeSo.h"
#include "utils/log/Trivial.h"
#include "utils/so/JsonIo.h"

#include "Serialize.h"
#include "SerializeResult.h"

namespace draftertest
{
    constexpr std::size_t TEST_OPTION_EXPAND_MSON = 0;
    constexpr std::size_t TEST_OPTION_SOURCEMAPS = 1;

    using test_options = std::bitset<2>;

    constexpr test_options MSONTestOptions = test_options(0b01);

    bool handleResultJSON(const std::string& fixturePath, test_options testOpts, bool mustBeOk = false);
}

#define TEST_MSON(name, mustBeOk)                                                                                      \
    TEST_CASE("Testing MSON serialization for " name, "[refract][MSON][" name "]")                                     \
    {                                                                                                                  \
        REQUIRE(::draftertest::handleResultJSON("test/fixtures/mson/" name, MSONTestOptions, mustBeOk));               \
    }

#define TEST_REFRACT(category, name)                                                                                   \
    TEST_CASE("Testing refract serialization for " category " " name, "[refract][" category "][" name "]")             \
    {                                                                                                                  \
        ::draftertest::handleResultJSON("test/fixtures/" category "/" name, test_options(0));                          \
    }

#define TEST_REFRACT_SOURCE_MAP(category, name)                                                                        \
    TEST_CASE("Testing refract + source map serialization for " category " " name,                                     \
        "[refract_sourcemap][" category "][" name "]")                                                                 \
    {                                                                                                                  \
        ::draftertest::handleResultJSON(                                                                               \
            "test/fixtures/" category "/" name, test_options(0).set(TEST_OPTION_SOURCEMAPS));                          \
    }

#endif // #ifndef DRAFTER_DRAFTERTEST_H
