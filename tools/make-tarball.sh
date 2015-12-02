#!/bin/sh

if [ "x$1" = "xcheckout" ] ; then
  # Recursively checkout a repository to destination
  # Usage: tools/make-tarball.sh checkout DESTINATION

  git checkout-index --all --prefix=$2
  git submodule foreach --quiet "$0 checkout $2"'$path/'

  exit 0
fi

TMPDIR=`mktemp -d`
trap 'rm -rf $TMPDIR' EXIT
VERSION=$1

if [ "x$VERSION" = "x" ] ; then
  echo "Usage: $0 VERSION"
  exit 1
fi

DIR=drafter-$VERSION
TARBALL=$(pwd)/drafter-$VERSION.tar.gz

echo "Exporting to $TMPDIR/$DIR"

$(pwd)/tools/make-tarball.sh checkout $TMPDIR/$DIR/

(
  # Remove unnessecery files
  cd $TMPDIR/$DIR
  rm -fr appveyor.yml circle.yml .npmignore tools/ package.json emcc/
)

(
  cd $TMPDIR
  echo "Creating tarball"
  env GZIP=-9 tar -czf $TARBALL $DIR
)
