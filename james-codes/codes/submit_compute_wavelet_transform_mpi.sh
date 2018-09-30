#!/bin/bash


for i in `seq 1 39`;
do
   qsub -v NUMBERARG=$i kickOff_compute_wavelet_transform_mpi.pbs
done    
       
