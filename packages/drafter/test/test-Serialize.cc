//
//  test/test-Serialize.cc
//  test-libdrafter
//
//  Created by Thomas Jandecka on 3/09/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include <catch2/catch.hpp>

#include "Serialize.h"
#include "refract/dsd/Number.h"

using namespace drafter;

SCENARIO("Valid MSON numbers are deserialised with great success", "[serialize][mson]")
{
    GIVEN("the integer `0`")
    {
        const char* tested = "0";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A short, positive integer")
    {
        const char* tested = "8345";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A long, positive integer")
    {
        const char* tested
            = "84509898700970920436064387946845098987009709204360643879468450989870097092043606438794684509898700970920"
              "43"
              "60643879468450989870097092043606438794684509898700970920436064387946845098987009709204360643879468450989"
              "87"
              "00970920436064387946845098987009709204360643879468450989870097092043606438794684509898700970920436064387"
              "94"
              "68450989870097092043606438111111111179468450989870097092043606438794684509898700970920436064387946845098"
              "98"
              "70097092043606438794684509898700970920436064387946845098987009709204360643879468450989870097092043606438"
              "79"
              "46845098987009709204360643879468450989870097092043606438794684509898700970920436064387946845098987009709"
              "20"
              "43606438794684509898700970920436064387946845098987009709204360643879463333333333333333333333338450989870"
              "09"
              "709204360643879463";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A short, positive, rational number without exponent")
    {
        const char* tested = "0.05";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A long, positive, rational number without exponent")
    {

        const char* tested
            = "1."
              "02029149685050505458391496850505054529149685050505458391496850505054529149685050505458391496850505054529"
              "14"
              "96850505054583914968505050545291496850505054583914968505050545291496850505054583914968505050545291496850"
              "50"
              "50545839149685050505452914968505050545839149685050505452914968505050545839149685050505452914968505050545"
              "83"
              "91496850505054529149685050505458391496850505054529149685050505458391496850505054529149685050505458391496"
              "85"
              "05050545291496850505054583914968505050545291496850505054583914968505050545291496850505054583914968505050"
              "54"
              "52914968505050545839149685050505452914968505050545839149685050505452914968505050545839149685050505452914"
              "96"
              "85050505458391496850505054529149685050505458391496850505054529149685050505458391496850505054529149685050"
              "50"
              "54583914968505050545291496850505054583914968505050545888888888888888888888888291496850505054583914968505"
              "05"
              "054583914968505050545839149685050505458399999149685050505458390505";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A short, positive, rational number with positive exponent `e`")
    {
        const char* tested = "0.05e2";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A long, positive, rational number with positive exponent `e`")
    {

        const char* tested
            = "0."
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "00005050545291496850505054583914968505050545291496850505054583914968505050545291496850505000005e34280253"
              "42"
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "252222222234280252";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A short, positive, rational number with positive exponent capital `E`")
    {
        const char* tested = "0.05E2";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A long, positive, rational number with positive exponent capital `E`")
    {

        const char* tested
            = "0."
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "00005050545291496850505054583914968505050545291496850505054583914968505050545291496850505000005E34280253"
              "42"
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "252222222234280252";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }
    ///

    GIVEN("A short, positive, rational number with explicitly positive exponent `e`")
    {
        const char* tested = "0.05e+2";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A long, positive, rational number with explicitly positive exponent `e`")
    {

        const char* tested
            = "0."
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "00005050545291496850505054583914968505050545291496850505054583914968505050545291496850505000005e+"
              "3428025342"
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "252222222234280252";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A short, positive, rational number with explicitly positive exponent capital `E`")
    {
        const char* tested = "0.05E+2";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A long, positive, rational number with explicitly positive exponent capital `E`")
    {

        const char* tested
            = "0."
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "00005050545291496850505054583914968505050545291496850505054583914968505050545291496850505000005E+"
              "3428025342"
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "252222222234280252";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A short, positive, rational number with explicitly negative exponent `e`")
    {
        const char* tested = "0.05e-2";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A long, positive, rational number with explicitly negative exponent `e`")
    {

        const char* tested
            = "0."
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "00005050545291496850505054583914968505050545291496850505054583914968505050545291496850505000005e-"
              "3428025342"
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "252222222234280252";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A short, positive, rational number with explicitly negative exponent capital `E`")
    {
        const char* tested = "0.05E-2";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A long, positive, rational number with explicitly negative exponent capital `E`")
    {

        const char* tested
            = "0."
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "00005050545291496850505054583914968505050545291496850505054583914968505050545291496850505000005E-"
              "3428025342"
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "252222222234280252";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A short, negative integer")
    {
        const char* tested = "-8345";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A long, negative integer")
    {

        const char* tested
            = "-8450989870097092043606438794684509898700970920436064387946845098987009709204360643879468450989870097092"
              "04"
              "3"
              "60643879468450989870097092043606438794684509898700970920436064387946845098987009709204360643879468450989"
              "87"
              "00970920436064387946845098987009709204360643879468450989870097092043606438794684509898700970920436064387"
              "94"
              "68450989870097092043606438111111111179468450989870097092043606438794684509898700970920436064387946845098"
              "98"
              "70097092043606438794684509898700970920436064387946845098987009709204360643879468450989870097092043606438"
              "79"
              "46845098987009709204360643879468450989870097092043606438794684509898700970920436064387946845098987009709"
              "20"
              "43606438794684509898700970920436064387946845098987009709204360643879463333333333333333333333338450989870"
              "09"
              "709204360643879463";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A short, negative, rational number without exponent")
    {
        const char* tested = "-0.05";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A long, negative, rational number without exponent")
    {

        const char* tested
            = "-1."
              "02029149685050505458391496850505054529149685050505458391496850505054529149685050505458391496850505054529"
              "14"
              "96850505054583914968505050545291496850505054583914968505050545291496850505054583914968505050545291496850"
              "50"
              "50545839149685050505452914968505050545839149685050505452914968505050545839149685050505452914968505050545"
              "83"
              "91496850505054529149685050505458391496850505054529149685050505458391496850505054529149685050505458391496"
              "85"
              "05050545291496850505054583914968505050545291496850505054583914968505050545291496850505054583914968505050"
              "54"
              "52914968505050545839149685050505452914968505050545839149685050505452914968505050545839149685050505452914"
              "96"
              "85050505458391496850505054529149685050505458391496850505054529149685050505458391496850505054529149685050"
              "50"
              "54583914968505050545291496850505054583914968505050545888888888888888888888888291496850505054583914968505"
              "05"
              "054583914968505050545839149685050505458399999149685050505458390505";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A short, negative, rational number with positive exponent `e`")
    {
        const char* tested = "-0.05e2";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A long, negative, rational number with positive exponent `e`")
    {

        const char* tested
            = "-0."
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "00005050545291496850505054583914968505050545291496850505054583914968505050545291496850505000005e34280253"
              "42"
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "252222222234280252";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A short, negative, rational number with positive exponent capital `E`")
    {
        const char* tested = "-0.05E2";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A long, negative, rational number with positive exponent capital `E`")
    {

        const char* tested
            = "-0."
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "00005050545291496850505054583914968505050545291496850505054583914968505050545291496850505000005E34280253"
              "42"
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "252222222234280252";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }
    ///

    GIVEN("A short, negative, rational number with explicitly positive exponent `e`")
    {
        const char* tested = "-0.05e+2";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A long, negative, rational number with explicitly positive exponent `e`")
    {

        const char* tested
            = "-0."
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "00005050545291496850505054583914968505050545291496850505054583914968505050545291496850505000005e+"
              "3428025342"
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "252222222234280252";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A short, negative, rational number with explicitly positive exponent capital `E`")
    {
        const char* tested = "-0.05E+2";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A long, negative, rational number with explicitly positive exponent capital `E`")
    {

        const char* tested
            = "-0."
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "00005050545291496850505054583914968505050545291496850505054583914968505050545291496850505000005E+"
              "3428025342"
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "252222222234280252";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A short, negative, rational number with explicitly negative exponent `e`")
    {
        const char* tested = "-0.05e-2";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A long, negative, rational number with explicitly negative exponent `e`")
    {

        const char* tested
            = "-0."
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "00005050545291496850505054583914968505050545291496850505054583914968505050545291496850505000005e-"
              "3428025342"
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "252222222234280252";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A short, negative, rational number with explicitly negative exponent capital `E`")
    {
        const char* tested = "-0.05E-2";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A long, negative, rational number with explicitly negative exponent capital `E`")
    {

        const char* tested
            = "-0."
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "00005050545291496850505054583914968505050545291496850505054583914968505050545291496850505000005E-"
              "3428025342"
              "80253428025342802534280253428025342802534280252222222234280252802534280253428025342802534280253428025342"
              "80"
              "252222222234280252";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }

    GIVEN("A number with leading zeros in exponent")
    {
        const char* tested = "0.05E-02";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == tested);
            }
        }
    }
}

SCENARIO("Invalid MSON numbers are deserialized without success", "[serialize][mson]")
{
    GIVEN("the string `Hello, world!`")
    {
        const char* tested = "Hello, world!";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is no result")
            {
                REQUIRE(!result.first);
            }
        }
    }

    GIVEN("the string `infinity`")
    {
        const char* tested = "infinity";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is no result")
            {
                REQUIRE(!result.first);
            }
        }
    }

    GIVEN("the string `-infinity`")
    {
        const char* tested = "-infinity";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is no result")
            {
                REQUIRE(!result.first);
            }
        }
    }

    GIVEN("the string `NaN`")
    {
        const char* tested = "NaN";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is no result")
            {
                REQUIRE(!result.first);
            }
        }
    }

    GIVEN("A positive number with ubiquitous leading zeros")
    {
        const char* tested = "05";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is no result")
            {
                REQUIRE(!result.first);
            }
        }
    }

    GIVEN("A negative number with ubiquitous leading zeros")
    {
        const char* tested = "-00.05E-2";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is no result")
            {
                REQUIRE(!result.first);
            }
        }
    }
}

SCENARIO("Valid MSON numbers with trailing spaces are deserialised with great success", "[serialize][mson]")
{
    GIVEN("the integer `0\\t `")
    {
        const char* tested = "0\t ";
        mson::Literal number(tested);

        WHEN("it is deserialized as a MSON number")
        {
            auto result = LiteralTo<refract::dsd::Number>(number);

            THEN("there is a result")
            {
                REQUIRE(result.first);
            }

            AND_THEN("the result is the original without trailing spaces")
            {
                REQUIRE(result.second.get() == "0");
            }
        }
    }
}
