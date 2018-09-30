#!/bin/bash

for i in `seq 1 51`;
do
   qsub -v NUMBERARG=$i kickOff_get_complete_slots_mpi.pbs
done    
       
