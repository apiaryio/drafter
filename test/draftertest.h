#ifndef DRAFTER_DRAFTERTEST_H
#define DRAFTER_DRAFTERTEST_H

#include "catch.hpp"
#include "dtl.hpp"

#include <string>
#include <cstring>
#include "stream.h"
#include "snowcrash.h"
#include "SerializeAST.h"
#include "SerializeResult.h"
#include "sosJSON.h"

#define TEST_REFRACT(category, name) TEST_CASE("Testing refract serialization for " category " " name, "[refract][" category "]") { \
    FixtureHelper::handleResultJSON("test/fixtures/" category "/" name, drafter::WrapperOptions(drafter::RefractASTType, false, false)); \
}

#define TEST_REFRACT_SOURCE_MAP(category, name) TEST_CASE("Testing refract + source map serialization for " category " " name, "[refract][" category "]") { \
    FixtureHelper::handleResultJSON("test/fixtures/" category "/" name, drafter::WrapperOptions(drafter::RefractASTType, false, true)); \
}

#define TEST_AST(category, name) TEST_CASE("Testing AST serialization for " category " " name, "[ast][" category "]") { \
    FixtureHelper::handleResultJSON("test/fixtures/" category "/" name, drafter::WrapperOptions(drafter::NormalASTType, false, false)); \
}

#define TEST_AST_SOURCE_MAP(category, name) TEST_CASE("Testing AST + source map serialization for " category " " name, "[ast][" category "]") { \
    FixtureHelper::handleResultJSON("test/fixtures/" category "/" name, drafter::WrapperOptions(drafter::NormalASTType, false, true)); \
}

namespace draftertest {
    namespace ext {
      const std::string apib = ".apib";
      const std::string json = ".json";
      const std::string sourceMapJson = ".sourcemap.json";
      const std::string astJson = ".ast.json";
      const std::string astSourceMapJson = ".ast.sourcemap.json";
    }

    class ITFixtureFiles {

        const std::string base_;

#ifdef WIN
        struct slashToBackslash {
            char operator()(const char& c) {
                return c == '/' ? '\\' : c;
            }
        };
#endif

        std::string normalizePath(const std::string& path) const {
            std::string normalized = path;

#ifdef WIN
            std::transform(path.begin(), path.end(), normalized.begin(), slashToBackslash());
#endif

            return normalized;
        }

    public:
        ITFixtureFiles(const std::string& base) : base_(base) {}

        typedef std::auto_ptr<std::istream> stream_type;

        const std::string fetchContent(const std::string& filename) const {

            stream_type in(CreateStreamFromName<std::istream>(normalizePath(filename)));
            std::stringstream strStream;
            strStream << in->rdbuf();

            return strStream.str();
        }

        const std::string get(const std::string& ext) const {
            return fetchContent(base_ + ext);
        }
    };

    struct FixtureHelper {
        static const char *printDiff(const std::string& actual, const std::string& expected) {
          // First, convert strings into arrays of lines.
          std::vector <std::string> actualLines, expectedLines;

          std::stringstream actualStream(actual.c_str());
          std::stringstream expectedStream(expected.c_str());
          std::stringstream output;
          std::string buf;

          while(getline(actualStream, buf)) {
            actualLines.push_back(buf);
          }

          while(getline(expectedStream, buf)) {
            expectedLines.push_back(buf);
          }

          // Now, diff the arrays of lines and save the output.
          dtl::Diff <std::string> d(actualLines, expectedLines);
          d.compose();
          d.composeUnifiedHunks();
          d.printUnifiedFormat(output);

          return output.str().c_str();
        }

        static bool handleBlueprintJSON(const std::string& basepath, const drafter::WrapperOptions& options, bool mustBeOk = true) {
            ITFixtureFiles fixture = ITFixtureFiles(basepath);

            snowcrash::ParseResult<snowcrash::Blueprint> blueprint;
            snowcrash::BlueprintParserOptions parserOptions = 0;

            if (options.exportSourceMap) {
                parserOptions |= snowcrash::ExportSourcemapOption;
            }

            int result = snowcrash::parse(fixture.get(ext::apib), parserOptions, blueprint);

            if (mustBeOk) {
                REQUIRE(result == snowcrash::Error::OK);
            }

            std::stringstream outStream;
            sos::SerializeJSON serializer;

            serializer.process(drafter::WrapBlueprint(blueprint, options), outStream);
            outStream << "\n";

            return (outStream.str() == fixture.get(ext::json));
        }

        static bool handleResultJSON(const std::string& basepath, const drafter::WrapperOptions& options, bool mustBeOk = false) {
            ITFixtureFiles fixture = ITFixtureFiles(basepath);

            snowcrash::ParseResult<snowcrash::Blueprint> blueprint;
            snowcrash::BlueprintParserOptions parserOptions = 0;

            if (options.exportSourceMap) {
              parserOptions |= snowcrash::ExportSourcemapOption;
            }

            int result = snowcrash::parse(fixture.get(ext::apib), parserOptions, blueprint);

            if (mustBeOk) {
                REQUIRE(result == snowcrash::Error::OK);
            }

            std::stringstream outStream;
            sos::SerializeJSON serializer;

            serializer.process(drafter::WrapResult(blueprint, options), outStream);
            outStream << "\n";

            std::string actual = outStream.str();
            std::string expected;
            std::string extension;
            bool matches = false;

            if (options.astType == drafter::RefractASTType) {
              if (options.exportSourceMap) {
                extension = ext::sourceMapJson;
              } else {
                extension = ext::json;
              }
            } else {
              if (options.exportSourceMap) {
                extension = ext::astSourceMapJson;
              } else {
                extension = ext::astJson;
              }
            }

            INFO("Filename: \x1b[35m" << basepath << extension << "\x1b[0m");
            expected = fixture.get(extension);

            if (actual != expected) {
              // If the two don't match, then output the diff.
              const char *diff = FixtureHelper::printDiff(actual, expected);
              FAIL(diff);
            }

            return actual == expected;
        }
    };
}

#endif // #ifndef DRAFTER_DRAFTERTEST_H
