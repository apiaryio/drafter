#!/usr/bin/env bash

set -e

DESTINATION=THIRD_PARTY_LICENSES.txt
SEPARATOR="---------separator---------"

echo "libapib" > $DESTINATION
echo $SEPARATOR >> $DESTINATION
(
    echo "Boost $(cat packages/boost/VERSION.txt)" >> $DESTINATION
    cat packages/boost/LICENSE_1_0.txt >> $DESTINATION
    echo $SEPARATOR >> $DESTINATION

    echo "variant" >> $DESTINATION
    cat packages/variant/LICENSE.md >> $DESTINATION
    echo $SEPARATOR >> $DESTINATION
)

echo "libapib-parser" > $DESTINATION
echo $SEPARATOR >> $DESTINATION
(
    echo "Boost $(cat packages/boost/VERSION.txt)" >> $DESTINATION
    cat packages/boost/LICENSE_1_0.txt >> $DESTINATION
    echo $SEPARATOR >> $DESTINATION

    echo "variant" >> $DESTINATION
    cat packages/variant/LICENSE.md >> $DESTINATION
    echo $SEPARATOR >> $DESTINATION

    echo "Sundown" >> $DESTINATION
    cat packages/sundown/LICENSE >> $DESTINATION
    echo $SEPARATOR >> $DESTINATION

    echo "PEGTL" >> $DESTINATION
    cat packages/PEGTL/LICENSE >> $DESTINATION
    echo $SEPARATOR >> $DESTINATION
)

echo "libdrafter" > $DESTINATION
echo $SEPARATOR >> $DESTINATION
(
    echo "Boost $(cat packages/boost/VERSION.txt)" >> $DESTINATION
    cat packages/boost/LICENSE_1_0.txt >> $DESTINATION
    echo $SEPARATOR >> $DESTINATION

    echo "variant" >> $DESTINATION
    cat packages/variant/LICENSE.md >> $DESTINATION
    echo $SEPARATOR >> $DESTINATION
)

echo "" >> $DESTINATION
echo "drafter-cli" >> $DESTINATION
echo $SEPARATOR >> $DESTINATION
(
    echo "cmdline" >> $DESTINATION
    cat packages/cmdline/LICENSE >> $DESTINATION
    echo $SEPARATOR >> $DESTINATION
)
