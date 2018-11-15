#!/bin/bash

set -e

clang-format -style=file -i $(git ls-files | grep -e '\.cc$\|\.h$' | grep -v 'ext/boost')
