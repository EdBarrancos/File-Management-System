#!/bin/bash

#make directory of outputs with given name. if already exists does nothing
#for cicle to run each file in inputs directory. for cicle to run 0 - max multiple threads for each file.

if [ ! -d "$2" ]
then
    mkdir $2
fi


FILES=$(ls ${1})

for filename in $FILES; do
    for i in $(seq 0 ${3}); do
        echo "InputFile=${filename} NumThreads=$i"
        file="${filename%.*}"
        timeoutput=$(./tecnicofs "${1}/${filename}" "${2}/$file-${i}.txt" "${i}" "mutex")
        echo "${timeoutput##*$'\n'}"
    done
done 