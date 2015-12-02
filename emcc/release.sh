#!/bin/bash

if [ "x$1" = "x" ] ; then
  echo "Usage: $0 GIT_RELEASE_BIN"
  exit 1
fi

TAG=`git describe --tags $CIRCLE_SHA1`

$1 upload -u apiaryio -r drafter --tag $TAG --name drafter.js --file emcc/drafter.js
$1 upload -u apiaryio -r drafter --tag $TAG --name drafter.js.mem --file emcc/drafter.js.mem

# Use the CI host's NPM_TOKEN environment variable for auth
echo '//registry.npmjs.org/:_authToken=${NPM_TOKEN}' >.npmrc

# Publish to npm
npm --no-git-tag-version version $TAG
npm publish
