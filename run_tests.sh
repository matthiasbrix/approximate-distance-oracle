#!/bin/bash
make all

for i in "$@"
do
case $i in
    -i=*|--input=*)
    INPUT="${i#*=}"
    ;;
    -s=*|--output=*)
    SIZE="${s#*=}"
    ;;
    --default)
    DEFAULT="ERROR"
    ;;
    *)
    # unknown option
    ;;
esac
done

./bin/thorupzwick tests/USANY.txt output.csv 
