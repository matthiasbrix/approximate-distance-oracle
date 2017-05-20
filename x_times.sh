#!/bin/bash

i=0
num=$1
echo $num
while [ $i -lt "$num" ]
do
	./run_tests.sh -d y
	i=$(( i+1 ))
done

