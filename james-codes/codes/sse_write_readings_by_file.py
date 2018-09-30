#!/bin/bash
#PBS -l nodes=1:ppn=16
#PBS -l walltime=24:00:00
#PBS -N JE
#PBS -j oe
#PBS -q cluster.q
#PBS -V

DIR=/users/james/SSE

${DIR}/spark/bin/spark-submit ${DIR}/codes/sse_write_readings_by_file.py

