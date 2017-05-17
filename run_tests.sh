#!/bin/bash

EXE=main
DEBUG='n'
INPUTFILE=''
OUTPUTFILE=''
KINT=-1
UINT=-1
VINT=-1

while getopts i:o:k:u:v:d: option
do
 case "${option}"
 in
     i) INPUTFILE=${OPTARG};;
     o) OUTPUTFILE=${OPTARG};;
     k) KINT=${OPTARG};;
     u) UINT=${OPTARG};;
     v) VINT=${OPTARG};;
     d) DEBUG=${OPTARG};;
 esac
done

make clean
make all

# Chop of "/" from tests/file
for (( i=0; i<${#INPUTFILE}; i++ )); do
    if [ "${INPUTFILE:$i:1}" == "/" ]
    then
        break
    fi 
done
# The new file included in final print file
NEWINPUTFILE=${INPUTFILE:i+1}

name=""

if [ $DEBUG = 'y' ]; then
    ./bin/$EXE > debug
    exit 0
elif [[ -r $INPUTFILE && $KINT -gt 0 && $UINT -gt 0 && $VINT -gt 0 ]]; then
    FILENAME=$NEWINPUTFILE-"k=$KINT"-"u=$UINT"-"v=$VINT"
    # Find out which num. of occurence it is
    i=0
    while [[ -e benchmarks/$i-$FILENAME.txt ]] ; do
        let i++
    done
    name=$i-$FILENAME.txt
    ./bin/$EXE $INPUTFILE $OUTPUTFILE $KINT $UINT $VINT > $name
else
    echo "Something went wrong!"
fi

mv $name benchmarks

