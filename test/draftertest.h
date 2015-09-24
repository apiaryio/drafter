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

        static bool handleResultJSON(const std::string& basepath, bool mustBeOk = false) {
            ITFixtureFiles fixture = ITFixtureFiles(basepath);

            snowcrash::ParseResult<snowcrash::Blueprint> blueprint;
            int result = snowcrash::parse(fixture.get(".apib"), 0, blueprint);

            if (mustBeOk) {
                REQUIRE(result == snowcrash::Error::OK);
            }

            std::stringstream outStream;
            sos::SerializeJSON serializer;

            serializer.process(drafter::WrapResult(blueprint, 0, drafter::RefractASTType), outStream);
            outStream << "\n";

            return (outStream.str() == fixture.get(".json"));
        }
    };
}

#endif // #ifndef DRAFTER_DRAFTERTEST_H
