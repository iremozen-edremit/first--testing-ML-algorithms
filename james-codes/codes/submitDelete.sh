#!/bin/bash
for i in $(seq -f "%.0f" 915352 915364);
do
    qdel $i
done    
        
