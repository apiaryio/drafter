#ifndef DRAFTER_DRAFTERTEST_H
#define DRAFTER_DRAFTERTEST_H

#include <catch2/catch.hpp>
#include "dtl.hpp"

#include "RefractAPI.h"
#include "RefractDataStructure.h"
#include "ConversionContext.h"

#include "stream.h"

#include "refract/SerializeSo.h"
#include "utils/log/Trivial.h"
#include "utils/so/JsonIo.h"

#include "Serialize.h"
#include "SerializeResult.h"

static drafter::WrapperOptions MSONTestOptions(false, true, true, true);
static drafter::WrapperOptions NormalTestOptions(false, true, true, false);
static drafter::WrapperOptions SourcemapTestOptions(true, true, true, false);

#define TEST_MSON(name, mustBeOk)                                                                                      \
    TEST_CASE("Testing MSON serialization for " name, "[refract][MSON][" name "]")                                     \
    {                                                                                                                  \
        REQUIRE(FixtureHelper::handleResultJSON("test/fixtures/mson/" name, MSONTestOptions, mustBeOk));               \
    }

#define TEST_REFRACT(category, name)                                                                                   \
    TEST_CASE("Testing refract serialization for " category " " name, "[refract][" category "][" name "]")             \
    {                                                                                                                  \
        FixtureHelper::handleResultJSON("test/fixtures/" category "/" name, NormalTestOptions);                        \
    }

#define TEST_REFRACT_SOURCE_MAP(category, name)                                                                        \
    TEST_CASE("Testing refract + source map serialization for " category " " name,                                     \
        "[refract_sourcemap][" category "][" name "]")                                                                 \
    {                                                                                                                  \
        FixtureHelper::handleResultJSON("test/fixtures/" category "/" name, SourcemapTestOptions);                     \
    }

namespace draftertest
{
    namespace ext
    {
        const std::string apib = ".apib";
        const std::string json = ".json";
        const std::string sourceMapJson = ".sourcemap.json";
    } // namespace ext

    class ITFixtureFiles
    {

        const std::string base_;

#ifdef WIN
        struct slashToBackslash {
            char operator()(const char& c)
            {
                return c == '/' ? '\\' : c;
            }
        };
#endif

        std::string normalizePath(const std::string& path) const
        {
            std::string normalized = path;

#ifdef WIN
            std::transform(path.begin(), path.end(), normalized.begin(), slashToBackslash());
#endif

            return normalized;
        }

    public:
        ITFixtureFiles(const std::string& base) : base_(base) {}

        typedef std::unique_ptr<std::istream> input_stream_type;

        const std::string fetchContent(const std::string& filename) const
        {

            input_stream_type in(CreateStreamFromName<std::istream>(normalizePath(filename)));
            std::stringstream strStream;
            strStream << in->rdbuf();

            return strStream.str();
        }

        const std::string get(const std::string& ext) const
        {
            return fetchContent(base_ + ext);
        }

        /// Set the contents of the fixture with the given file extension
        void set(const std::string& extension, const std::string& content)
        {
            std::string filename = base_ + extension;
            std::ofstream outputStream(normalizePath(filename).c_str());
            outputStream << content;
        }
    };

    struct FixtureHelper {

        static const std::string printDiff(const std::string& actual, const std::string& expected)
        {
            // First, convert strings into arrays of lines.
            std::vector<std::string> actualLines, expectedLines;

            std::stringstream actualStream(actual.c_str());
            std::stringstream expectedStream(expected.c_str());
            std::stringstream output;
            std::string buf;

            while (getline(actualStream, buf)) {
                actualLines.push_back(buf);
            }

            while (getline(expectedStream, buf)) {
                expectedLines.push_back(buf);
            }

            // Now, diff the arrays of lines and save the output.
            dtl::Diff<std::string> d(expectedLines, actualLines);
            d.compose();
            d.composeUnifiedHunks();
            d.printUnifiedFormat(output);

            return output.str();
        }

        static std::string getFixtureExtension(const drafter::WrapperOptions& options)
        {

            if (options.exportSourceMap) {
                return ext::sourceMapJson;
            } else {
                return ext::json;
            }

            return ext::json;
        }

        static bool handleResultJSON(
            const std::string& fixturePath, const drafter::WrapperOptions& options, bool mustBeOk = false)
        {
            ENABLE_LOGGING;
            ITFixtureFiles fixture = ITFixtureFiles(fixturePath);

            snowcrash::ParseResult<snowcrash::Blueprint> blueprint;

            const auto source = fixture.get(ext::apib);

            int result = snowcrash::parse(source, snowcrash::ExportSourcemapOption, blueprint);

            std::ostringstream outStream;
            drafter::ConversionContext context(source.c_str(), options);

            if (auto parsed = WrapRefract(blueprint, context)) {
                auto soValue = refract::serialize::renderSo(*parsed, options.exportSourceMap);
                drafter::utils::so::serialize_json(outStream, soValue);
            }

            outStream << "\n";

            std::string actual = outStream.str();
            std::string expected;
            std::string extension = getFixtureExtension(options);

            INFO("Filename: \x1b[35m" << fixturePath << extension << "\x1b[0m");
            expected = fixture.get(extension);

            if (actual != expected) {
                const char* generate = std::getenv("GENERATE");

                if (generate && std::string(generate) == "1") {
                    INFO("Updating incorrect fixture")
                    fixture.set(extension, actual);
                } else {
                    // If the two don't match, then output the diff.
                    std::string diff = FixtureHelper::printDiff(actual, expected);
                    FAIL(diff);
                }
            }

            if (mustBeOk) {
                REQUIRE(result == snowcrash::Error::OK);
            }

            return actual == expected;
        }
    };
} // namespace draftertest

#endif // #ifndef DRAFTER_DRAFTERTEST_H
