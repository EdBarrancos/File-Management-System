#!/bin/bash

#make directory of outputs with given name. if already exists does nothing
#for cicle to run each file in inputs directory. for cicle to run 0 - max multiple threads for each file.

FILES=$(ls ${1})

for filename in $FILES; do
    for ((i=1; i<=${3}; i++)); do
        echo "InputFile=${filename} NumThreads=$i"
        file="${filename%.*}"
        timeoutput=$(./tecnicofs "${1}/${filename}" "${2}/$file-${i}.txt" "${i}" "mutex")
    done
done 