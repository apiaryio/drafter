//
//  stream.h
//  drafter
//
//  Created by Jiri Kratochvil on 2015-03-03
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_STREAM_H
#define DRAFTER_STREAM_H

#include <memory>

#include <string>

#include <iostream>
#include <sstream>
#include <fstream>

/**
 *  \brief proxy redirect i/o operations to stdin/stdout
 *  and avoid close stdin/stdout while delete
 */
template <typename T> struct std_io_proxy;

template <> struct std_io_proxy<std::ostream> : public std::ostream {
    //std::streambuf dummy;
    std::stringbuf dummy;

    std_io_proxy() : std::ostream(std::cout.rdbuf())
    {
        //rdbuf(std::cout.rdbuf());
    }

    virtual ~std_io_proxy()
    {
        rdbuf(&dummy);
    }
};

template <> struct std_io_proxy<std::istream> : public std::istream {
    //std::streambuf dummy;
    std::stringbuf dummy;

    std_io_proxy() : std::istream(std::cin.rdbuf())
    {
        //saved = rdbuf(std::cin.rdbuf());
    }

    virtual ~std_io_proxy()
    {
        rdbuf(&dummy);
    }
};

/**
 *  \brief functor returns cin/cout ptr in based on istream or ostream
 */
template<typename T> struct std_io {
    T* operator()() const
    {
        return new std_io_proxy<T>();
    }
};

/**
 *  \brief functor returns appropriate cin/cout based on istream/ostream
 */
template<typename T> struct std_io_selector {
    typedef T stream_type;
    typedef stream_type* return_type;

    return_type operator()() const
    {
        return return_type(std_io<T>()());
    }
};

/**
 *  \brief functor select fstream based on iostream
 */
template <typename T> struct to_fstream;

template<>
struct to_fstream<std::istream>{
  typedef std::ifstream stream_type;
  static std::ios_base::openmode open_flags() { return std::ios_base::in | std::ios_base::binary; }
};

template<>
struct to_fstream<std::ostream>{
  typedef std::ofstream stream_type;
  static std::ios_base::openmode open_flags() { return std::ios_base::out | std::ios_base::binary; }
};

/**
 *  \brief functor select return appropriate opened fstream based on type of stream
 */
template<typename T> struct fstream_io_selector{
    typedef typename to_fstream<T>::stream_type stream_type;
    typedef stream_type* return_type;

    return_type operator()(const char* name) const
    {
        return return_type(new stream_type(name, to_fstream<T>::open_flags()));
    }
};

/**
 *  \brief return pointer to readable/writable stream or report error and exit()
 *
 *  free allocated memory must be released by calling `delete`
 *  optionaly you can use std::auto_ptr<>
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
T* CreateStreamFromName(const std::string& file)
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


#endif // #ifndef DRAFTER_STREAM_H
