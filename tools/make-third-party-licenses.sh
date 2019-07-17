#!/usr/bin/env bash

set -e

DESTINATION=THIRD_PARTY_LICENSES.txt
SEPARATOR="---------separator---------"

echo "libdrafter" > $DESTINATION
echo $SEPARATOR >> $DESTINATION
(
    echo "Boost $(cat ext/boost/VERSION)" >> $DESTINATION
    cat ext/boost/LICENSE_1_0.txt >> $DESTINATION
    echo $SEPARATOR >> $DESTINATION

    echo "variant" >> $DESTINATION
    cat ext/variant/LICENSE.md >> $DESTINATION
    echo $SEPARATOR >> $DESTINATION

    echo "Sundown" >> $DESTINATION
    cat ext/snowcrash/ext/markdown-parser/ext/sundown/LICENSE >> $DESTINATION
    echo $SEPARATOR >> $DESTINATION
)

echo "" >> $DESTINATION
echo "drafter-cli" >> $DESTINATION
echo $SEPARATOR >> $DESTINATION
(
    echo "cmdline" >> $DESTINATION
    cat ext/cmdline/LICENSE >> $DESTINATION
    echo $SEPARATOR >> $DESTINATION
)
