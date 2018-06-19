#!/bin/sh

TAG=`git describe --tags $CIRCLE_SHA1`
TARBALL=drafter-$TAG.tar.gz

download_gh_release() {
  # Download GitHub releases script
  OS=`uname -s | tr '[:upper:]' '[:lower:]'`
  curl -L -O https://github.com/aktau/github-release/releases/download/v0.6.2/$OS-amd64-github-release.tar.bz2
  tar -xjf $OS-amd64-github-release.tar.bz2
  export PATH="./bin/$OS/amd64/:$PATH"
}

create_release() {
  # Create GitHub release
  github-release release -u apiaryio -r drafter --tag $TAG
  github-release upload -u apiaryio -r drafter --tag $TAG --name $TARBALL --file $TARBALL
}

create_tarball() {
  # Create release tarball
  ./tools/make-tarball.sh $TAG
}

download_gh_release
create_tarball
create_release
