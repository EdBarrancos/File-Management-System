#!/bin/bash

#make directory of outputs with given name. if already exists does nothing

if [ ! -d "$2" ]
then
    mkdir $2
fi

#for cicle to run each file in inputs directory. for cicle to run 0 - max multiple threads for each file.

FILES=$(ls inputs/*.txt)

for filename in $FILES; do
    for ((i=0; i<=$3; i++)); do
        echo "InputFile=$filename NumThreads=$i"
        ./tecnicofs "$filename" "$filename-$i.txt" "$i" "mutex"
        mv "$filename-$i.txt" $2
    done
done 
