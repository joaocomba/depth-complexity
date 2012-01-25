#!/bin/bash

DIR=$1

#Limits for NORMAL
LOWER_LIMIT[0]=1
UPPER_LIMIT[0]=2000
#Limits for RANDOM
LOWER_LIMIT[1]=1
UPPER_LIMIT[1]=2000

TYPE[0]="Normal"
TYPE[1]="Random"

TERM[0]="u"
TERM[1]="r"

TMP="temporary_genPlot_toRem.gp"

for t in 0 1
do
    echo "set terminal pdf" > ${TMP}
    echo "set output \"${DIR}_${TERM[t]}.pdf\"" >> ${TMP}
    if [ $# -eq 2 ]; then
        echo "set key off" >> ${TMP}
    else
        echo "set key below samplen 1" >> ${TMP}
    fi
    echo "set title \"${DIR}_${TERM[t]} Histogram\"" >> ${TMP}
    echo "set xlabel \"Depth Complexity\" " >> ${TMP}
    echo "set ylabel \"Frequency\"" >> ${TMP}
    DIV_CAR="plot"
    for d in $( ls Tests/${DIR}/${TYPE[t]} | sort -n );
    do
        if [ $d -lt ${LOWER_LIMIT[t]} ]; then continue; fi
        if [ $d -gt ${UPPER_LIMIT[t]} ]; then break; fi
        echo -n "${DIV_CAR} \"Tests/${DIR}/${TYPE[t]}/$d/hist.txt\" using 1:(log(\$2+1)+1) every ::1 w lines title '$d'" >> ${TMP}
	    DIV_CAR=","
    done
    echo "" >> ${TMP}
    gnuplot ${TMP}
    rm ${TMP}
done

