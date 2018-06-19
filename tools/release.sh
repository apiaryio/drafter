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

update_homebrew_formula() {
  # Updates the Homebrew formula via a pull request to the repo
  HASH="$(openssl sha256 "$TARBALL" | awk '{print $2}')"
  BRANCH="drafter-$TAG"

  CREDENTIALS="ApiaryBot:$GITHUB_TOKEN"

  git clone "https://$CREDENTIALS@github.com/apiaryio/homebrew-formulae.git"

  # Update URL and HASH of tarball
  sed -i -e "s/v.*\.tar\.gz/$TAG\/$TARBALL/" homebrew-formulae/Formula/drafter.rb
  sed -i -e "s/sha256 '.*'/sha256 '$HASH'/" homebrew-formulae/Formula/drafter.rb

  # Print diff to stdout and commit
  cd homebrew-formulae
  git config user.name "Apiary Bot"
  git config user.email "support@apiary.io"

  git checkout -b "$BRANCH"
  git diff
  git add Formula/drafter.rb
  git commit -m "feat: Update drafter to $TAG"
  git push origin "$BRANCH"

  curl -s -u "$CREDENTIALS" \
      -H 'Content-Type: application/json' \
      -X POST -d "{\"title\": \"Update drafter to $TAG\", \"head\": \"$BRANCH\", \"base\": \"master\"}" \
      "https://api.github.com/repos/apiaryio/homebrew-formulae/pulls"
}

download_gh_release
create_tarball
create_release
update_homebrew_formula
