#!/bin/bash

set -e

clang-format --version | cut -d ' '  -f 3 | grep -q "^5\." || (echo "Requires clang-format version 5" && exit 1)

clang-format -style=file -i $(git ls-files | grep -e '\.cc$\|\.h$' | grep -v 'ext/boost')
