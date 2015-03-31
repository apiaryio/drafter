#!/bin/sh

RUNEMRUN=off
DEBUG=off
while [ $# -gt 0 ]
do
    case "$1" in
        -e)  RUNEMRUN=on;;
        -d)  DEBUG=on;;
        -*)
            echo >&2 "usage: $0 [-e] [-d]"
            exit 1;;
        *)  break;;
    esac
    shift
done

PYVER=`python -V 2>&1 | cut -d" " -f2 | cut -d. -f1`

if test $PYVER -eq 3; then
    PY=python2;
else
    PY="";
fi

FLAGS=""

if test "x$RUNEMRUN" == "xon"; then
    FLAGS="$FLAGS --emrun";
fi

if test "x$DEBUG" == "xon"; then
    FLAGS="$FLAGS -O0 -g";
else
    FLAGS="$FLAGS -O3 ";
fi

emconfigure $PY ./configure --shared
emmake make libdrafter

em++ $FLAGS ./build/out/Release/lib.target/libdrafter.so -s EXPORTED_FUNCTIONS="['_drafter_c_parse']" -o ./emcc/jsdrafter.raw.js ;

sed '/\/\* JSDRAFTER.RAW.JS \*\// {
        r ./emcc/jsdrafter.raw.js
        d
}' < ./emcc/wrapper.js > ./emcc/jsdrafter.js

rm -rf ./emcc/jsdrafter.raw.js
