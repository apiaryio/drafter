#include <backend/MediaTypeS11n.h>

#include <backend/Backend.h>
#include <catch2/catch.hpp>
#include <parser/Mediatype.h>

#include <string>

using namespace apib::backend;
using media_type = apib::parser::mediatype::state;

SCENARIO("Representations of media type are serialized", "[backend][serialize][media-type]")
{
    GIVEN("a default constructed media type")
    {
        const auto tested = media_type();
        WHEN("it is serialialized")
        {
            const std::string result = serialize(tested);
            THEN("the result is empty")
            {
                REQUIRE(result.empty());
            }
        }
    }

    GIVEN("a media type with empty type")
    {
        const auto tested = media_type{ "", "schema", "json", {} };
        WHEN("it is serialialized")
        {
            const std::string result = serialize(tested);
            THEN("the result is empty")
            {
                REQUIRE(result.empty());
            }
        }
    }

    GIVEN("a media type with empty subtype")
    {
        const auto tested = media_type{ "application", "", "json", {} };
        WHEN("it is serialialized")
        {
            const std::string result = serialize(tested);
            THEN("the result is empty")
            {
                REQUIRE(result.empty());
            }
        }
    }

    GIVEN("a json media type")
    {
        const auto tested = media_type{ "application", "json", "", {} };
        WHEN("it is serialialized")
        {
            const std::string result = serialize(tested);
            THEN("the result is `application/json`")
            {
                REQUIRE(result == "application/json");
            }
        }
    }

    GIVEN("a json schema type")
    {
        const auto tested = media_type{ "application", "schema", "json", {} };
        WHEN("it is serialialized")
        {
            const std::string result = serialize(tested);
            THEN("the result is `application/json`")
            {
                REQUIRE(result == "application/schema+json");
            }
        }
    }

    GIVEN("a media type with restricted characters in its type")
    {
        const auto tested = media_type{ "!application", "schema", "json", {} };
        WHEN("it is serialialized")
        {
            const std::string result = serialize(tested);
            THEN("the result is escaped")
            {
                REQUIRE(result == "\"!application\"/schema+json");
            }
        }
    }

    GIVEN("a media type with restricted characters in its subtype")
    {
        const auto tested = media_type{ "application", "*schema", "json", {} };
        WHEN("it is serialialized")
        {
            const std::string result = serialize(tested);
            THEN("the result is escaped")
            {
                REQUIRE(result == "application/\"*schema\"+json");
            }
        }
    }

    GIVEN("a media type with restricted characters in its suffix")
    {
        const auto tested = media_type{ "application", "schema", "+json", {} };
        WHEN("it is serialialized")
        {
            const std::string result = serialize(tested);
            THEN("the result is escaped")
            {
                REQUIRE(result == "application/schema+\"+json\"");
            }
        }
    }

    GIVEN("a media type with parameters")
    {
        const auto tested = media_type{ //
            "application",              //
            "schema",                   //
            "json",                     //
            {
                { "charset", "utf-8" }, //
                { "c", "5930" }         //
            }
        };
        WHEN("it is serialialized")
        {
            const std::string result = serialize(tested);
            THEN("the result includes all parameters")
            {
                REQUIRE(result == "application/schema+json; charset=utf-8; c=5930");
            }
        }
    }

    GIVEN("a media type with restricted characters in parameter name")
    {
        const auto tested = media_type{ //
            "application",              //
            "schema",                   //
            "json",                     //
            {
                { "+charset", "utf-8" }, //
                { "c", "5930" }          //
            }
        };
        WHEN("it is serialialized")
        {
            const std::string result = serialize(tested);
            THEN("the result is escaped")
            {
                REQUIRE(result == "application/schema+json; \"+charset\"=utf-8; c=5930");
            }
        }
    }

    GIVEN("a media type with tspecial characters in parameter value")
    {
        const auto tested = media_type{ //
            "application",              //
            "schema",                   //
            "json",                     //
            {
                { "charset", "utf?8" }, //
                { "c", "59\r30" }       //
            }
        };
        WHEN("it is serialialized")
        {
            const std::string result = serialize(tested);
            THEN("the result is escaped")
            {
                REQUIRE(result == "application/schema+json; charset=\"utf?8\"; c=\"59\\r30\"");
            }
        }
    }
}
