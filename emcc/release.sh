#!/bin/bash
OS=`uname -s | tr '[:upper:]' '[:lower:]'`
TAG=`git describe --tags $CIRCLE_SHA1`

# Publish to GitHub releases
curl -L -O https://github.com/aktau/github-release/releases/download/v0.6.2/$OS-amd64-github-release.tar.bz2

tar -xjf $OS-amd64-github-release.tar.bz2

./bin/$OS/amd64/github-release release -u apiaryio -r drafter --tag $TAG
./bin/$OS/amd64/github-release upload -u apiaryio -r drafter --tag $TAG --name drafter.js --file emcc/drafter.js
./bin/$OS/amd64/github-release upload -u apiaryio -r drafter --tag $TAG --name drafter.js --file emcc/drafter.js.mem

# Publish to npm
npm --no-git-tag-version version $TAG
npm publish
