#ifndef DRAFTER_DRAFTERTEST_H
#define DRAFTER_DRAFTERTEST_H

#include "catch.hpp"

#include <string>
#include <cstring>
#include "stream.h"
#include "snowcrash.h"
#include "SerializeAST.h"
#include "SerializeResult.h"
#include "sosJSON.h"

#define TEST_REFRACT(category, name) TEST_CASE("Testing refract serialization for " category " " name, "[refract][" category "]") { \
  FixtureHelper::handleResultJSON("test/fixtures/" category "/" name); \
}

#define TEST_AST(category, name) TEST_CASE("Testing AST serialization for " category " " name, "[ast][" category "]") { \
  FixtureHelper::handleResultJSON("test/fixtures/" category "/" name, drafter::NormalASTType); \
}

#define TEST_AST_SOURCE_MAP(category, name) TEST_CASE("Testing AST + source map serialization for " category " " name, "[ast][" category "]") { \
  FixtureHelper::handleResultJSON("test/fixtures/" category "/" name, drafter::NormalASTType, true); \
}

namespace draftertest {

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

        static bool handleBlueprintJSON(const std::string& basepath, drafter::ASTType astType = drafter::NormalASTType, bool expand = false, bool mustBeOk = true) {
            ITFixtureFiles fixture = ITFixtureFiles(basepath);

            snowcrash::ParseResult<snowcrash::Blueprint> blueprint;
            int result = snowcrash::parse(fixture.get(".apib"), 0, blueprint);

            if (mustBeOk) {
                REQUIRE(result == snowcrash::Error::OK);
            }

            std::stringstream outStream;
            sos::SerializeJSON serializer;

            serializer.process(drafter::WrapBlueprint(blueprint.node, astType, expand), outStream);
            outStream << "\n";

            return (outStream.str() == fixture.get(".json"));
        }

        static bool handleResultJSON(const std::string& basepath, drafter::ASTType astType = drafter::RefractASTType, bool sourceMap = false, bool mustBeOk = false) {
            ITFixtureFiles fixture = ITFixtureFiles(basepath);
            int options = 0;

            if (sourceMap) {
              options |= snowcrash::ExportSourcemapOption;
            }

            snowcrash::ParseResult<snowcrash::Blueprint> blueprint;
            int result = snowcrash::parse(fixture.get(".apib"), options, blueprint);

            if (mustBeOk) {
                REQUIRE(result == snowcrash::Error::OK);
            }

            std::stringstream outStream;
            sos::SerializeJSON serializer;

            serializer.process(drafter::WrapResult(blueprint, options, astType), outStream);
            outStream << "\n";

            std::string output = outStream.str();
            bool matches = false;

            if (astType == drafter::RefractASTType) {
              if (sourceMap) {
                matches = output == fixture.get(".sourcemap.json");
              } else {
                matches = output == fixture.get(".json");
              }
            } else {
              if (sourceMap) {
                matches = output == fixture.get(".ast.sourcemap.json");
              } else {
                matches = output == fixture.get(".ast.json");
              }
            }

            if (!matches) {
              // If the two don't match, then output the parsed result to help
              // with debugging the differences.
              FAIL(output);
            }

            return matches;
        }
    };
}

#endif // #ifndef DRAFTER_DRAFTERTEST_H
