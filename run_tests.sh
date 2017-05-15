#!/bin/bash

EXE=main
DEBUG="n"
TESTFILE=''
OUTPUTFILE=''
K_INT=-1
U_INT=-1
V_INT=-1

for i in "$@"
do
case $i in
    -d=*|--debug=*)
    DEBUG="${i#*=}"
    ;;
    -i=*|--input=*)
    TESTFILE="${i#*=}"
    ;;
    -o=*|--output=*)
    OUTPUTFILE="${o#*=}"
    ;;
    -k=*|--kint=*)
    K_INT="${k#*=}"
    ;;
    -u=*|--uint=*)
    U_INT="${u#*=}"
    ;;
    -v=*|--vint=*)
    V_INT="${v#*=}"
    ;;
    --default)
    DEFAULT="ERROR"
    ;;
    *)
    # unknown option
    ;;
esac
done

make clean
make all

FILENAME=$OUTPUTFILE-"k=$K_INT"-"u=$U_INT"-"v=$U_INT"
if [ $DEBUG == "y" ] 
then
	./bin/$EXE
elif [[ -r $TESTFILE && $OUTPUTFILE && $K_INT -gt 0 && $U_INT -gt 0 && V_INT -gt 0 ]]; 
then
	./bin/$EXE tests/TESTFILE.txt $OUTPUTFILE.csv $K_INT $U_INT $V_INT
else
	echo "Something went wrong!"
fi

if [[ -e $FILENAME.csv && $DEBUG == "n" ]] ; then
	i=0
    while [[ -e $i-$FILENAME.csv ]] ; do
        let i++
    done
    name=$i-$FILENAME
fi
touch "$FILENAME".csv
mv $FILENAME.csv benchmarks
