#!/bin/sh

RUNEMRUN=off
DEBUG=off
UGLIFY=off
while [ $# -gt 0 ]
do
    case "$1" in
        -e)  RUNEMRUN=on;;
        -d)  DEBUG=on;;
        -u)  UGLIFY=on;;
        -*)
            echo >&2 "usage: $0 [-e] [-d] [-u]"
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

if test "x$RUNEMRUN" = "xon"; then
    FLAGS="$FLAGS --emrun";
fi

if test "x$DEBUG" = "xon"; then
    FLAGS="$FLAGS -O0 -g";
    BUILDFLAGS=""
    UGLIFY=off
else
    FLAGS="$FLAGS -Oz --llvm-lto 1";
    BUILDFLAGS="-Oz"
fi

emconfigure $PY ./configure --shared
emmake make libdrafter CXXFLAGS=$BUILDFLAGS

em++ $FLAGS ./build/out/Release/lib.target/libdrafter.so -s EXPORTED_FUNCTIONS="['_drafter_c_parse']" -o ./emcc/drafter.js  --pre-js ./emcc/pre.js --post-js ./emcc/post.js

em++ $FLAGS --memory-init-file 0 ./build/out/Release/lib.target/libdrafter.so -s EXPORTED_FUNCTIONS="['_drafter_c_parse']" -o ./emcc/drafter.nomem.js  --pre-js ./emcc/pre.js --post-js ./emcc/post.js

if test "x$UGLIFY" = "xon"; then
    uglifyjs emcc/drafter.js -o drafter.js -c;
    mv drafter.js emcc/drafter.js;
fi
