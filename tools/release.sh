#!/bin/sh

OS=`uname -s | tr '[:upper:]' '[:lower:]'`
TAG=`git describe --tags $CIRCLE_SHA1`

# Download GitHub releases script
curl -L -O https://github.com/aktau/github-release/releases/download/v0.6.2/$OS-amd64-github-release.tar.bz2
tar -xjf $OS-amd64-github-release.tar.bz2
GITHUB_RELEASE=./bin/$OS/amd64/github-release

# Create GitHub release
VERSION=$(git describe --tags)
$GITHUB_RELEASE release -u apiaryio -r drafter --tag $VERSION

# Create and upload tarball
TARBALL=drafter-$VERSION.tar.gz
./tools/make-tarball.sh $VERSION
$GITHUB_RELEASE upload -u apiaryio -r drafter --tag $VERSION --name $TARBALL --file $TARBALL

# Create emcc release
./emcc/release.sh $GITHUB_RELEASE
