#!/bin/bash

# Usage:
# $ plotHistograms.sh model lower_limit upper_limit

make

python plotHistograms.py $1 $2 $3 "./depthcomplexity3d_offline"
max=$?
i=0
while [ $i -ne $max ]
do
   gnuplot -e "var=$i" genPlot.gp
   mv plot.svg plot$i.svg
   i=$(( $i + 1 ))
done

python plotHistograms.py $1 $2 $3 "./randomdepthcomplexity3d_offline"
max=$?
i=0
while [ $i -ne $max ]
do
   gnuplot -e "var=$i" genPlot.gp
   mv plot.svg rplot$i.svg
   i=$(( $i + 1 ))
done

make clean

