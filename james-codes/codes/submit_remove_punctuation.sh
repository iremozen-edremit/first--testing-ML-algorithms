#!/bin/bash
for i in `seq 5000 5801`;
do
   qsub -v NUMBERARG=$i kickOff_remove_punctuation.pbs
done    
       
