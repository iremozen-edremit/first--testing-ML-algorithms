#!/bin/bash


for i in `seq 1 39`;
do
   qsub -v NUMBERARG=$i kickOff_compute_variability_mpi.pbs
done    
       
