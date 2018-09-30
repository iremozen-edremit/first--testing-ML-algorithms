#!/bin/bash

for i in `seq 0 9`;
do
   qsub -v NUMBERARG=$i kickOff_get_complete_data_ids.pbs
done    
       
