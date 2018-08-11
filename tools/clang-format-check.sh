#!/bin/bash 

set -e

function report_error () {
  echo "There are some style issues. Please run clang-format on your files."
  exit 1;
}

clang-format --version | cut -d ' '  -f 3 | grep -q "^5\." || (echo "Requires clang-format version 5" && exit 1)

clang-format -style=file -output-replacements-xml $(git ls-files | grep -e '\.cc$\|\.h$' | grep -v 'ext/boost') | awk '/\<replacement /{exit 1}' || report_error
