#!/bin/bash

RUN=true

build_coverage() {
    if [ ! -d build/coverage ]; then
        echo "$ make clean"
        if [ "$RUN" = true ]; then
            make clean 2>/dev/null 1>&2
        fi
    fi

    if [ build/out -nt build/coverage ]; then
        echo "$ make clean"
        if [ "$RUN" = true ]; then
            make clean 2>/dev/null 1>&2
        fi
    fi

    echo "$ CFLAGS='-fprofile-arcs -ftest-coverage' CXXFLAGS='-fprofile-arcs -ftest-coverage' LDFLAGS='-lgcov --coverage' make test"
    if [ "$RUN" = true ]; then
        CFLAGS='-fprofile-arcs -ftest-coverage' CXXFLAGS='-fprofile-arcs -ftest-coverage' LDFLAGS='-lgcov --coverage' make test 2>/dev/null 1>&2
    fi
}

annotate_coverage() {
    echo "$ cd build"
    cd build

    echo "$ gcov -lr -s test $1"

    if [ "$RUN" = true ]; then
        gcov -lr -s ../test "../$1" 1>/dev/null
    fi

    echo "$ cd .."
    cd ..
}

filter_coverage() {
    echo "$ rm build/test-*##catch_*gcov"
    echo "$ rm build/test-*##test-*gcov"

    if [ "$RUN" = true ]; then
        rm build/test-*\#\#catch_*gcov # filter test framework
        rm build/test-*\#\#test-*gcov # filter tests themselves
    fi
}

dump_coverage() {
    echo "$ mkdir -p build/coverage"
    echo "$ mv -t build/coverage build/*.gcov"

    if [ "$RUN" = true ]; then
        mkdir -p build/coverage
        mv -t build/coverage build/*.gcov
    fi
}

################################################################################

TEST_OBJECTS=()
FILTER=true
while [[ $# -gt 0 ]]; do
    key="$1"
    case "$key" in
        # simulate only
        -s|--simulate)
        RUN=false
        ;;
        # dump all annotated sources, even Catch and tests themselves
        -a|--no-filter)
        FILTER=false
        ;;
        # remove annotated sources
        -c|--clean)
        rm -r build/coverage
        ;;
        # Explicitly set object file
        -o|--object-file)
        shift
        TEST_OBJECTS+=("$1")
        ;;
        *)
        TEST_OBJECTS+=("build/out/*/obj.target/test-libdrafter/${1%.*}.o")
        ;;
    esac
    shift
done

echo
echo "Building instrumented binaries & running them..."
build_coverage

echo
echo "Annotating sources..."
for test_object in $TEST_OBJECTS
do
    annotate_coverage $test_object
done

echo
echo "Filtering annotated sources..."
if [ "$FILTER" = true ]; then
    filter_coverage
fi

echo
echo "Dumping annotated sources to build/coverage/ ..."
dump_coverage
