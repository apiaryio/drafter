#ifndef DRAFTER_DRAFTERTEST_H
#define DRAFTER_DRAFTERTEST_H

#include "catch.hpp"

#include "stream.h"

class it_fixture_files {

    const std::string base_;

public:
    it_fixture_files(const std::string& base) : base_(base) {} 

    typedef std::tr1::shared_ptr<std::istream> stream_type;

    const std::string fetchContent(const std::string& filename) const {

        stream_type in = CreateStreamFromName<std::istream>(filename);
        std::stringstream strStream;
        strStream << in->rdbuf();

        return strStream.str();
    }

    const std::string apib() const {
        return fetchContent(base_ + ".apib");
    }

    const std::string json() const {
        return fetchContent(base_ + ".json");
    }

};

#endif // #ifndef DRAFTER_DRAFTERTEST_H
