#!/bin/bash

# Receives path to models and runs DC over them, respecting given time limit (in seconds).
# Usage:
# $ doTest.sh model_dir time_limit

MODEL_DIR=$1
TIME_LIMIT=$2

TEST_DIR="Tests"

echo ${MODEL_DIR} ${TIME_LIMIT} ${MODELS}

TYPE[0]="Normal"
TYPE[1]="Random"
EXEC_NAME[0]="depthcomplexity3d_offline"
EXEC_NAME[1]="randomdepthcomplexity3d_offline"
for m in $MODEL_DIR/*
do
	for i in 0 1
	do
		NAME="$(basename ${m})"
		echo "Current model is ${NAME}"
		CURR_DIR=${TEST_DIR}/${NAME}_${TIME_LIMIT}/${TYPE[i]}
		EXIT=0
		DSTEPS=3
		ITH=100
		BEST[i]=0
		until [ $EXIT -eq 1 ]; do
			echo "Current DSTEPS is ${DSTEPS}"
			timeout ${TIME_LIMIT} ./${EXEC_NAME[i]} -f ${MODEL_DIR}/${NAME} -dsteps ${DSTEPS} -fh "hist.txt" -fr "bestRays.off" -cmr true -it ${ITH}
			TIMEOUT=$?
			if [ $TIMEOUT -eq 124 ]; then
				EXIT=1
			else
				ITH=$((`awk 'END { print NR }' hist.txt`-2))
				if [ ${BEST[i]} -lt $ITH ]; then
					BEST[i]=$ITH
				fi
				ITH=$((ITH-(ITH/5)))
				mkdir -p  ${CURR_DIR}/${DSTEPS}
				mv hist.txt bestRays.off rays* ${CURR_DIR}/${DSTEPS}
				let DSTEPS+=1
			fi
		done
	done
	echo "BEST USUAL: ${BEST[0]}" > ${TEST_DIR}/${NAME}_${TIME_LIMIT}/comp.txt
	echo "BEST RANDOM: ${BEST[1]}" >> ${TEST_DIR}/${NAME}_${TIME_LIMIT}/comp.txt
done

