//
//  drafter
//  stream.h
//
//  Created by Jiri Kratochvil on 2015-03-03
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef _DRAFTER_STREAM_H_
#define _DRAFTER_STREAM_H_

#include <tr1/memory>
#include <tr1/shared_ptr.h>

#include <string>

#include <iostream>
#include <sstream>
#include <fstream>

/**
 *  \brief deleter functor to shared_ptr<> which does nothing
 *  usefull to get system resource like std::cin, std::cout to shared_ptr<>
 */
template<typename T>
struct dummy_deleter {
    void operator()(T* obj) const 
    {
      // do nothing
    }
};

/**
 *  \brief functor returns cin/cout ptr in based on istream or ostream
 */
template<typename T> struct std_io;

template<> struct std_io<std::istream> {
    std::istream* operator()() const
    { 
        return &std::cin; 
    }
};

template<> struct std_io<std::ostream> {
    std::ostream* operator()() const
    { 
        return &std::cout; 
    }
};

/**
 *  \brief functor returns appropriate cin/cout based on istream/ostream
 */
template<typename T> struct std_io_selector {
    typedef T stream_type;
    typedef std::tr1::shared_ptr<stream_type> return_type;

    return_type operator()() const
    { 
        return return_type(std_io<T>()(), dummy_deleter<stream_type>()); 
    }
};

/**
 *  \brief functor select fstream based on iostream
 */
template <typename T> struct to_fstream;

template<> 
struct to_fstream<std::istream>{
  typedef std::ifstream stream_type;
};

template<> 
struct to_fstream<std::ostream>{
  typedef std::ofstream stream_type;
};

/**
 *  \brief functor select return appropriate opened fstream based on type of stream
 */
template<typename T> struct fstream_io_selector{
    typedef typename to_fstream<T>::stream_type stream_type;
    typedef std::tr1::shared_ptr<stream_type> return_type;

    return_type operator()(const char* name) const 
    { 
        return return_type(new stream_type(name)); 
    }
};

/**
 *  \brief return writable stream or report error and exit()
 *
 *  return is based on \template param T (must be std::ostream or std::istream)
 *
 *  For std::istream it return opened std::ifstream with filename as in \param `file`
 *  if \param `file` is empty it will return std::cin
 *
 *  In similar way it work for std::ostream
 *
 *  \param template<T> type of returned stream
 *  \param file - name of file to open for read/write if empty use standart input/output
 *
 */

template<typename T>
std::tr1::shared_ptr<T> CreateStreamFromName(const std::string& file)
{
    if (file.empty()) {
        return std_io_selector<T>()();
    }

    typedef typename fstream_io_selector<T>::return_type return_type;

    return_type stream = fstream_io_selector<T>()(file.c_str());

    if (!stream->is_open()) {
      std::cerr << "fatal: unable to open file '" << file << "'\n";
      exit(EXIT_FAILURE);
    }

    return stream;
}


#endif // #ifndef _DRAFTER_STREAM_H_
