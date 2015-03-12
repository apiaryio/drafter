#ifndef DRAFTER_DRAFTERTEST_H
#define DRAFTER_DRAFTERTEST_H

#include "catch.hpp"

#include "stream.h"

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

#endif // #ifndef DRAFTER_DRAFTERTEST_H
