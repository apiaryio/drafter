#/usr/bin/env bash

set -e

print_changed_message() {
  echo "Committed THIRD_PARTY_LICENSES.txt is inconsistent with license files."
  echo "Please run './tools/make-third-party-licenses.sh' and commit 'THIRD_PARTY_LICENSES.txt'."
  exit 1
}

./tools/make-third-party-licenses.sh
git diff --quiet -- THIRD_PARTY_LICENSES.txt || print_changed_message
