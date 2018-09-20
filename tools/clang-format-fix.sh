#!/bin/bash

set -e

clang-format -style=file -i $(git ls-files markdown-parser snowcrash drafter | grep -e '\.cc$\|\.h$' | grep -v 'ext/')
