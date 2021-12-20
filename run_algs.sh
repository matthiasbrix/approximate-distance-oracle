#!/bin/bash

EXE=main
DEBUG='n'
INPUTFILE=''
OUTPUTFILE=''
KINT=-1
UINT=-1
VINT=-1
PREPRO=-1
QUERY=-1

while getopts i:o:k:u:v:d:p:q: option
do
 case "${option}"
 in
     i) INPUTFILE=${OPTARG};;
     o) OUTPUTFILE=${OPTARG};;
     k) KINT=${OPTARG};;
     u) UINT=${OPTARG};;
     v) VINT=${OPTARG};;
     d) DEBUG=${OPTARG};;
     p) PREPRO=${OPTARG};;
     q) QUERY=${OPTARG};;
 esac
done

make rebuild

# Chop of "/" from tests/file
for (( i=0; i<${#INPUTFILE}; i++ )); do
    if [ "${INPUTFILE:$i:1}" == "/" ]
    then
        break
    fi 
done
# The new file included in final print file
NEWINPUTFILE=${INPUTFILE:i+1}

# check if folder benchmarks exists
if [ ! -d "benchmarks" ]; then
	mkdir benchmarks
fi

name=""

if [ $DEBUG = 'y' ]; then
    FILENAME=debug
    i=0
    while [[ -e benchmarks/$i-$FILENAME.txt ]] ; do
        let i++
    done
    name=$i-$FILENAME.txt
    ./bin/$EXE > $name
elif [[ -r $INPUTFILE && $KINT -gt 0 && $UINT -gt 0 && $VINT -gt 0 && $PREPRO -gt 0 && $QUERY -gt 0 ]]; then
    FILENAME=$NEWINPUTFILE-"k=$KINT"-"u=$UINT"-"v=$VINT"-"p=$PREPRO"-"q=$QUERY"
    # Find out which num. of occurence it is
    i=0
    while [[ -e benchmarks/$i-$FILENAME.txt ]] ; do
        let i++
    done
    name=$i-$FILENAME.txt
    touch $name
    for (( j=0; j<$PREPRO; j++ )); do
        ./bin/$EXE "tz" $INPUTFILE $OUTPUTFILE $KINT $UINT $VINT $QUERY >> $name
    done
    ./bin/$EXE "dj" $INPUTFILE $OUTPUTFILE $KINT $UINT $VINT $QUERY >> $name
    ./bin/$EXE "djopt" $INPUTFILE $OUTPUTFILE $KINT $UINT $VINT $QUERY >> $name
    ./bin/$EXE "bdj" $INPUTFILE $OUTPUTFILE $KINT $UINT $VINT $QUERY >> $name
else
    echo "Something went wrong!"
    exit
fi

mv $name benchmarks

