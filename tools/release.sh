#!/bin/sh

OS=`uname -s | tr '[:upper:]' '[:lower:]'`
TAG=`git describe --tags $CIRCLE_SHA1`

# Download GitHub releases script
curl -L -O https://github.com/aktau/github-release/releases/download/v0.6.2/$OS-amd64-github-release.tar.bz2
tar -xjf $OS-amd64-github-release.tar.bz2
GITHUB_RELEASE=./bin/$OS/amd64/github-release

# Create GitHub release
$GITHUB_RELEASE release -u apiaryio -r drafter --tag $TAG

# Create and upload tarball
TARBALL=drafter-$TAG.tar.gz
./tools/make-tarball.sh $TAG
$GITHUB_RELEASE upload -u apiaryio -r drafter --tag $TAG --name $TARBALL --file $TARBALL

# Create emcc release
./emcc/release.sh $GITHUB_RELEASE
