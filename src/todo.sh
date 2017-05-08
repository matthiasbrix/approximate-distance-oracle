#!/bin/bash

COUNT=0
FN="thorupzwick.c"

rm -rf $FN

while IFS='' read -r line || [[ -n "$line" ]]; do
    COUNT=$(( $COUNT + 1 ))
    if [[ $line == *"TODO:"* ]]; then
        echo "Ln:" $COUNT ${line:5} >> $FN
    fi
done < "$1"
