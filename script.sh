#!/bin/bash

declare -a inputImages
inputImages=(./images/720.jpg ./images/1080.jpg ./images/4k.jpg)

declare -a outputImages
outputImages=(./images/720_out.jpg ./images/1080_out.jpg ./images/4k_out.jpg)

declare -a threadNums
threadNums=(1 2 4 8 16)

for i in {0..2}
do  
    printf "______________________________\n" >> times.txt
    #Title
    echo "Imagen: ${inputImages[$i]}" >> times.txt

    #Column names
    printf "Hilos\t\t\tTiempo (MM/SS)\n" >> times.txt

    input=${inputImages[i]}
    output=${outputImages[i]}

    #Running the program with different thread numbers
    for threadNum in ${threadNums[@]}
    do  
        (\time -ao times.txt -f "$threadNum\t\t\t\t\t%E" ./my-effect $input $output $threadNum) >> times.txt
    done

done
