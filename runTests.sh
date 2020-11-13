#!/bin/bash

#make directory of outputs with given name. if already exists does nothing

ZERO=0

if [[ ! -d "$2" ]]
then
    echo "Output Directory non existant"
else
    if [[ ! -d "$1" ]]
    then 
        echo "Input Directory non existant"

    else
        if [[ "$3" < "$ZERO" ]]
        then
            echo "Number of Threads needs to be greater then Zero"
        else

            #for cicle to run each file in inputs directory. for cicle to run 0 - max multiple threads for each file.
            #prints the last line of variable

            FILES=$(ls ${1})

            for filename in $FILES; do
                for i in $(seq 1 ${3}); do
                    echo "InputFile=${filename} NumThreads=$i"
                    file=${filename%.*}
                    timeoutput=$(./tecnicofs ${1}/${filename} ${2}/$file-${i}.txt ${i})
                    echo ${timeoutput##*$'\n'}
                done
            done
        fi
    fi
fi


